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

#include "qmlpreviewadapter.h"
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QLocalSocket>
#include <QDebug>
#include <QProcess>


Q_GLOBAL_STATIC(QProcess, proc)
Q_GLOBAL_STATIC(QString, serverName)


QmlPreviewAdapter::QmlPreviewAdapter(QObject *parent) :
    QObject(parent)
{
}

bool QmlPreviewAdapter::canPreview(const QString &path) const
{
    return path.endsWith(".qml");
}

QImage QmlPreviewAdapter::preview(const QString &path, const QSize &requestedSize)
{
    QImage img;

    if (!requestedSize.isValid()) {
        qWarning() << "preview for" << path << "with invalid size" << requestedSize << "requested";
        return QImage();
    }

    static const QString program = QCoreApplication::applicationDirPath() +
#ifdef Q_OS_WINDOWS
        QStringLiteral("/previewGenerator.exe");
#else
        QStringLiteral("/../libexec/qmllive/previewGenerator");
#endif
    static const QStringList arguments("QmlLiveBench");

    if (proc()->state() != QProcess::Running) {
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        {
            QMutexLocker m(&m_mutex);
            env.insert(QStringLiteral("QT_PLUGIN_PATH"), m_pluginPaths.join(QChar(':')));
            env.insert(QStringLiteral("QML_IMPORT_PATH"), m_importPaths.join(QChar(':')));
            env.insert(QStringLiteral("QML2_IMPORT_PATH"), m_importPaths.join(QChar(':')));
        }
        proc()->setProcessEnvironment(env);

        proc()->setProcessChannelMode(QProcess::ForwardedErrorChannel);

        proc()->start(program, arguments);
        if (!proc()->waitForStarted()) {
            qWarning() << "Failed to start" << program;
            return QImage();
        }

        proc()->waitForReadyRead();
        QByteArray data = proc()->readAll();
        if (!data.startsWith("ready#")) {
            qWarning() << "previewGenerator did not send the \"ready\" token";
            return QImage();
        }
        QString server = QString::fromUtf8(QByteArray::fromHex(data.mid(6)));
        *serverName() = server;
    }

    QLocalSocket socket;

    socket.connectToServer(*serverName());
    if (!socket.waitForConnected(3000)) {
        qWarning() << "Could not connect to PreviewGenerator";
        return QImage();
    }

    //qWarning() << "QL: sending request [" << requestedSize << "]" << path;

    {
        QDataStream ds(&socket);
        ds << requestedSize << path;
    }
    socket.waitForBytesWritten();

    //qWarning() << "QL: sent";

    QByteArray data;
    bool allData = false;
    while (!allData) {
        int bytes = socket.bytesAvailable();
        //qWarning() << "QL: can read" << bytes << "bytes";

        if (proc()->state() != QProcess::Running) {
            qWarning() << "previewGenerator stopped unexpectedly";
            break;
        }

        if (!bytes) {
            //qWarning() << "QL: waiting for more data (have" << data.size() << "already)";
            socket.waitForReadyRead(5000);
            continue;
        }

        data += socket.read(bytes);

        if (data.endsWith("\nEND")) {
            allData = true;
            data.chop(4);
            //qWarning() << "QL: received end marker after"  << data.size() << "bytes";
        }
    }
    socket.close();

    img.loadFromData(data, "PNG");

    if (img.size().isNull()) {
        qWarning() << "Failed to generate Preview";

        img = QImage("://livert/no.png");
    }

    return img;
}

bool QmlPreviewAdapter::canAdapt(const QUrl &url) const
{
    Q_UNUSED(url);

    return false;
}

QUrl QmlPreviewAdapter::adapt(const QUrl &url, QQmlContext *context)
{
    Q_UNUSED(url);
    Q_UNUSED(context);

    return QUrl();
}

void QmlPreviewAdapter::setImportPaths(QStringList importPaths)
{
    QMutexLocker m(&m_mutex);
    m_importPaths = importPaths;
}

void QmlPreviewAdapter::setPluginPaths(QStringList pluginPaths)
{
    QMutexLocker m(&m_mutex);
    m_importPaths = pluginPaths;
}
