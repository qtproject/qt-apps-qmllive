/****************************************************************************
**
** Copyright (C) 2016 Pelagicore AG
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QmlLive tool.
**
** $QT_BEGIN_LICENSE:GPL-QTAS$
** Commercial License Usage
** Licensees holding valid commercial Qt Automotive Suite licenses may use
** this file in accordance with the commercial license agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and The Qt Company.  For
** licensing terms and conditions see https://www.qt.io/terms-conditions.
** For further information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
** SPDX-License-Identifier: GPL-3.0
**
****************************************************************************/

import QtQuick 1.1

Rectangle {
    id: root
    width: 1024
    height: 768
    color: "#FF6600"

    Image {
        id: logo
        source: "pelagicore-symbol-white-rgb.png"
        x: 50
        anchors.top: parent.top
        anchors.topMargin: 100
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 250

        fillMode: Image.PreserveAspectFit
    }

    Text {
        id: mantraText

        text: "Open Source Infotainment\n    Enabling Great Design"

        font.pixelSize: 60
        color: "white"
        lineHeight: 1.2

        y: logo.y
        anchors.left: logo.right
        anchors.leftMargin: 25

    }

    Text {
        id: qmllivetext
        text: "QmlLive"

        font.pixelSize: 70
        color: "white"

        x: mantraText.x
        anchors.top: mantraText.bottom
        anchors.margins: 200
    }
    Text {
        id: pleaseConnectText
        text: "Please connect with QmlLiveRemote"

        font.pixelSize: 40
        color: "white"
        SequentialAnimation {
            running: true
            loops: Animation.Infinite
            ColorAnimation { target: pleaseConnectText; property: "color"; from: "white"; to: root.color; duration: 2000;}
            ColorAnimation { target: pleaseConnectText; property: "color"; from: root.color; to: "white"; duration: 2000;}
        }

        x: mantraText.x
        anchors.top: qmllivetext.bottom
        anchors.margins: 20
    }
}
