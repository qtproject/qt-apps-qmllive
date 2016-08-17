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

#include <QApplication>
#include <QLoggingCategory>
#include <QQuickView>
#include <QQmlEngine>
#include <QElapsedTimer>
#include <QDir>
#include <QBuffer>
#include <QLocalServer>
#include <QLocalSocket>
#include <QDebug>

#include <stdio.h>

void handlePreview(QLocalSocket *socket);

class PreviewServer : public QObject
{
    Q_OBJECT
public:
    PreviewServer()
        : m_server(new QLocalServer())
    {
        connect(m_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    }

    bool listen()
    {
        QLocalServer::removeServer("QmlLivePreviewGenerator");
        return m_server->listen("QmlLivePreviewGenerator");
    }

    QString serverName() const
    {
        return m_server->serverName();
    }

    QString errorString() const
    {
        return m_server->errorString();
    }

protected slots:
    void onNewConnection()
    {
        while (m_server->hasPendingConnections()) {
            QLocalSocket *socket = m_server->nextPendingConnection();

            handlePreview(socket);

            delete socket;
        }
    }
private:
    QLocalServer *m_server;
};

Q_DECLARE_LOGGING_CATEGORY(pg)
Q_LOGGING_CATEGORY(pg, "PreviewGenerator", QtWarningMsg)

int main (int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    if (app.arguments().count() != 2 ||
       (app.arguments().at(1) != "QmlLiveBench") )
        qFatal("This application is managed by QmlLive and is not supposed to be manually started");

    PreviewServer preview;
    if (!preview.listen()) {
        qFatal("Failed to listen to local socket \"QmlLivePreviewGenerator\": %s",
               qPrintable(preview.errorString()));
    }

    printf("ready#%s\n", preview.serverName().toUtf8().toHex().constData());
    fflush(stdout);

    return app.exec();
}

void handlePreview(QLocalSocket *socket)
{
    qCDebug(pg) << "Waiting on input";

    socket->waitForReadyRead();

    QDataStream ds (socket);
    QSize expectedSize;
    QString path;

    ds >> expectedSize >> path;

    qCDebug(pg) << "Received" << expectedSize << path;

    //QML Import paths and plugin paths will be set by environment variables
    QQuickView *view = new QQuickView();

    view->engine()->setOutputWarningsToStandardError(false);
    QObject::connect(view->engine(), &QQmlEngine::warnings, [](const QList<QQmlError> &warnings) {
        foreach (const QQmlError &warning, warnings) {
            qCWarning(pg) << warning;
        }
    });

    view->setFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint |
                   Qt::CustomizeWindowHint | Qt::WindowDoesNotAcceptFocus); // | Qt::WindowStaysOnBottomHint);
    view->setPosition(-9999999, -9999999);
    view->setSource(QUrl::fromLocalFile(path));

    if (view->errors().isEmpty()) {
        view->show();

        //Wait until the Window is exposed;
        int timeout = 3000;
        QElapsedTimer timer;
        timer.start();
        while (!view->isExposed()) {
            int remaining = timeout - int(timer.elapsed());
            if (remaining <= 0)
                break;
            QCoreApplication::processEvents(QEventLoop::AllEvents, remaining);
            //Garbage collection (Delivers all DeferredDeleteEvents which are in the pipe)
            QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
        }

        if (view->width() <= 0 || view->height() <= 0) {
            view->setWidth(500);
            view->setHeight(500);
        }

        if (view->rootObject()) {
            QImage img = view->grabWindow();
            if (!img.isNull()) {
                img = img.scaled(expectedSize, Qt::KeepAspectRatio);
                img.save(socket, "PNG");

                qCDebug(pg) << "Sending image";
            }
        }
    }
    delete view;

    socket->write("\nEND");
    socket->flush();

    qCDebug(pg) << "Sent END and flushed";
}


#include "main.moc"
