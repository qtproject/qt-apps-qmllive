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

#include <QUdpSocket>
#include "remotelogger.h"

/*!
 * \class RemoteLogger
 * \brief Installs a qt messageHandler and sends the logs over udp
 * \inmodule qmllive
 *
 * \sa Logger, LogReceiver
 */

/*!
 * Standard constructor using \a parent as parent
 */
RemoteLogger::RemoteLogger(QObject *parent) :
    Logger(parent) ,
    m_socket(new QUdpSocket(this)) ,
    m_port(45454)
{
    connect(this, SIGNAL(message(int,QString)), this, SLOT(broadcast(int,QString)));
}

/*!
 * Sets the \a address where the log messages will be sent to
 * \sa setPort()
 */
void RemoteLogger::setHostAddress(const QHostAddress &address)
{
    m_host = address;
}

/*!
 * Sets the \a port where the log messages will be sent to
 * \sa setHostAddress()
 */
void RemoteLogger::setPort(int port)
{
    m_port = port;
}

/*!
 * Broadcasts the data using UDP.
 *
 * With \a type as debug type, \a msg as message content, \a url as document
 * location and \a line and \a column as location inside the document.
 */
void RemoteLogger::broadcast(int type, const QString &msg, const QUrl &url, int line, int column)
{
    QByteArray datagram;
    QStringList data;
    data << QString::number(type) << msg << url.toString() << QString::number(line) << QString::number(column);

    datagram = data.join("%%%").toUtf8();
    m_socket->writeDatagram(datagram.data(), datagram.size(),
                            m_host, m_port);
}

/*!
 * Broadcasts each error from the \a errors
 */
void RemoteLogger::appendToLog(const QList<QQmlError> &errors)
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

        broadcast(type, err.description(), err.url(), err.line(), err.column());
    }
}
