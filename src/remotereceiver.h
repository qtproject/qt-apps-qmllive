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

#include <QtCore>
#include <QHostAddress>

#include <QQmlError>

#include "qmllive_global.h"

class LiveNodeEngine;
class IpcServer;
class IpcClient;

QT_FORWARD_DECLARE_CLASS(QTcpSocket);

class QMLLIVESHARED_EXPORT RemoteReceiver : public QObject
{
    Q_OBJECT

    enum UpdateState
    {
        UpdateNotStarted,
        UpdateRequested,
        UpdateStarted,
        UpdateFinished
    };

public:
    enum ConnectionOption
    {
        NoConnectionOption = 0x0,
        UpdateDocumentsOnConnect = 0x1,
        BlockingConnect = 0x2
    };
    Q_DECLARE_FLAGS(ConnectionOptions, ConnectionOption)
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    Q_FLAG(ConnectionOptions)
#else
    Q_FLAGS(ConnectionOptions)
#endif

public:
    explicit RemoteReceiver(QObject *parent = 0);
    bool listen(int port, ConnectionOptions options = NoConnectionOption);
    void registerNode(LiveNodeEngine *node);
    void setPin(const QString& pin);
    QString pin() const;

    void setMaxConnections(int max);

Q_SIGNALS:
    void activateDocument(const QString& document);
    void reload();
    void clientConnected(const QHostAddress& address);
    void clientDisconnected(const QHostAddress& address);
    void pinOk(bool ok);
    void xOffsetChanged(int offset);
    void yOffsetChanged(int offset);
    void rotationChanged(int rotation);
    void beginBulkUpdate();
    void endBulkUpdate();
    void updateDocumentsOnConnectFinished(bool ok);
    void updateDocument(const QString &document, const QByteArray &content);

private Q_SLOTS:
    void handleCall(const QString& method, const QByteArray& content);

    void appendToLog(const QList<QQmlError> &errors);
    void clearLog();

    void onClientConnected(QTcpSocket *socket);
    void onClientDisconnected(QTcpSocket *socket);
    void maybeStartUpdateDocumentsOnConnect();

private:
    IpcServer *m_server;
    LiveNodeEngine *m_node;

    QString m_pin;
    bool m_connectionAcknowledged;

    QTcpSocket* m_socket;
    IpcClient* m_client;

    ConnectionOptions m_connectionOptions;
    bool m_bulkUpdateInProgress;
    UpdateState m_updateDocumentsOnConnectState;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(RemoteReceiver::ConnectionOptions)
