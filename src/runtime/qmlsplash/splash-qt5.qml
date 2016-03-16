/****************************************************************************
**
** Copyright (C) 2015 Pelagicore AG
** Contact: http://www.qt.io/ or http://www.pelagicore.com/
**
** This file is part of the QmlLive tool.
**
** $QT_BEGIN_LICENSE:GPL3-PELAGICORE$
** Commercial License Usage
** Licensees holding valid commercial Pelagicore Application Manager
** licenses may use this file in accordance with the commercial license
** agreement provided with the Software or, alternatively, in accordance
** with the terms contained in a written agreement between you and
** Pelagicore. For licensing terms and conditions, contact us at:
** http://www.pelagicore.com.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU General Public License version 3 requirements will be
** met: http://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
** SPDX-License-Identifier: GPL-3.0
**
****************************************************************************/

import QtQuick 2.0

Rectangle {
    id: root
    width: parent.width ? parent.width : 1024
    height: parent.height ? parent.height : 768
    color: "#FF6600"

    Image {
        id: logo
        width: 300
        source: "logo_white.png"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: -100
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 250

        fillMode: Image.PreserveAspectFit
    }

    Text {
        id: mantraText

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: logo.bottom
        anchors.topMargin: -150

        text: "Open Source Infotainment\n    Enabling Great Design"

        font.pixelSize: 60
        color: "white"
        lineHeight: 1.2
    }

    Text {
        id: qmllivetext
        anchors.top: mantraText.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        text: "QmlLive"

        font.pixelSize: 70
        color: "white"
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
