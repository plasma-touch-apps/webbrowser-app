/*
 * Copyright 2013-2015 Canonical Ltd.
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
import ".."

ChromeBase {
    id: chrome

    property alias searchUrl: addressbar.searchUrl
    readonly property string text: addressbar.text
    property alias bookmarked: addressbar.bookmarked
    property list<Action> drawerActions
    readonly property bool drawerOpen: internal.openDrawer
    property alias requestedUrl: addressbar.requestedUrl
    property alias addressBarCanSimplifyText: addressbar.canSimplifyText
    property alias incognito: addressbar.incognito

    backgroundColor: incognito ? UbuntuColors.darkGrey : Theme.palette.normal.background

    function addressBarSelectAll() { addressbar.selectAll() }

    FocusScope {
        anchors {
            fill: parent
            margins: units.gu(1)
        }

        focus: true

        ChromeButton {
            id: backButton
            objectName: "backButton"

            iconName: "previous"
            iconSize: 0.4 * height
            iconColor: internal.iconColor

            height: chrome.height
            width: height * 0.8

            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }

            enabled: chrome.webview ? chrome.webview.canGoBack : false
            onTriggered: chrome.webview.goBack()
        }

        ChromeButton {
            id: forwardButton
            objectName: "forwardButton"

            iconName: "next"
            iconSize: 0.4 * height
            iconColor: internal.iconColor

            height: chrome.height
            visible: enabled
            width: visible ? height * 0.8 : 0

            anchors {
                left: backButton.right
                verticalCenter: parent.verticalCenter
            }

            enabled: chrome.webview ? chrome.webview.canGoForward : false
            onTriggered: chrome.webview.goForward()
        }

        AddressBar {
            id: addressbar

            focus: true

            anchors {
                left: forwardButton.right
                leftMargin: units.gu(1)
                right: drawerButton.left
                rightMargin: units.gu(1)
                verticalCenter: parent.verticalCenter
            }

            icon: (chrome.webview && !chrome.webview.certificateError) ? chrome.webview.icon : ""

            loading: chrome.webview ? chrome.webview.loading : false

            onValidated: {
                chrome.webview.forceActiveFocus()
                chrome.webview.url = requestedUrl
            }
            onRequestReload: {
                chrome.webview.forceActiveFocus()
                chrome.webview.reload()
            }
            onRequestStop: chrome.webview.stop()

            Connections {
                target: chrome.webview
                onUrlChanged: {
                    // ensure that the URL actually changes so that the
                    // address bar is updated in case the user has entered a
                    // new address that redirects to where she previously was
                    // (https://bugs.launchpad.net/webbrowser-app/+bug/1306615)
                    addressbar.actualUrl = ""
                    addressbar.actualUrl = chrome.webview.url
                }
            }

            Connections {
                target: chrome
                onYChanged: addressbar.hideSecurityCertificateDetails()
            }
        }

        ChromeButton {
            id: drawerButton
            objectName: "drawerButton"

            iconName: "contextual-menu"
            iconSize: 0.5 * height
            iconColor: internal.iconColor

            height: chrome.height
            width: height * 0.8

            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }

            onTriggered: {
                if (!internal.openDrawer) {
                    internal.openDrawer = drawerComponent.createObject(chrome)
                    internal.openDrawer.opened = true
                }
            }
        }
    }

    QtObject {
        id: internal
        property var openDrawer: null
        readonly property color iconColor: chrome.incognito ? "white" : "grey"
    }

    onWebviewChanged: {
        if (webview) {
            addressbar.actualUrl = webview.url
            addressbar.securityStatus = webview.securityStatus
        } else {
            addressbar.actualUrl = ""
            addressbar.securityStatus = null
        }
    }

    Component {
        id: drawerComponent

        Item {
            id: drawer
            objectName: "drawer"

            property bool opened: false
            property bool closing: false
            onOpenedChanged: {
                if (!opened) {
                    closing = true
                }
            }

            anchors {
                top: parent.bottom
                right: parent.right
            }
            width: units.gu(22)
            height: actionsColumn.height
            clip: actionsColumn.y != 0

            InverseMouseArea {
                enabled: drawer.opened
                onPressed: drawer.opened = false
            }

            Rectangle {
                anchors.fill: actionsColumn
                color: Theme.palette.normal.background

                Rectangle {
                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                        left: parent.left
                    }
                    width: units.dp(1)
                    color: "#dedede"
                }

                Rectangle {
                    anchors {
                        left: parent.left
                        right: parent.right
                        bottom: parent.bottom
                    }
                    height: units.dp(1)
                    color: "#dedede"
                }
            }

            Column {
                id: actionsColumn

                anchors {
                    left: parent.left
                    right: parent.right
                }

                y: drawer.opened ? 0 : -height
                Behavior on y { UbuntuNumberAnimation {} }
                onYChanged: {
                    if (drawer.closing && (y == -height)) {
                        drawer.destroy()
                    }
                }

                Repeater {
                    model: chrome.drawerActions
                    delegate: AbstractButton {
                        objectName: action.objectName
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        height: units.gu(6)
                        visible: action.enabled

                        action: modelData
                        onClicked: drawer.opened = false

                        Rectangle {
                            anchors.fill: parent
                            color: Theme.palette.selected.background
                            visible: parent.pressed
                        }

                        Icon {
                            id: actionIcon
                            anchors {
                                left: parent.left
                                leftMargin: units.gu(2)
                                verticalCenter: parent.verticalCenter
                            }
                            width: units.gu(2)
                            height: width

                            name: model.iconName
                        }

                        Label {
                            anchors {
                                left: actionIcon.right
                                leftMargin: units.gu(2)
                                verticalCenter: parent.verticalCenter
                                right: parent.right
                                rightMargin: units.gu(1)
                            }
                            text: model.text
                            fontSize: "small"
                            elide: Text.ElideRight
                        }
                    }
                }
            }
        }
    }
}
