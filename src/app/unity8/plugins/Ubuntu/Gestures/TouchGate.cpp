/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TouchGate.h"

#include <QCoreApplication>
#include <QDebug>

#include <TouchOwnershipEvent.h>
#include <TouchRegistry.h>

#if TOUCHGATE_DEBUG
#include <DebugHelpers.h>
#endif

bool TouchGate::event(QEvent *e)
{
    if (e->type() == TouchOwnershipEvent::touchOwnershipEventType()) {
        touchOwnershipEvent(static_cast<TouchOwnershipEvent *>(e));
        return true;
    } else {
        return QQuickItem::event(e);
    }
}

void TouchGate::touchEvent(QTouchEvent *event)
{
    #if TOUCHGATE_DEBUG
    qDebug() << "[TouchGate] got touch event" << qPrintable(touchEventToString(event));
    #endif
    event->accept();

    const QList<QTouchEvent::TouchPoint> &touchPoints = event->touchPoints();
    bool goodToGo = true;
    for (int i = 0; i < touchPoints.count(); ++i) {
        const QTouchEvent::TouchPoint &touchPoint = touchPoints[i];

        if (touchPoint.state() == Qt::TouchPointPressed) {
            Q_ASSERT(!m_touchInfoMap.contains(touchPoint.id()));
            m_touchInfoMap[touchPoint.id()].ownership = OwnershipRequested;
            m_touchInfoMap[touchPoint.id()].ended = false;
            TouchRegistry::instance()->requestTouchOwnership(touchPoint.id(), this);

            Q_EMIT pressed();
        }

        goodToGo &= m_touchInfoMap.contains(touchPoint.id())
            && m_touchInfoMap[touchPoint.id()].ownership == OwnershipGranted;

        if (touchPoint.state() == Qt::TouchPointReleased && m_touchInfoMap.contains(touchPoint.id())) {
            m_touchInfoMap[touchPoint.id()].ended = true;
        }

    }

    if (goodToGo) {
        if (m_storedEvents.isEmpty()) {
            // let it pass through
            dispatchTouchEventToTarget(event);
        } else {
            // Retain the event to ensure TouchGate dispatches them in order.
            // Otherwise the current event would come before the stored ones, which are older.
            #if TOUCHGATE_DEBUG
            qDebug("[TouchGate] Storing event because thouches %s are still pending ownership.",
                qPrintable(oldestPendingTouchIdsString()));
            #endif
            storeTouchEvent(event);
        }
    } else {
        // Retain events that have unowned touches
        storeTouchEvent(event);
    }
}

void TouchGate::touchOwnershipEvent(TouchOwnershipEvent *event)
{
    // TODO: Optimization: batch those actions as TouchOwnershipEvents
    //       might come one right after the other.

    Q_ASSERT(m_touchInfoMap.contains(event->touchId()));

    TouchInfo &touchInfo = m_touchInfoMap[event->touchId()];

    if (event->gained()) {
        #if TOUCHGATE_DEBUG
        qDebug() << "[TouchGate] Got ownership of touch " << event->touchId();
        #endif
        touchInfo.ownership = OwnershipGranted;
    } else {
        #if TOUCHGATE_DEBUG
        qDebug() << "[TouchGate] Lost ownership of touch " << event->touchId();
        #endif
        m_touchInfoMap.remove(event->touchId());
        removeTouchFromStoredEvents(event->touchId());
    }

    dispatchFullyOwnedEvents();
}

bool TouchGate::isTouchPointOwned(int touchId) const
{
    return m_touchInfoMap[touchId].ownership == OwnershipGranted;
}

void TouchGate::storeTouchEvent(const QTouchEvent *event)
{
    #if TOUCHGATE_DEBUG
    qDebug() << "[TouchGate] Storing" << qPrintable(touchEventToString(event));
    #endif

    TouchEvent clonedEvent(event);
    m_storedEvents.append(std::move(clonedEvent));
}

