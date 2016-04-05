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
#include <QtCore>
#include <QtTest>
#include <QtNetwork>

#include "ipc/ipcserver.h"
#include "ipc/ipcclient.h"

class TestIpc : public QObject
{
    Q_OBJECT

public:
    TestIpc() {}
public slots:
    void handleCall(QString method, QByteArray data) {
        if (method == "echo(QString)") {
            QDataStream stream(data);
            QString message;
            stream >> message;
            qDebug() << method << ": " << message;
        }
        if (method == "sendFile(QString,QByteArray)") {
            QDataStream stream(data);
            QString path;
            stream >> path;
            QByteArray body;
            stream >> body;
            qDebug() << method << ": " << path;
        }
    }

private Q_SLOTS:
    void call() {
        IpcServer peer1;
        peer1.listen(10234);
        connect(&peer1, SIGNAL(received(QString,QByteArray)), this, SLOT(handleCall(QString,QByteArray)));
        IpcClient peer2;
        peer2.setDestination("127.0.0.1", 10234);
        QByteArray bytes;
        QDataStream stream(&bytes, QIODevice::ReadWrite);
        stream << QString("Hello IPC!");
        peer2.send("echo(QString)", bytes);
        QSignalSpy received(&peer1, SIGNAL(received(QString,QByteArray)));
        QTest::qWait(100);
        QVERIFY(received.count() == 1);
    }

    void sendFile() {
        IpcServer peer1;
        peer1.listen(10234);
        connect(&peer1, SIGNAL(received(QString,QByteArray)), this, SLOT(handleCall(QString,QByteArray)));
        IpcClient peer2;
        peer2.setDestination("127.0.0.1", 10234);
        QByteArray bytes;
        QDataStream stream(&bytes, QIODevice::ReadWrite);
        QString filePath("tst_testipc.cpp");
        stream << filePath;
        stream << QString("hello").toLatin1();
        peer2.send("sendFile(QString,QByteArray)", bytes);
        QSignalSpy received(&peer1, SIGNAL(received(QString,QByteArray)));
        QTest::qWait(100);
        QVERIFY(received.count() == 1);
    }
};

QTEST_MAIN(TestIpc)

#include "tst_testipc.moc"
