/*
 * Copyright 2013 Canonical Ltd.
 *
 * This file is part of webbrowser-app.
 *
 * webbrowser-app is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * webbrowser-app is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Qt
#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

// local
#include "history-model.h"
#include "history-domain-model.h"
#include "history-timeframe-model.h"


class HistoryDomainModelTests : public QObject
{
    Q_OBJECT

private:
    HistoryModel* history;
    HistoryTimeframeModel* timeframe;
    HistoryDomainModel* model;

private Q_SLOTS:
    void init()
    {
        history = new HistoryModel;
        history->setDatabasePath(":memory:");
        timeframe = new HistoryTimeframeModel;
        timeframe->setSourceModel(history);
        model = new HistoryDomainModel;
        model->setSourceModel(timeframe);
    }

    void cleanup()
    {
        delete model;
        delete timeframe;
        delete history;
    }

    void shouldBeInitiallyEmpty()
    {
        QCOMPARE(model->rowCount(), 0);
    }

    void shouldNotifyWhenChangingSourceModel()
    {
        QSignalSpy spy(model, SIGNAL(sourceModelChanged()));
        model->setSourceModel(timeframe);
        QVERIFY(spy.isEmpty());
        HistoryTimeframeModel* timeframe2 = new HistoryTimeframeModel(model);
        model->setSourceModel(timeframe2);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(model->sourceModel(), timeframe2);
        model->setSourceModel(0);
        QCOMPARE(spy.count(), 2);
        QCOMPARE(model->sourceModel(), (HistoryTimeframeModel*) 0);
    }

    void shouldNotifyWhenChangingDomain()
    {
        QSignalSpy spy(model, SIGNAL(domainChanged()));
        model->setDomain(QString());
        QVERIFY(spy.isEmpty());
        model->setDomain(QString(""));
        QVERIFY(spy.isEmpty());
        model->setDomain("example.org");
        QCOMPARE(spy.count(), 1);
    }

    void shouldMatchAllWhenNoDomainSet()
    {
        history->add(QUrl("http://example.org"), "Example Domain", QUrl());
        history->add(QUrl("http://example.com"), "Example Domain", QUrl());
        QCOMPARE(model->rowCount(), 2);
    }

    void shouldFilterOutNonMatchingDomains()
    {
        history->add(QUrl("http://example.org/"), "Example Domain", QUrl());
        history->add(QUrl("http://example.com/"), "Example Domain", QUrl());
        model->setDomain("example.org");
        QCOMPARE(model->rowCount(), 1);
        QCOMPARE(model->data(model->index(0, 0), HistoryModel::Url).toUrl(), QUrl("http://example.org/"));
        model->setDomain("example.com");
        QCOMPARE(model->rowCount(), 1);
        QCOMPARE(model->data(model->index(0, 0), HistoryModel::Url).toUrl(), QUrl("http://example.com/"));
        model->setDomain("ubuntu.com");
        QCOMPARE(model->rowCount(), 0);
        model->setDomain("");
        QCOMPARE(model->rowCount(), 2);
    }
};

QTEST_MAIN(HistoryDomainModelTests)
#include "tst_HistoryDomainModelTests.moc"
