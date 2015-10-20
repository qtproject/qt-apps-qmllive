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

#pragma once

#include <QObject>
#include <QUrl>
#include <QHostAddress>

class QUdpSocket;
class LogReceiver : public QObject
{
    Q_OBJECT
public:
    explicit LogReceiver(QObject *parent = 0);

    int port() const;
    QString address() const;

public Q_SLOTS:
    void setPort(int port);
    void setAddress(const QString& address);
    void connectToServer();

Q_SIGNALS:
    void message(int type, const QString &msg, const QUrl &url, int line, int column);

private Q_SLOTS:
    void processPendingDatagrams();

private:
    QUdpSocket* m_socket;
    QHostAddress m_address;
    int m_port;
};