void TouchGate::removeTouchFromStoredEvents(int touchId)
{
    int i = 0;
    while (i < m_storedEvents.count()) {
        TouchEvent &event = m_storedEvents[i];
        bool removed = event.removeTouch(touchId);

        if (removed && event.touchPoints.isEmpty()) {
            m_storedEvents.removeAt(i);
        } else {
            ++i;
        }
    }
}

void TouchGate::dispatchFullyOwnedEvents()
{
    while (!m_storedEvents.isEmpty() && eventIsFullyOwned(m_storedEvents.first())) {
        TouchEvent event = m_storedEvents.takeFirst();
        dispatchTouchEventToTarget(event);
    }
}

#if TOUCHGATE_DEBUG
QString TouchGate::oldestPendingTouchIdsString()
{
    Q_ASSERT(!m_storedEvents.isEmpty());

    QString str;

    const auto &touchPoints = m_storedEvents.first().touchPoints;
    for (int i = 0; i < touchPoints.count(); ++i) {
        if (!isTouchPointOwned(touchPoints[i].id())) {
            if (!str.isEmpty()) {
                str.append(", ");
            }
            str.append(QString::number(touchPoints[i].id()));
        }
    }

    return str;
}
#endif

bool TouchGate::eventIsFullyOwned(const TouchGate::TouchEvent &event) const
{
    for (int i = 0; i < event.touchPoints.count(); ++i) {
        if (!isTouchPointOwned(event.touchPoints[i].id())) {
            return false;
        }
    }

    return true;
}

void TouchGate::setTargetItem(QQuickItem *item)
{
    // TODO: changing the target item while dispatch of touch events is taking place will
    //       create a mess

    if (item == m_dispatcher.targetItem())
        return;

    m_dispatcher.setTargetItem(item);
    Q_EMIT targetItemChanged(item);
}

void TouchGate::dispatchTouchEventToTarget(const TouchEvent &event)
{
    removeTouchInfoForEndedTouches(event.touchPoints);
    m_dispatcher.dispatch(event.eventType,
            event.device,
            event.modifiers,
            event.touchPoints,
            event.window,
            event.timestamp);
}

void TouchGate::dispatchTouchEventToTarget(QTouchEvent* event)
{
    removeTouchInfoForEndedTouches(event->touchPoints());
    m_dispatcher.dispatch(event->type(),
            event->device(),
            event->modifiers(),
            event->touchPoints(),
            event->window(),
            event->timestamp());
}

void TouchGate::removeTouchInfoForEndedTouches(const QList<QTouchEvent::TouchPoint> &touchPoints)
{
    for (int i = 0; i < touchPoints.size(); ++i) {\
        const QTouchEvent::TouchPoint &touchPoint = touchPoints.at(i);

        if (touchPoint.state() == Qt::TouchPointReleased) {
            Q_ASSERT(m_touchInfoMap.contains(touchPoint.id()));
            Q_ASSERT(m_touchInfoMap[touchPoint.id()].ended);
            Q_ASSERT(m_touchInfoMap[touchPoint.id()].ownership == OwnershipGranted);
            m_touchInfoMap.remove(touchPoint.id());
        }
    }
}

TouchGate::TouchEvent::TouchEvent(const QTouchEvent *event)
    : eventType(event->type())
    , device(event->device())
    , modifiers(event->modifiers())
    , touchPoints(event->touchPoints())
    , target(qobject_cast<QQuickItem*>(event->target()))
    , window(event->window())
    , timestamp(event->timestamp())
{
}

bool TouchGate::TouchEvent::removeTouch(int touchId)
{
    bool removed = false;
    for (int i = 0; i < touchPoints.count() && !removed; ++i) {
        if (touchPoints[i].id() == touchId) {
            touchPoints.removeAt(i);
            removed = true;
        }
    }

    return removed;
}
