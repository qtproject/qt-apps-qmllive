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

#include <QGuiApplication>
#include <QLoggingCategory>
#include <QPointer>
#include <QQuickItem>
#include <QQuickItemGrabResult>
#include <QQuickView>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QElapsedTimer>
#include <QDir>
#include <QBuffer>
#include <QLocalServer>
#include <QLocalSocket>
#include <QDebug>
#include <QTextStream>

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
    QGuiApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    if (app.arguments().count() != 2 ||
       (app.arguments().at(1) != "QmlLiveBench") )
        qFatal("This application is managed by QmlLive and is not supposed to be manually started");

    PreviewServer preview;
    if (!preview.listen()) {
        qFatal("Failed to listen to local socket \"QmlLivePreviewGenerator\": %s",
               qPrintable(preview.errorString()));
    }

    QTextStream(stdout) << QLatin1String("ready#") << preview.serverName().toUtf8().toHex() << endl;

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

    QQmlEngine engine;

    engine.setOutputWarningsToStandardError(false);
    QObject::connect(&engine, &QQmlEngine::warnings, [](const QList<QQmlError> &warnings) {
        foreach (const QQmlError &warning, warnings) {
            qCWarning(pg) << warning;
        }
    });

    QPointer<QQmlComponent> component = new QQmlComponent(&engine, QUrl::fromLocalFile(path));
    QPointer<QObject> object = component->create();

    QQuickWindow *window = 0;
    QQuickItem *item = 0;

    if (!component->isReady()) {
        if (component->isLoading()) {
            qCCritical(pg) << "Component did not load synchronously" << path;
        } else {
            qCWarning(pg) << "Failed to load component" << path;
            foreach (const QQmlError &error, component->errors()) {
                qCWarning(pg) << "\t" << error.toString();
            }
        }
    } else if ((window = qobject_cast<QQuickWindow *>(object.data()))) {
        if (!window->contentItem()->childItems().isEmpty()) {
            item = window->contentItem()->childItems().first();
        } else {
            qCWarning(pg) << "Window has no child item:" << path;
        }
    } else if ((item = qobject_cast<QQuickItem *>(object.data()))) {
        QQuickView *view = new QQuickView(&engine, 0);
        view->setContent(QUrl::fromLocalFile(path), component, object);
        window = view;
    } else {
        qCWarning(pg) << "Root object is not a QQuickWindow nor a QQuickItem:" << path;
    }


    if (window) {
        window->setFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint |
                       Qt::CustomizeWindowHint | Qt::WindowDoesNotAcceptFocus); // | Qt::WindowStaysOnBottomHint);
        window->setPosition(-9999999, -9999999);
        window->show();
    }

    if (item) {
        qCDebug(pg) << "About to grab image for" << path;
        if (QSharedPointer<QQuickItemGrabResult> result = item->grabToImage()) {
            QEventLoop loop;
            QObject::connect(result.data(), &QQuickItemGrabResult::ready, &loop, &QEventLoop::quit);
            loop.exec();
            QImage image = result->image();
            if (!image.isNull()) {
                image = image.scaled(expectedSize, Qt::KeepAspectRatio);
                image.save(socket, "PNG");
                qCDebug(pg) << "Sending image";
            } else {
                qCWarning(pg) << "Grabbed a null image for" << path;
            }
        } else {
            qCWarning(pg) << "Failed to grab an image for" << path;
        }
    }

    delete window; // this equals either to object or view
    delete object;
    delete component;

    socket->write("\nEND");
    socket->flush();

    qCDebug(pg) << "Sent END and flushed";
}


#include "main.moc"
