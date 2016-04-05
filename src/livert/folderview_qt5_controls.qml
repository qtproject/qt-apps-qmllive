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
import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.0

Item {
    id: root
    width: livert.screenWidth
    height: livert.screenHeight

    property color backgroundColor: '#666'
    property color textColor: '#fff'

    Rectangle {
        anchors.fill: parent
        color: root.backgroundColor
    }

    Item {
        id: header

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 32

        z: 0

        Rectangle {
            anchors.fill: parent
            color: "#E0E0E0"
        }
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 8
            anchors.rightMargin: 8
            Repeater {
                model: [
                    { bg: '#000', text: '#fff' },
                    { bg: '#666', text: '#000' },
                    { bg: '#fff', text: '#000' }
                ]
                Button {
                    text: " "
                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: 2
                        color: modelData.bg
                    }
                    onClicked: {
                        root.backgroundColor = modelData.bg
                        root.textColor = modelData.text
                    }
                }
            }
            Item {
                width: 16; height: 16
                Layout.fillWidth: true
            }

            Slider {
                id: slider
                anchors.verticalCenter: parent.verticalCenter
                maximumValue: 300
                value: 190
                minimumValue: 80
            }
        }

    }


    Component {
        id: gridDelegate
        FocusScope {
            id: delegateContainer
            width: grid.cellWidth - 4
            height: grid.cellHeight - 4
            Item {
                id: delegate
                anchors.fill: parent
                anchors.margins: 4

                function loadDocument() {
                    adapter.loadDocument(path + "/" + modelData)
                }

                focus: true

                Keys.onReturnPressed: delegate.loadDocument()
                Keys.onLeftPressed: grid.moveCurrentIndexLeft()
                Keys.onRightPressed: grid.moveCurrentIndexRight()
                Keys.onUpPressed: grid.moveCurrentIndexUp()
                Keys.onDownPressed: grid.moveCurrentIndexDown()

                Image {
                    id: image
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right

                    height: 0.8 * width

                    source: "image://qmlLiveDirectoryPreview/" + path + "/" + modelData

                    fillMode: Image.PreserveAspectFit
                    sourceSize.width: slider.maximumValue
                    sourceSize.height: slider.maximumValue

                    asynchronous: true
                    cache: false
                }

                Text {
                    id: label
                    anchors.top: image.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    text: modelData
                    color: root.textColor

                    textFormat: Text.PlainText
                    elide: Text.ElideMiddle
                    horizontalAlignment: Text.AlignHCenter
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: grid.currentIndex = index;
                    onDoubleClicked: delegate.loadDocument()
                }
            }
        }
    }
    ScrollView {
        id: scrollView
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        //anchors.margins: 30
        GridView {
            id: grid


            cellWidth: slider.value
            cellHeight: slider.value

            keyNavigationWraps: true

            focus: true

            model: files

            delegate: gridDelegate

            highlight: Item {
                id: highlight
                Rectangle {
                    width: grid.cellWidth
                    height: grid.cellHeight
                    color: "#33B5E5"
                    opacity: 0.5
                }
            }
        }
    }

    Component.onCompleted: {
        scrollView.forceActiveFocus();
    }
}
