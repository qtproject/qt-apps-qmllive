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

#include "remotepublisher.h"
#include "ipc/ipcclient.h"
#include "livehubengine.h"

#ifdef QMLLIVE_DEBUG
#define DEBUG qDebug()
#else
#define DEBUG if (0) qDebug()
#endif
/*!
 * \class RemotePublisher
 * \brief Publishes hub changes to a remote node
 * \group qmllive
 *
 * To see the progress which commands were really sent successfully to to the server
 * you have to connect the signals from the LiveHubEngine yourself and monitor the QUuids you
 * got and wait for sendingError() or sentSuccessfully() signals
 */

/*!
 * Standard Constructor using \a parent as parent
 */
RemotePublisher::RemotePublisher(QObject *parent)
    : QObject(parent)
    , m_ipc(new IpcClient(this))
    , m_hub(0)
{
    connect(m_ipc, SIGNAL(sentSuccessfully(QUuid)), this, SIGNAL(sentSuccessfully(QUuid)));
    connect(m_ipc, SIGNAL(sendingError(QUuid,QAbstractSocket::SocketError)),
            this, SIGNAL(sendingError(QUuid,QAbstractSocket::SocketError)));
    connect(m_ipc, SIGNAL(connectionError(QAbstractSocket::SocketError)),
            this, SIGNAL(connectionError(QAbstractSocket::SocketError)));
    connect(m_ipc, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(m_ipc, SIGNAL(disconnected()), this, SIGNAL(disconnected()));

    connect(m_ipc, SIGNAL(received(QString,QByteArray)), this, SLOT(handleCall(QString,QByteArray)));

    connect(m_ipc, SIGNAL(sentSuccessfully(QUuid)), this, SLOT(onSentSuccessfully(QUuid)));
    connect(m_ipc, SIGNAL(sendingError(QUuid,QAbstractSocket::SocketError)),
            this, SLOT(onSendingError(QUuid,QAbstractSocket::SocketError)));
}

QAbstractSocket::SocketState RemotePublisher::state() const
{
    return m_ipc->state();
}

/*!
 * Register the \a hub to be used with this publisher
 */
void RemotePublisher::registerHub(LiveHubEngine *hub)
{
    if (m_hub) {
        disconnect(m_hub);
    }
    m_hub = hub;
    connect(hub, SIGNAL(activateDocument(QString)), this, SLOT(activateDocument(QString)));
    connect(hub, SIGNAL(fileChanged(QString)), this, SLOT(sendDocument(QString)));
}

/*!
 * Sets the current workspace to \a path. Documents location will be adjusted based on
 * this workspace path.
 */
void RemotePublisher::setWorkspace(const QString &path)
{
    m_workspace = QDir(path);
}

/*!
 * Set Ipc destination to use \a hostName and \a port
 * \sa IpcClient::connectToServer()
 */
void RemotePublisher::connectToServer(const QString &hostName, int port)
{
    m_ipc->connectToServer(hostName, port);
}

QString RemotePublisher::errorToString(QAbstractSocket::SocketError error)
{
    return m_ipc->errorToString(error);
}

void RemotePublisher::disconnectFromServer()
{
    m_ipc->disconnectFromServer();
}

/*!
 * Send "activateDocument(QString)" to ipc-server on activate document.
 * \a document defines the Document which should be activated
 */
QUuid RemotePublisher::activateDocument(const QString &document)
{
    DEBUG << "RemotePublisher::activateDocument" << document;
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out << document;
    return m_ipc->send("activateDocument(QString)", bytes);
}

/*!
 * Send "sendDocument(QString,QByteArray)" to ipc-server on send document.
 * It sends \a document with content \a data
 */
QUuid RemotePublisher::sendDocument(const QString& document)
{
    DEBUG << "RemotePublisher::sendDocument" << document;
    return sendWholeDocument(document);
}

QUuid RemotePublisher::checkPin(const QString &pin)
{
    DEBUG << "RemotePublisher::checkPin" << pin;
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out << pin;
    return m_ipc->send("checkPin(QString)", bytes);
}

QUuid RemotePublisher::setXOffset(int offset)
{
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out << offset;
    return m_ipc->send("setXOffset(int)", bytes);
}

QUuid RemotePublisher::setYOffset(int offset)
{
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out << offset;
    return m_ipc->send("setYOffset(int)", bytes);
}

QUuid RemotePublisher::setRotation(int rotation)
{
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out << rotation;
    return m_ipc->send("setRotation(int)", bytes);
}

QUuid RemotePublisher::sendWholeDocument(const QString& document)
{
    DEBUG << "RemotePublisher::sendWholeDocument" << document;
    QFile file(document);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "ERROR: can't open file: " << document;
        return QUuid();
    }
    QByteArray data = file.readAll();

    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out << m_workspace.relativeFilePath(document);
    out << data;
    return m_ipc->send("sendDocument(QString,QByteArray)", bytes);
}

void RemotePublisher::onSentSuccessfully(const QUuid &uuid)
{
    QString path = m_packageHash.value(uuid);
    m_packageHash.remove(uuid);

    QList<QUuid> keys = m_packageHash.keys(path);
    if (keys.count() == 1) {
        m_packageHash.remove(keys.at(0));
        emit sentSuccessfully(keys.at(0));
    }
}

void RemotePublisher::onSendingError(const QUuid &uuid, QAbstractSocket::SocketError socketError)
{
    QString path = m_packageHash.value(uuid);
    m_packageHash.remove(uuid);

    QList<QUuid> keys = m_packageHash.keys(path);
    if (keys.count() == 1) {
        m_packageHash.remove(keys.at(0));
        emit sendingError(keys.at(0), socketError);
    }
}


void RemotePublisher::handleCall(const QString &method, const QByteArray &content)
{
    DEBUG << "RemotePublisher::handleIpcCall: " << method << content;

    if (method == "needsPinAuthentication()") {
        qDebug() << "needsPinAuthentication";
        emit needsPinAuthentication();
    } else if (method == "pinOK(bool)") {
        qDebug() << "pinOk" << content.toInt();
        emit pinOk(content.toInt());
    } else if (method == "qmlLog(QtMsgType, QString, QUrl, int, int)") {
        int msgType;
        QString description;
        QUrl url;
        int line = -1;
        int column = -1;

        QDataStream in(content);
        in >> msgType;
        in >> description;
        in >> url;
        in >> line;
        in >> column;

        emit remoteLog(msgType, description, url, line, column);
    }
}
