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

#include "remotereceiver.h"
#include "ipc/ipcserver.h"
#include "ipc/ipcclient.h"
#include "livenodeengine.h"

#include <QTcpSocket>

#ifdef QMLLIVE_DEBUG
#define DEBUG qDebug()
#else
#define DEBUG if (0) qDebug()
#endif


/*!
 * \class RemoteReceiver
 * \brief Receives commands form the remote publisher
 * \inmodule qmllive
 *
 * Receives commands from a remote publisher to publish workspace files and to
 * setup the active document.
 */

/*!
 * Standard Constructor using \a parent as parent
 */
RemoteReceiver::RemoteReceiver(QObject *parent)
    : QObject(parent)
    , m_server(new IpcServer(this))
    , m_node(0)
    , m_connectionAcknowledged(false)
    , m_socket(0)
    , m_client(0)
{
    connect(m_server, SIGNAL(received(QString,QByteArray)), this, SLOT(handleCall(QString,QByteArray)));
    connect(m_server, SIGNAL(clientConnected(QTcpSocket*)), this, SLOT(onClientConnected(QTcpSocket*)));
    connect(m_server, SIGNAL(clientConnected(QHostAddress)), this, SIGNAL(clientConnected(QHostAddress)));
    connect(m_server, SIGNAL(clientDisconnected(QHostAddress)), this, SIGNAL(clientDisconnected(QHostAddress)));
}

/*!
 * Listens on remote publisher connections on \a port
 */
void RemoteReceiver::listen(int port)
{
    m_server->listen(port);
}

/*!
 * Sets the \a pin to access this live node
 */
void RemoteReceiver::setPin(const QString &pin)
{
    m_pin = pin;
}

/*!
 * Returns the current pin
 */
QString RemoteReceiver::pin() const
{
    return m_pin;
}

/*!
 * Set maximum allowed client connection to \a max
 */
void RemoteReceiver::setMaxConnections(int max)
{
    m_server->setMaxConnections(max);
}

/*!
 * Handle RPC calls with \a method and data as \a content
 */
void RemoteReceiver::handleCall(const QString &method, const QByteArray &content)
{
    DEBUG << "RemoteReceiver::handleIpcCall: " << method;

    if (method == "checkPin(QString)") {
        QString pin;
        QDataStream in(content);
        in >> pin;
        if (m_pin == pin && m_client) {
            m_connectionAcknowledged = true;
            emit pinOk(true);
            m_client->send("pinOK(bool)", QByteArray::number(1));
        } else if (m_client) {
            emit pinOk(false);
            m_client->send("pinOK(bool)", QByteArray::number(0));
        }
    }

    if (!m_connectionAcknowledged) {
        qWarning() << "Connecting without Pin Authentication is not allowed";
        return;
    }

    if (method == "setXOffset(int)") {
        int offset;
        QDataStream in(content);
        in >> offset;
        emit xOffsetChanged(offset);
    } else if (method == "setYOffset(int)") {
        int offset;
        QDataStream in(content);
        in >> offset;
        emit yOffsetChanged(offset);
    } else if (method == "setRotation(int)") {
        int rotation;
        QDataStream in(content);
        in >> rotation;
        emit rotationChanged(rotation);
    } else if (method == "sendDocument(QString,QByteArray)") {
        QString document;
        QByteArray data;
        QDataStream in(content);
        in >> document;
        in >> data;
        emit updateDocument(document, data);
    } else if (method == "activateDocument(QString)") {
        QString document;
        QDataStream in(content);
        in >> document;
        qDebug() << "\tactivate document: " << document;
        emit activateDocument(document);
    } else if (method == "ping()") {
        if (m_client)
            m_client->send("pong()", QByteArray());
    }
}

/*!
 * Register the \a node to be notified about changes
 */
