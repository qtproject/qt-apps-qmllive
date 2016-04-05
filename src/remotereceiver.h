/****************************************************************************
**
** Copyright (C) 2016 Pelagicore AG
** Contact: http://www.qt.io/ or http://www.pelagicore.com/
**
** This file is part of the QmlLive tool.
**
** $QT_BEGIN_LICENSE:GPL3-PELAGICORE$
** Commercial License Usage
** Licensees holding valid commercial Pelagicore QmlLive
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

#include <QtCore>
#include <QHostAddress>

#include <QQmlError>

class QTcpSocket;
class LiveNodeEngine;
class IpcServer;
class IpcClient;

class RemoteReceiver : public QObject
{
    Q_OBJECT
public:
    explicit RemoteReceiver(QObject *parent = 0);
    void listen(int port);
    void registerNode(LiveNodeEngine *node);
    void setWorkspaceWriteable(bool on);
    void setWorkspace(const QString &path);
    QString workspace() const;
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

private Q_SLOTS:
    void handleCall(const QString& method, const QByteArray& content);
    void writeDocument(const QString& document, const QByteArray& content);

    void appendToLog(const QList<QQmlError> &errors);

    void onClientConnected(QTcpSocket *socket);
private:
    IpcServer *m_server;
    LiveNodeEngine *m_node;

    QDir m_workspace;
    bool m_workspaceWriteable;

    QString m_pin;
    bool m_connectionAcknowledged;

    QTcpSocket* m_socket;
    IpcClient* m_client;
};

