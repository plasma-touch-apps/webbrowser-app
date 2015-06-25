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

import QtQuick 2.0
import Ubuntu.OnlineAccounts 0.1
import Ubuntu.OnlineAccounts.Client 0.1

Item {
    id: root
    property string accountProvider: ""
    property string applicationName: ""
    property alias count: accountsModel.count

    signal finished

    function createNewAccount() {
        setup.exec();
    }

    readonly property alias model: accountsModel

    AccountServiceModel {
        id: accountsModel
        includeDisabled: false
        serviceType: "webapps"
        applicationId: root.applicationName
        provider: root.accountProvider
    }

    Setup {
        id: setup
        applicationId: root.applicationName
        providerId: root.accountProvider
        onFinished: root.finished()
    }
}
