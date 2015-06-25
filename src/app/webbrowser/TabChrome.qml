/*
 * Copyright 2014-2015 Canonical Ltd.
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

Item {
    property alias title: label.text

    signal selected()
    signal closed()

    implicitHeight: units.gu(4)

    Row {
        anchors.fill: parent

        AbstractButton {
            id: closeButton
            objectName: "closeButton"

            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            width: units.gu(5)

            Rectangle {
                anchors.fill: parent
                color: closeButton.pressed ? Qt.rgba(1.0, 1.0, 1.0, 0.9) : "white"
            }

            Icon {
                height: units.gu(2)
                width: height
                anchors.centerIn: parent
                name: "close"
            }

            onTriggered: closed()

            Rectangle {
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                }
                width: units.dp(1)

                color: "#d9d9d9"
            }
        }

        Item {
            width: parent.width - closeButton.width
            anchors {
                top: parent.top
                bottom: parent.bottom
            }

            Image {
                id: tabBackgroundLeft
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                    right: tabBackgroundCenter.left
                }
                source: "assets/tab-header-left.png"
                fillMode: Image.TileHorizontally
            }

            Image {
                id: tabBackgroundCenter
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    horizontalCenter: parent.horizontalCenter
                }
                source: "assets/tab-header-center.png"
            }

            Image {
                id: tabBackgroundRight
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: tabBackgroundCenter.right
                    right: parent.right
                }
                source: "assets/tab-header-right.png"
                fillMode: Image.TileHorizontally
            }

            Label {
                id: label
                anchors {
                    fill: tabBackgroundLeft
                    leftMargin: units.gu(1)
                }
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }

            MouseArea {
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                }
                // The activable part of the tab occupies its leftmost half
                width: parent.width / 2

                onClicked: selected()
            }
        }
    }

    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: units.dp(1)

        color: "#d9d9d9"
    }
}
