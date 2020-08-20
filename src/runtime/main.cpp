/****************************************************************************
**
** Copyright (C) 2019 Luxoft Sweden AB
** Copyright (C) 2018 Pelagicore AG
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QML Live tool.
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
#include "constants.h"

struct Options
{
    Options()
        : updatesAsOverlay(false)
        , updateOnConnect(false)
        , fullscreen(false)
        , transparent(false)
        , frameless(false)
        , stayontop(false)
        , hideButtons(false)
        , allowCreateMissing(false)
        , windowTitle("QML Live Runtime")
    {
        ipcPort = Constants::DEFAULT_PORT();
    }

    bool updatesAsOverlay;
    bool updateOnConnect;
    bool fullscreen;
    bool transparent;
    bool frameless;
    bool stayontop;
    bool hideButtons;
    bool allowCreateMissing;
    QString windowTitle;
    QString activeDocument;
    QString workspace;
    QString pluginPath;
    QStringList importPaths;
    int ipcPort;
};

static Options options;

static void parseArguments(const QStringList &arguments)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("QML Live reloading runtime");

    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("workspace", "workspace folder to watch");

    QCommandLineOption ipcPortOption("ipcport", QString("the port the IPC shall listen on, default is %1").arg(Constants::DEFAULT_PORT()), "ipcport");
    parser.addOption(ipcPortOption);

    QCommandLineOption pluginPathOption("pluginpath", "path to QML Live plugins", "pluginpath");
    parser.addOption(pluginPathOption);

    QCommandLineOption importPathOption("importpath", "path to QML import path. Can appear multiple times", "importpath");
    parser.addOption(importPathOption);

    QCommandLineOption stayOnTopOption("stayontop", "keep viewer window on top");
    parser.addOption(stayOnTopOption);

    QCommandLineOption updatesAsOverlayOption("updates-as-overlay", "allow to receive updates even if workspace is "
                                              "readonly - store updates in a writable overlay");
    parser.addOption(updatesAsOverlayOption);

    QCommandLineOption updateOnConnectOption("update-on-connect", "update all workspace documents initially (blocking).");
    parser.addOption(updateOnConnectOption);

    QCommandLineOption allowCreateMissingOption("allow-create-missing", "without this option updates are only "
                                                "accepted for existing workspace documents.");
    parser.addOption(allowCreateMissingOption);

    QCommandLineOption fullScreenOption("fullscreen", "shows in fullscreen mode");
    parser.addOption(fullScreenOption);

    QCommandLineOption transparentOption("transparent", "Make the window transparent");
    parser.addOption(transparentOption);

    QCommandLineOption framelessOption("frameless", "run with no window frame");
    parser.addOption(framelessOption);

    QCommandLineOption windowTitleOption("title", "set window title");
    parser.addOption(windowTitleOption);

    QCommandLineOption hideButtonsOption("hidebuttons", "hide window control buttons (close, minimize, maximize)");
    parser.addOption(hideButtonsOption);

    parser.process(arguments);

    if (parser.isSet(ipcPortOption)) {
        options.ipcPort = parser.value(ipcPortOption).toInt();
    }
    options.pluginPath = parser.value(pluginPathOption);
    options.importPaths = parser.values(importPathOption);
    options.stayontop = parser.isSet(stayOnTopOption);
    options.updatesAsOverlay = parser.isSet(updatesAsOverlayOption);
    options.updateOnConnect = parser.isSet(updateOnConnectOption);
    options.allowCreateMissing = parser.isSet(allowCreateMissingOption);
    options.fullscreen = parser.isSet(fullScreenOption);
    options.transparent = parser.isSet(transparentOption);
    options.frameless = parser.isSet(framelessOption);
    options.windowTitle = parser.isSet(windowTitleOption);
    options.hideButtons = parser.isSet(hideButtonsOption);

    QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.count() == 1)
        options.workspace = positionalArguments.value(0);
    if (positionalArguments.count() == 2)
        options.workspace = positionalArguments.value(1);
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

        if (options.hideButtons) {
            activeWindow->setFlags(activeWindow->flags() | Qt::WindowMinMaxButtonsHint);
        }
    }
};

int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);

    app.setApplicationVersion(QString(QMLLIVE_VERSION_STR).append(" ").append(QMLLIVE_REVISION_STR));
    app.setApplicationName("QML Live Runtime");
    app.setOrganizationDomain(QLatin1String(QMLLIVE_ORGANIZATION_DOMAIN));
    app.setOrganizationName(QLatin1String(QMLLIVE_ORGANIZATION_NAME));

    parseArguments(app.arguments());

    app.setApplicationName(options.windowTitle);

    QQmlEngine qmlEngine;
    qmlEngine.setImportPathList(options.importPaths + qmlEngine.importPathList());

    QQuickView fallbackView(&qmlEngine, 0);

    LiveNodeEngine::WorkspaceOptions workspaceOptions = LiveNodeEngine::LoadDummyData | LiveNodeEngine::AllowUpdates;
    if (options.updatesAsOverlay)
        workspaceOptions |= LiveNodeEngine::UpdatesAsOverlay;
    if (options.allowCreateMissing)
        workspaceOptions |= LiveNodeEngine::AllowCreateMissing;

    RemoteReceiver::ConnectionOptions connectionOptions;
    if (options.updateOnConnect)
        connectionOptions |= RemoteReceiver::UpdateDocumentsOnConnect | RemoteReceiver::BlockingConnect;

    RuntimeLiveNodeEngine engine;
    engine.setQmlEngine(&qmlEngine);
    engine.setFallbackView(&fallbackView);
    engine.setWorkspace(options.workspace, workspaceOptions);
    engine.setPluginPath(options.pluginPath);
    RemoteReceiver receiver;
    receiver.registerNode(&engine);
    if (!receiver.listen(options.ipcPort, connectionOptions))
        return EXIT_FAILURE;

    fallbackView.setSource(QUrl("qrc:/qml/qmlsplash/splash-qt5.qml"));

    int ret = app.exec();

    return ret;
}

#include "main.moc"
