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

#pragma once

#include <QTcpSocket>
#include <QUuid>
#include <QQueue>
#include "ipcconnection.h"

class Package;
class IpcClient : public QObject
{
    Q_OBJECT
public:
    explicit IpcClient(QObject *parent = 0);
    IpcClient(QTcpSocket* socket, QObject *parent = 0);

    QAbstractSocket::SocketState state() const;

    void connectToServer(const QString& hostName, int port);
    QUuid send(const QString& method, const QByteArray& data);

    bool waitForConnected(int msecs = 30000);
    bool waitForDisconnected(int msecs = 30000);
    bool waitForSent(const QUuid uuid, int msecs = 30000);

    QString errorToString(QAbstractSocket::SocketError error);

Q_SIGNALS:
    void connected();
    void disconnected();
    void connectionError(QAbstractSocket::SocketError socketError);

    void sentSuccessfully(const QUuid& uuid);
    void sendingError(const QUuid& uuid, QAbstractSocket::SocketError socketError);

    void received(const QString& method, const QByteArray& content);

public Q_SLOTS:
    void disconnectFromServer();

private Q_SLOTS:
    void processQueue();
    void onBytesWritten(qint64 written);
    void onError(QAbstractSocket::SocketError socketError);

private:
    qint64 sendPackage(const QString& method, const QByteArray& data);

    QTcpSocket *m_socket;
    QQueue<Package*> m_queue;
    Package* m_current;
    qint64 m_written;
    QUuid m_lastSuccess;

    IpcConnection* m_connection;
};

