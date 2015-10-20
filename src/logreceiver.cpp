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

#include <QUdpSocket>
#include <QStringList>
#include <QUrl>
#include "logreceiver.h"

/*!
 * \class LogReceiver
 * \brief Connects to a port and waits for log messages sent via udp
 * \group qmllive
 *
 * \sa Logger, RemoteLogger
 */

/*!
 * Standard constructor using \a parent as parent
 */
LogReceiver::LogReceiver(QObject *parent) :
    QObject(parent),
    m_socket(new QUdpSocket(this))
{
    setPort(45454);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
}

/*!
 * Sets the \a port on which we wait for incoming logs
 */
void LogReceiver::setPort(int port)
{
    m_port = port;
}

void LogReceiver::setAddress(const QString &address)
{
    m_address = QHostAddress(address);
}

void LogReceiver::connectToServer()
{
    m_socket->disconnectFromHost();
    m_socket->bind(m_address, m_port, QUdpSocket::ShareAddress);
}

/*!
 * The port on which we wait for incoming logs
 */
int LogReceiver::port() const
{
    return m_port;
}

QString LogReceiver::address() const
{
    return m_address.toString();
}

void LogReceiver::processPendingDatagrams()
{
    while (m_socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_socket->pendingDatagramSize());
        m_socket->readDatagram(datagram.data(), datagram.size());

        QStringList data = QString::fromUtf8(datagram).split("%%%");
        if (data.count() != 5) {
            qWarning("Invalid Log package received");
            return;
        }

        emit message(data.at(0).toInt(), data.at(1), QUrl(data.at(2)) ,data.at(3).toInt(), data.at(4).toInt());
    }
}
