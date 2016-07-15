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

#include "ipcserver.h"
#include "ipcconnection.h"

#ifdef QMLLIVE_IPC_DEBUG
#define DEBUG qDebug()
#else
#define DEBUG if (0) qDebug()
#endif

/*!
 * \class IpcServer
 * \brief The IpcServer listens on a port and creates an IpcConnection for an incoming
 * connection.
 * \inmodule ipc
 *
 * The IPC server receives a method call from the client an notifies the user through
 * the IpcServer::received signal.
 *
 * \code{.cpp}
 * m_server = new IpcServer(this);
 * connect(
 *   m_server, SIGNAL(received(QString,QByteArray)),
 *   this, SLOT(handleCall(QString, QByteArray))
 * );
 * m_server->listen(10234);
 *
 * ...
 *
 * MyHandler::handleCall(const QString& method, const QByteArray& contnt) {
 *   if (method == "echo(QString)") {
 *     QString text;
 *     QDataStream in(content);
 *     in >> text;
 *     qDebug() << "call received: " << method << ": " << text;
 *   }
 * }
 * \endcode
 */

/*!
 * \brief Standard constructor using \a parent as parent object
 */
IpcServer::IpcServer(QObject *parent)
    : QObject(parent)
    , m_server(new QTcpServer(this))
{
    connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

/*!
 * \brief listens to incomong connections on \a port
 */
void IpcServer::listen(int port)
{
    DEBUG << "IpcServer::listen: " << port;
    m_server->listen(QHostAddress::Any, port);
}

/*!
 * \brief Creates a IpcConnection on incoming connection
 */
void IpcServer::newConnection()
{
    DEBUG << "IpcServer::newConnection";
    if (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();
        emit clientConnected(socket->peerAddress());
        emit clientConnected(socket);
        IpcConnection *connection = new IpcConnection(socket, this);
        connect(connection, SIGNAL(connectionClosed()), this, SLOT(onConnectionClosed()));
        connect(connection, SIGNAL(received(QString,QByteArray)), this, SIGNAL(received(QString,QByteArray)));
    }
}


void IpcServer::onConnectionClosed()
{
    IpcConnection *connection = qobject_cast<IpcConnection*>(sender());

    emit clientDisconnected(connection->socket()->peerAddress());
    emit clientDisconnected(connection->socket());

    if (connection->parent() == this)
        delete connection;
}

/*!
 * Sets the maximal nuber of pending connections to \a num
 */
void IpcServer::setMaxConnections(int num)
{
    m_server->setMaxPendingConnections(num);
}


/*!
 * \fn void IpcServer::received(const QString& method, const QByteArray& content)
 * \brief signals a ipc call has arrived
 *
 * A ipc call requesting \a method and using \a content a the parameters for the method
 */

/*!
 * \fn void IpcServer::clientConnected(const QHostAddress& address)
 *
 * Called when a new client connection is established, providing the \a address
 */

/*!
 * \fn void IpcServer::clientConnected(QTcpSocket *socket)
 *
 * * Called when a new client connection is established, providing the \a socket
 */

/*!
 * \fn void IpcServer::clientDisconnected(QTcpSocket *socket)
 *
 * * Called when an existing client connection is dropped, providing the \a socket
 */

/*!
 * \fn void IpcServer::clientDisconnected(const QHostAddress& address)
 *
 * Called when an existing client connection is dropped, providing the \a address
 */
