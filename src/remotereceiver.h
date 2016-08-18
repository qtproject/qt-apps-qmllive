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

class LiveNodeEngine;
class IpcServer;
class IpcClient;

QT_FORWARD_DECLARE_CLASS(QTcpSocket);

class RemoteReceiver : public QObject
{
    Q_OBJECT
public:
    explicit RemoteReceiver(QObject *parent = 0);
    void listen(int port);
    void registerNode(LiveNodeEngine *node);
    void setWorkspaceWriteable(bool on);
    QString workspace() const;
    void setPin(const QString& pin);
    QString pin() const;

    void setMaxConnections(int max);

public Q_SLOTS:
    void setWorkspace(const QString &path);

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

