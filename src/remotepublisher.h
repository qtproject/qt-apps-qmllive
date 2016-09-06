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
#include <QAbstractSocket>

#include "qmllive_global.h"

class LiveHubEngine;
class IpcClient;

class QMLLIVESHARED_EXPORT RemotePublisher : public QObject
{
    Q_OBJECT
public:
    explicit RemotePublisher(QObject *parent = 0);
    void connectToServer(const QString& hostName, int port);
    QString errorToString(QAbstractSocket::SocketError error);
    QAbstractSocket::SocketState state() const;

    void registerHub(LiveHubEngine *hub);
Q_SIGNALS:
    void connected();
    void disconnected();
    void sentSuccessfully(const QUuid& uuid);
    void sendingError(const QUuid& uuid, QAbstractSocket::SocketError socketError);
    void connectionError(QAbstractSocket::SocketError error);
    void needsPinAuthentication();
    void needsPublishWorkspace();
    void pinOk(bool ok);
    void remoteLog(int type, const QString &msg, const QUrl &url = QUrl(), int line = -1, int column = -1);
    void clearLog();

public Q_SLOTS:
    void setWorkspace(const QString &path);
    void disconnectFromServer();
    QUuid activateDocument(const QString& document);
    QUuid beginBulkSend();
    QUuid endBulkSend();
    QUuid sendDocument(const QString& document);
    QUuid checkPin(const QString& pin);
    QUuid setXOffset(int offset);
    QUuid setYOffset(int offset);
    QUuid setRotation(int rotation);

private Q_SLOTS:
    void handleCall(const QString &method, const QByteArray &content);
    QUuid sendWholeDocument(const QString &document);

    void onSentSuccessfully(const QUuid& uuid);
    void onSendingError(const QUuid& uuid, QAbstractSocket::SocketError socketError);

private:
    IpcClient *m_ipc;
    LiveHubEngine *m_hub;
    QDir m_workspace;

    QHash<QUuid, QString> m_packageHash;
};
