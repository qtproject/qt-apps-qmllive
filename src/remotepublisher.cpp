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
 * \inmodule qmllive
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

/*!
  Return the state of the \l IpcClient

  \sa IpcClient::state()
 */
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
    connect(hub, SIGNAL(publishFile(QString)), this, SLOT(sendDocument(QString)));
    connect(this, SIGNAL(needsPublishWorkspace()), hub, SLOT(publishWorkspace()));
    connect(hub, SIGNAL(beginPublishWorkspace()), this, SLOT(beginBulkSend()));
    connect(hub, SIGNAL(endPublishWorkspace()), this, SLOT(endBulkSend()));
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
 * \sa IpcClient::connectToServer
 */
void RemotePublisher::connectToServer(const QString &hostName, int port)
{
    m_ipc->connectToServer(hostName, port);
}

/*!
  Converts the socket error \a error into a string
 */
QString RemotePublisher::errorToString(QAbstractSocket::SocketError error)
{
    return m_ipc->errorToString(error);
}

/*!
 * Disconnects this publisher from the IPC
 */
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
 * Sends "beginBulkSend()" via IPC.
 */
QUuid RemotePublisher::beginBulkSend()
{
    DEBUG << "RemotePublisher::beginBulkSend";
    return m_ipc->send("beginBulkSend()", QByteArray());
}

/*!
 * Sends "endBulkSend()" via IPC.
 */
QUuid RemotePublisher::endBulkSend()
{
    DEBUG << "RemotePublisher::endBulkSend";
    return m_ipc->send("endBulkSend()", QByteArray());
}

/*!
 * Sends "sendDocument(QString)" using \a document as path to the document to be
 *send to via IPC.
 */
QUuid RemotePublisher::sendDocument(const QString& document)
{
    DEBUG << "RemotePublisher::sendDocument" << document;
    return sendWholeDocument(document);
}

/*!
 Send checkPin with \a pin argument and returns the package uuid.
 */
QUuid RemotePublisher::checkPin(const QString &pin)
{
    DEBUG << "RemotePublisher::checkPin" << pin;
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out << pin;
    return m_ipc->send("checkPin(QString)", bytes);
}

/*!
  Sends the \e setXOffset with \a offset as argument via IPC
 */

QUuid RemotePublisher::setXOffset(int offset)
{
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out << offset;
    return m_ipc->send("setXOffset(int)", bytes);
}

/*!
  Sends the \e setYOffset with \a offset as argument via IPC
 */

QUuid RemotePublisher::setYOffset(int offset)
{
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out << offset;
    return m_ipc->send("setYOffset(int)", bytes);
}

/*!
  Sends the \e setRotation with \a rotation as argument via IPC
 */
QUuid RemotePublisher::setRotation(int rotation)
{
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out << rotation;
    return m_ipc->send("setRotation(int)", bytes);
}

/*!
  Sends the \e sendWholeDocument with \a document as argument via IPC
 */
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
    } else if (method == "needsPublishWorkspace()") {
        emit needsPublishWorkspace();
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
    } else if (method == "clearLog()") {
        emit clearLog();
    }
}

/*!
 * \fn RemotePublisher::connected()
 *
 * The signal is emitted when the IPC is connected
*/

/*!
 * \fn RemotePublisher::disconnected()
 *
 * The signal is emitted when the IPC is disconnected
*/

/*!
 * \fn RemotePublisher::pinOk(bool ok)
 *
 * The signal is emitted after receiving the pinOk IPC call
 * with \a ok to indicate a valid pin
*/

/*!
 * \fn RemotePublisher::connectionError(QAbstractSocket::SocketError error)
 *
 * The signal is emitted when a connection error \a error appears on the IPC
 * level
*/

/*!
 * \fn RemotePublisher::remoteLog(int type, const QString &msg, const QUrl &url = QUrl(), int line = -1, int column = -1)
 *
 *  The signal is emmited after receiving a log call from a remote client. With the \a type, \a msg, \a url,
 * \a line and \a column of the log entry.
 */

/*!
 * \fn RemotePublisher::clearLog()
 *
 * The signal is emmited after receiving a clearLog call from a remote client.
 */

/*!
 * \fn RemotePublisher::needsPinAuthentication()
 *
 * The signal is emitted after receiving the needsPinAuthentication IPC call,
 * to indicate the client requires a pin authentication to continue.
 */

/*!
 * \fn RemotePublisher::needsPublishWorkspace()
 *
 * The signal is emitted after receiving the needsPublishWorkspace IPC call,
 * to indicate the client asks for (re)sending all workspace documents.
 */

/*! \fn RemotePublisher::sentSuccessfully(const QUuid& uuid)
 *
 * The signal is emitted after the package identified by \a uuid has been send
 */

/*!
 * \fn RemotePublisher::sendingError(const QUuid &uuid, QAbstractSocket::SocketError socketError)
 *
 * The signal is emitted when an error occurred while sending a package \a uuid
 * with the error \a socketError
 */
