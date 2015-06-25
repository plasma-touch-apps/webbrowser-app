/*
 * Copyright 2013-2014 Canonical Ltd.
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
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem

Item {
    id: root

    property var model

    signal accountSelected(QtObject accountServiceHandle)

    ListView {
        id: accounts

        anchors.fill: parent

        model: root.model

        header: ListItem.Header {
            text: i18n.tr("Select an account");
        }

        delegate: AccountItemView {
            visible: enabled

            accountName: providerName + ": " + displayName

            onClicked: {
                root.accountSelected(accountServiceHandle)
            }
        }
    }
}


