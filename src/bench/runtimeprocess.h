/****************************************************************************
**
** Copyright (C) 2018 Luxoft Sweden AB
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

#ifndef RUNTIMEPROCESS_H
#define RUNTIMEPROCESS_H

#include "constants.h"
#include <QProcess>
#include <remotepublisher.h>

class RuntimeProcess : public QProcess
{
    Q_OBJECT
public:
    explicit RuntimeProcess(QObject *parent = nullptr, int port = Constants::DEFAULT_BENCH_PORT());
    ~RuntimeProcess();

    void setCurrentFile(const LiveDocument &currentFile);
    void setWorkspace(const QString &workspace);
    void setLiveHubEngine(LiveHubEngine *engine);
    void connectToServer();
    void connectToServer(int msecs);
    int port() const;

Q_SIGNALS:
    void connected();
    void remoteLog(int type, const QString &msg, const QUrl &url = QUrl(), int line = -1, int column = -1);
    void clearLog();

private Q_SLOTS:
    void onConnected();
    void onDisconnected();
    void onConnectionError(QAbstractSocket::SocketError error);
    void sendDocument(const LiveDocument &document);
    void updateOutput();
    void updateErrors();
    void onStateChanged();
private:
    bool m_doNotConnect;
    int m_port;
    RemotePublisher m_publisher;
    QPointer<LiveHubEngine> m_engine;
    LiveDocument *m_document;
};

#endif // RUNTIMEPROCESS_H