void RemoteReceiver::registerNode(LiveNodeEngine *node)
{
    if (m_node) { disconnect(m_node); }
    m_node = node;
    connect(m_node, SIGNAL(logErrors(QList<QQmlError>)), this, SLOT(appendToLog(QList<QQmlError>)));
    connect(m_node, SIGNAL(clearLog()), this, SLOT(clearLog()));
    connect(this, SIGNAL(activateDocument(QString)), m_node, SLOT(setActiveDocument(QString)));
    connect(this, SIGNAL(updateDocument(QString,QByteArray)), m_node, SLOT(updateDocument(QString,QByteArray)));
    connect(this, SIGNAL(xOffsetChanged(int)), m_node, SLOT(setXOffset(int)));
    connect(this, SIGNAL(yOffsetChanged(int)), m_node, SLOT(setYOffset(int)));
    connect(this, SIGNAL(rotationChanged(int)), m_node, SLOT(setRotation(int)));
}

/*!
 * Handles client connection and if required requests pin authentication
 */
void RemoteReceiver::onClientConnected(QTcpSocket *socket)
{
    if (m_client)
        delete m_client;

    m_client = new IpcClient(socket, this);

    m_socket = socket;

    if (!m_pin.isEmpty()) {
        m_client->send("needsPinAuthentication()", QByteArray());
        m_connectionAcknowledged = false;
    } else {
        m_connectionAcknowledged = true;
    }
}

/*!
 * Called to send \a errors to remote for remote logging
 */
void RemoteReceiver::appendToLog(const QList<QQmlError> &errors)
{
    foreach (const QQmlError &err, errors) {
        if (!err.isValid())
            continue;

        QtMsgType type = QtDebugMsg;

        if (err.description().contains(QString::fromLatin1("error"), Qt::CaseInsensitive) ||
            err.description().contains(QString::fromLatin1("is not installed"), Qt::CaseInsensitive) ||
            err.description().contains(QString::fromLatin1("is not a type"), Qt::CaseInsensitive))
            type = QtCriticalMsg;
        else if (err.description().contains(QString::fromLatin1("warning"), Qt::CaseInsensitive))
            type = QtWarningMsg;

        QByteArray bytes;
        QDataStream out(&bytes, QIODevice::WriteOnly);
        out << type;
        out << err.description();
        out << err.url();
        out << err.line();
        out << err.column();

        m_client->send("qmlLog(QtMsgType, QString, QUrl, int, int)", bytes);
    }
}

/*!
 * Called to clear remote logging output
 */
void RemoteReceiver::clearLog()
{
    m_client->send("clearLog()", QByteArray());
}

/*!
 * \fn void RemoteReceiver::activateDocument(const QString& document)
 *
 * This signal is emitted when the remote active document \a document has changed
 */

/*!
 * \fn void RemoteReceiver::reload()
 *
 * This signal is emitted to notify that a relaod is requested by the remote client
 */

/*!
 * \fn void RemoteReceiver::clientConnected(const QHostAddress& address)
 *
 * This signal is emitted when a new client with address \a address has been connected
 */

/*!
 * \fn void RemoteReceiver::clientDisconnected(const QHostAddress& address)
 *
 * This signal is emitted when a client with address \a address has been disconnected
 */

/*!
 * \fn void RemoteReceiver::pinOk(bool ok)
 *
 * This signal is emitted to notiify that the pin entered on the remote side is valid, if \a ok equals true
 */

/*!
 * \fn void RemoteReceiver::xOffsetChanged(int offset)
 *
 * This signal is emitted to notify the view to apply the x-offset \a offset
 */

/*!
 * \fn void RemoteReceiver::yOffsetChanged(int offset)
 *
 * This signal is emitted to notify the view to apply the y-offset \a offset
 */

/*!
 * \fn void RemoteReceiver::rotationChanged(int rotation)
 *
 * This signal is emitted to notify the view to apply the rotation with the angle \a rotation
 */

/*!
 * \fn void RemoteReceiver::updateDocument(const QString &document, const QByteArray &content)
 *
 * This signal is emitted to notify that a \a document has changed its \a content
 */
