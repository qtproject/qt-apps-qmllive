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
 * \enum RemoteReceiver::ConnectionOption
 *
 * This enum type is used to select optional connection related features:
 *
 * \value NoConnectionOption
 *        No optional feature is enabled.
 * \value UpdateDocumentsOnConnect
 *        The remote publisher will be asked to publish all workspace files on
 *        connect. This applies to the very first connection only.
 * \value BlockingConnect
 *        Call to \l listen() will block until a connection from remote publisher
 *        is open and (optional) PIN exchange and (optional) initial documents
 *        update finishes.
 *
 * \sa listen()
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
    , m_bulkUpdateInProgress(false)
    , m_updateDocumentsOnConnectState(UpdateNotStarted)
{
    connect(m_server, SIGNAL(received(QString,QByteArray)), this, SLOT(handleCall(QString,QByteArray)));
    connect(m_server, SIGNAL(clientConnected(QTcpSocket*)), this, SLOT(onClientConnected(QTcpSocket*)));
    connect(m_server, SIGNAL(clientConnected(QHostAddress)), this, SIGNAL(clientConnected(QHostAddress)));
    connect(m_server, SIGNAL(clientDisconnected(QHostAddress)), this, SIGNAL(clientDisconnected(QHostAddress)));
}

/*!
 * Listens on remote publisher connections on \a port with given \a options. If
 * \a options contains BlockingConnect the return value indicates whether PIN
 * exchange and/or initial documents update was successful. Otherwise the
 * return value is always \c true.
 */
bool RemoteReceiver::listen(int port, ConnectionOptions options)
{
    m_connectionOptions = options;
    m_server->listen(port);

    if (m_connectionOptions & BlockingConnect) {
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
        qWarning() << "Waiting for connection from QML Live bench…";
#else
        qInfo() << "Waiting for connection from QML Live bench…";
#endif

        QEventLoop loop;

        if (!m_pin.isEmpty()) {
            bool pinOk = false;
            connect(this, &RemoteReceiver::pinOk, [&loop, &pinOk](bool ok) {
                pinOk = ok;
                loop.quit();
            });
            loop.exec();
            if (!pinOk)
                return false;
        }

        if (m_connectionOptions & UpdateDocumentsOnConnect) {
            bool finishedOk = false;
            connect(this, &RemoteReceiver::updateDocumentsOnConnectFinished, [&loop, &finishedOk](bool ok) {
                finishedOk = ok;
                loop.quit();
            });
            loop.exec();
            if (!finishedOk)
                return false;
        }
    }

    return true;
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
            maybeStartUpdateDocumentsOnConnect();
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
    } else if (method == "beginBulkSend()") {
        if (!m_bulkUpdateInProgress) {
            m_bulkUpdateInProgress = true;
            emit beginBulkUpdate();
            if (m_updateDocumentsOnConnectState == UpdateRequested)
                m_updateDocumentsOnConnectState = UpdateStarted;
        } else {
            qCritical() << "Ignoring nested 'beginBulkSend()' call";
        }
    } else if (method == "endBulkSend()") {
        if (m_bulkUpdateInProgress) {
            m_bulkUpdateInProgress = false;
            emit endBulkUpdate();
            if (m_updateDocumentsOnConnectState == UpdateStarted) {
                m_updateDocumentsOnConnectState = UpdateFinished;
                emit updateDocumentsOnConnectFinished(true);
            }
        } else {
            qCritical() << "Ignoring unpaired 'endBulkSend()' call";
        }
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
        maybeStartUpdateDocumentsOnConnect();
    }
}

void RemoteReceiver::onClientDisconnected(QTcpSocket *socket)
{
    Q_ASSERT(socket == m_socket);
    Q_UNUSED(socket);

    if (m_updateDocumentsOnConnectState != UpdateNotStarted) {
        if (m_updateDocumentsOnConnectState != UpdateFinished) {
            emit updateDocumentsOnConnectFinished(false);
            m_updateDocumentsOnConnectState = UpdateFinished;
        }
    }
    if (m_bulkUpdateInProgress)
        emit endBulkUpdate();
}
void RemoteReceiver::maybeStartUpdateDocumentsOnConnect()
{
    if (!(m_connectionOptions & UpdateDocumentsOnConnect))
        return;

    if (m_updateDocumentsOnConnectState == UpdateNotStarted) {
        m_client->send("needsPublishWorkspace()", QByteArray());
        m_updateDocumentsOnConnectState = UpdateRequested;
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
 * \fn void RemoteReceiver::beginBulkUpdate()
 *
 * This signal is emitted before an expected sequence of \l updateDocument emissions.
 */

/*!
 * \fn void RemoteReceiver::endBulkUpdate()
 *
 * This signal is emitted after an expected sequence of \l updateDocument emissions.
 */

/*!
 * \fn void RemoteReceiver::updateDocumentsOnConnectFinished(bool ok)
 *
 * This signal is emitted to notify that the (optional) initial update of all
 * workspace documents finished. \a ok indicates its result.
 *
 * \sa UpdateDocumentsOnConnect
 */

/*!
 * \fn void RemoteReceiver::updateDocument(const QString &document, const QByteArray &content)
 *
 * This signal is emitted to notify that a \a document has changed its \a content
 */
