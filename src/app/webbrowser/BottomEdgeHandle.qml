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
import Ubuntu.Gestures 0.1

DirectionalDragArea {
    direction: Direction.Upwards

    // default values taken from unity8’s EdgeDragArea component
    maxDeviation: units.gu(3)
    wideningAngle: 50
    distanceThreshold: units.gu(1.5)
    minSpeed: 0
    maxSilenceTime: 200
    compositionTime: 60

    readonly property real dragFraction: dragging ? Math.min(1.0, Math.max(0.0, sceneDistance / parent.height)) : 0.0
    readonly property var thresholds: [0.05, 0.18, 0.36, 0.54, 1.0]
    readonly property int stage: thresholds.map(function(t) { return dragFraction <= t }).indexOf(true)
}
