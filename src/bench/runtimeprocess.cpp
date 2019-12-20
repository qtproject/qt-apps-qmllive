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

#include "runtimeprocess.h"
#include "livehubengine.h"

RuntimeProcess::RuntimeProcess(QObject *parent, int port) :
   QProcess(parent)
  ,m_doNotConnect(false)
  ,m_port(port)
  ,m_engine(nullptr)
  ,m_document(nullptr)
{
    connect(&m_publisher, &RemotePublisher::connected, this, &RuntimeProcess::connected);
    connect(&m_publisher, &RemotePublisher::connected, this, &RuntimeProcess::onConnected);
    connect(&m_publisher, &RemotePublisher::disconnected, this, &RuntimeProcess::onDisconnected);
    connect(&m_publisher, &RemotePublisher::connectionError, this, &RuntimeProcess::onConnectionError);
    connect(&m_publisher, &RemotePublisher::remoteLog, this, &RuntimeProcess::remoteLog);
    connect(&m_publisher, &RemotePublisher::clearLog, this, &RuntimeProcess::clearLog);

    //transmit log messages to appropriate LogView
    connect(this, &RuntimeProcess::readyReadStandardError, this, &RuntimeProcess::updateErrors);
    connect(this, &RuntimeProcess::readyReadStandardOutput, this, &RuntimeProcess::updateOutput);
    connect(this, &RuntimeProcess::stateChanged, this, &RuntimeProcess::onStateChanged);
}

RuntimeProcess::~RuntimeProcess()
{
    exit(QProcess::NormalExit);
}

void RuntimeProcess::setCurrentFile(const LiveDocument &currentFile)
{
    if (m_document == nullptr) {
        m_document = new LiveDocument(currentFile);
    } else {
        *m_document = currentFile;
    }
    if (m_publisher.state() != QAbstractSocket::ConnectedState) {
        qWarning()<<"RuntimeProcess::setCurrentFile failed: RemotePublisher is not connected";
        return;
    }

    m_publisher.activateDocument(currentFile);
}

void RuntimeProcess::setWorkspace(const QString &workspace)
{
    m_publisher.setWorkspace(workspace);
}

void RuntimeProcess::setLiveHubEngine(LiveHubEngine *engine)
{
    m_engine = engine;

    m_publisher.setWorkspace(m_engine->workspace());

    connect(m_engine.data(), &LiveHubEngine::workspaceChanged, &m_publisher, &RemotePublisher::setWorkspace);
    connect(m_engine.data(), &LiveHubEngine::activateDocument, &m_publisher, &RemotePublisher::activateDocument);
    connect(m_engine.data(), &LiveHubEngine::beginPublishWorkspace, &m_publisher, &RemotePublisher::beginBulkSend);
    connect(m_engine.data(), &LiveHubEngine::endPublishWorkspace, &m_publisher, &RemotePublisher::endBulkSend);
}

void RuntimeProcess::connectToServer()
{
    if (!m_doNotConnect) {
        qInfo()<<"RuntimeProcess::connectToServer()"<< Constants::LOCAL_HOST() <<
                 " port=" << m_port << m_publisher.state();

        if (m_publisher.state() != QAbstractSocket::UnconnectedState)
            return;

        m_publisher.connectToServer(Constants::LOCAL_HOST(), m_port);
    }
}

void RuntimeProcess::connectToServer(int msecs)
{
    if (!m_doNotConnect) {
        qInfo()<<"RuntimeProcess::connectToServer(msecs)"<< Constants::LOCAL_HOST() <<
                 " port=" << m_port << m_publisher.state();
        QThread::msleep(msecs);

        if (m_publisher.state() != QAbstractSocket::UnconnectedState)
            return;

        m_publisher.connectToServer(Constants::LOCAL_HOST(), m_port, msecs);
    }
}

void RuntimeProcess::onConnected()
{
    if (m_publisher.state() == QAbstractSocket::ConnectedState) {
        m_publisher.activateDocument(m_engine->activePath());
    }
}

void RuntimeProcess::onDisconnected()
{
    qInfo()<<"RuntimeProcess::onDisconnected ";
}

void RuntimeProcess::onConnectionError(QAbstractSocket::SocketError error)
{
    qWarning()<<"RuntimeProcess::onConnectionError: Host connection error: "<<error;

    if (m_publisher.state() != QAbstractSocket::ConnectedState)
        onDisconnected();
}

void RuntimeProcess::sendDocument(const LiveDocument &document)
{
    if (m_document == nullptr) {
        m_document = new LiveDocument(document);
    } else {
        *m_document = document;
    }

    if (m_publisher.state() != QAbstractSocket::ConnectedState)
        return;
}

void RuntimeProcess::updateErrors()
{
    QByteArray log = readAllStandardError();
    emit remoteLog(QtCriticalMsg, log.data());
}

void RuntimeProcess::updateOutput()
{
    QByteArray log = readAllStandardOutput();
    emit remoteLog(QtInfoMsg, log.data());
}

int RuntimeProcess::port() const
{
    return m_port;
}

void RuntimeProcess::onStateChanged()
{
    switch (state()) {
    case QProcess::Running:
        m_doNotConnect = false;
        break;
    case QProcess::Starting:
    case QProcess::NotRunning:
        m_doNotConnect = true;
        break;
    default:
        break;
    }
}
