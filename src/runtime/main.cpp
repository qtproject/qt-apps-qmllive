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

#include <QtGui>
#include <QCryptographicHash>
#include <QtQuick>

#include "livenodeengine.h"
#include "remotereceiver.h"
#include "logger.h"
#include "qmlhelper.h"
#include "qmllive_version.h"

struct Options
{
    Options()
        : ipcPort(10234)
        , updatesAsOverlay(false)
        , updateOnConnect(false)
        , fullscreen(false)
        , transparent(false)
        , frameless(false)
        , stayontop(false)
    {}
    int ipcPort;
    bool updatesAsOverlay;
    bool updateOnConnect;
    QString activeDocument;
    QString workspace;
    QString pluginPath;
    QStringList importPaths;
    bool fullscreen;
    bool transparent;
    bool frameless;
    bool stayontop;
};

static Options options;

static void parseArguments(const QStringList &arguments)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("QmlLive reloading runtime");

    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("workspace", "workspace folder to watch");

    QCommandLineOption ipcPortOption("ipcport", "the port the ipc shall listen on, default is 10234", "ipcport");
    parser.addOption(ipcPortOption);

    QCommandLineOption pluginPathOption("pluginpath", "path to qmllive plugins", "pluginpath");
    parser.addOption(pluginPathOption);

    QCommandLineOption importPathOption("importpath", "path to qml import path. Can appear multiple times", "importpath");
    parser.addOption(importPathOption);

    QCommandLineOption stayOnTopOption("stayontop", "keep viewer window on top");
    parser.addOption(stayOnTopOption);

    QCommandLineOption updatesAsOverlayOption("updates-as-overlay", "allow to receive updates even if workspace is "
                                              "readonly - store updates in a writable overlay");
    parser.addOption(updatesAsOverlayOption);

    QCommandLineOption updateOnConnectOption("update-on-connect", "update all workspace documents initially (blocking).");
    parser.addOption(updateOnConnectOption);

    QCommandLineOption fullScreenOption("fullscreen", "shows in fullscreen mode");
    parser.addOption(fullScreenOption);

    QCommandLineOption transparentOption("transparent", "Make the window transparent");
    parser.addOption(transparentOption);

    QCommandLineOption framelessOption("frameless", "run with no window frame");
    parser.addOption(framelessOption);

    parser.process(arguments);

    if (parser.isSet(ipcPortOption)) {
        options.ipcPort = parser.value(ipcPortOption).toInt();
    }
    options.pluginPath = parser.value(pluginPathOption);
    options.importPaths = parser.values(importPathOption);
    options.stayontop = parser.isSet(stayOnTopOption);
    options.updatesAsOverlay = parser.isSet(updatesAsOverlayOption);
    options.updateOnConnect = parser.isSet(updateOnConnectOption);
    options.fullscreen = parser.isSet(fullScreenOption);
    options.transparent = parser.isSet(transparentOption);
    options.frameless = parser.isSet(framelessOption);

    QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.count() == 1)
        options.workspace = positionalArguments.value(0);
}

class RuntimeLiveNodeEngine : public LiveNodeEngine
{
    Q_OBJECT

public:
    RuntimeLiveNodeEngine()
    {
        connect(this, &LiveNodeEngine::activeWindowChanged,
                this, &RuntimeLiveNodeEngine::onActiveWindowChanged);
    }

private slots:
    void onActiveWindowChanged(QQuickWindow *activeWindow)
    {
        if (activeWindow == 0)
            return;

        if (options.transparent) {
            QSurfaceFormat surfaceFormat;
            surfaceFormat.setAlphaBufferSize(8);
            activeWindow->setFormat(surfaceFormat);
            activeWindow->setClearBeforeRendering(true);
            activeWindow->setColor(QColor(Qt::transparent));
        }

        if (options.stayontop) {
            activeWindow->setFlags(activeWindow->flags() | Qt::X11BypassWindowManagerHint);
            activeWindow->setFlags(activeWindow->flags() | Qt::WindowStaysOnTopHint);
        }

        if (options.frameless) {
            activeWindow->setFlags(activeWindow->flags() | Qt::FramelessWindowHint);
        }

        if (options.fullscreen) {
            activeWindow->setWindowState(Qt::WindowFullScreen);
        }
    }
};

int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("QmlLiveRuntime");
    app.setOrganizationDomain(QLatin1String(QMLLIVE_ORGANIZATION_DOMAIN));
    app.setOrganizationName(QLatin1String(QMLLIVE_ORGANIZATION_NAME));

    parseArguments(app.arguments());

    QQmlEngine qmlEngine;
    qmlEngine.setImportPathList(options.importPaths + qmlEngine.importPathList());

    QQuickView fallbackView(&qmlEngine, 0);

    LiveNodeEngine::WorkspaceOptions workspaceOptions = LiveNodeEngine::LoadDummyData | LiveNodeEngine::AllowUpdates;
    if (options.updatesAsOverlay)
        workspaceOptions |= LiveNodeEngine::UpdatesAsOverlay;

    RemoteReceiver::ConnectionOptions connectionOptions;
    if (options.updateOnConnect)
        connectionOptions |= RemoteReceiver::UpdateDocumentsOnConnect | RemoteReceiver::BlockingConnect;

    RuntimeLiveNodeEngine engine;
    engine.setQmlEngine(&qmlEngine);
    engine.setFallbackView(&fallbackView);
    engine.setWorkspace(options.workspace, workspaceOptions);
    engine.setPluginPath(options.pluginPath);
    engine.loadDocument(QUrl("qrc:/qml/qmlsplash/splash-qt5.qml"));
    RemoteReceiver receiver;
    receiver.registerNode(&engine);
    if (!receiver.listen(options.ipcPort, connectionOptions))
        return EXIT_FAILURE;

    int ret = app.exec();

    return ret;
}

#include "main.moc"
