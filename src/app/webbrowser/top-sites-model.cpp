/*
 * Copyright 2015 Canonical Ltd.
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

#include "history-model.h"
#include "history-timeframe-model.h"
#include "top-sites-model.h"

/*!
    \class TopSitesModel
    \brief Proxy model that filters a history model based on hidden role and sorts it by number of visits

    TopSitesModel is a proxy model that filters a HistoryTimeframeModel
    based on the hidden rule and sorts it by the number of visits
    (i.e. the history with the greatest number of visits first ignoring entries marked as removed by user).
*/
TopSitesModel::TopSitesModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setSortRole(HistoryModel::Visits);
    sort(0, Qt::DescendingOrder);
    connect(this, SIGNAL(rowsInserted(const QModelIndex&, int, int)), SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsRemoved(const QModelIndex&, int, int)), SIGNAL(countChanged()));
    connect(this, SIGNAL(modelReset()), SIGNAL(countChanged()));
}

HistoryTimeframeModel* TopSitesModel::sourceModel() const
{
    return qobject_cast<HistoryTimeframeModel*>(QSortFilterProxyModel::sourceModel());
}

void TopSitesModel::setSourceModel(HistoryTimeframeModel* sourceModel)
{
    if (sourceModel != this->sourceModel()) {
        QSortFilterProxyModel::setSourceModel(sourceModel);
        Q_EMIT sourceModelChanged();
        Q_EMIT countChanged();
    }
}

bool TopSitesModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    return !sourceModel()->data(index, HistoryModel::Hidden).toBool();
}
