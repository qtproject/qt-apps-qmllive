/****************************************************************************
**
** Copyright (C) 2016 Pelagicore AG
** Contact: http://www.qt.io/ or http://www.pelagicore.com/
**
** This file is part of the QmlLive tool.
**
** $QT_BEGIN_LICENSE:GPL3-PELAGICORE$
** Commercial License Usage
** Licensees holding valid commercial Pelagicore QmlLive
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

#include <QUdpSocket>
#include "remotelogger.h"

/*!
 * \class RemoteLogger
 * \brief Installs a qt messageHandler and sends the logs over udp
 * \group qmllive
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

void RemoteLogger::broadcast(int type, const QString &msg, const QUrl &url, int line, int column)
{
    QByteArray datagram;
    QStringList data;
    data << QString::number(type) << msg << url.toString() << QString::number(line) << QString::number(column);

    datagram = data.join("%%%").toUtf8();
    m_socket->writeDatagram(datagram.data(), datagram.size(),
                            m_host, m_port);
}

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
