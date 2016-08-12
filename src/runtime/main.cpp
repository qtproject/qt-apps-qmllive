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
#include <QtWidgets>

#include "livenodeengine.h"
#include "remotereceiver.h"
#include "logger.h"
#include "qmlhelper.h"


struct Options
{
    Options()
        : ipcPort(10234)
        , allowUpdates(true)
        , fullscreen(false)
        , transparent(false)
        , frameless(false)
        , stayontop(false)
    {}
    int ipcPort;
    bool allowUpdates;
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

    QCommandLineOption noFileUpdatesOption("no-file-updates", "do not write to files on changes");
    parser.addOption(noFileUpdatesOption);

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
    options.allowUpdates = !parser.isSet(noFileUpdatesOption);
    options.fullscreen = parser.isSet(fullScreenOption);
    options.transparent = parser.isSet(transparentOption);
    options.frameless = parser.isSet(framelessOption);

    QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.count() == 1)
        options.workspace = positionalArguments.value(0);
}


class RuntimeLiveNodeEngine : public LiveNodeEngine {

    virtual QQuickView *initView()
    {
        QQuickView *view = new QQuickView();
        if (options.transparent) {
            QSurfaceFormat surfaceFormat;
            surfaceFormat.setAlphaBufferSize(8);
            view->setFormat(surfaceFormat);
            view->setClearBeforeRendering(true);
            view->setColor(QColor(Qt::transparent));
        }

        if (options.stayontop) {
            view->setFlags(view->flags() | Qt::X11BypassWindowManagerHint);
            view->setFlags(view->flags() | Qt::WindowStaysOnTopHint);
        }

        if (options.frameless) {
            view->setFlags(view->flags() | Qt::FramelessWindowHint);
        }

        if (options.fullscreen) {
            view->setWindowState(Qt::WindowFullScreen);
        }

        return view;
    }
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setApplicationName("QmlLiveRuntime");
    app.setOrganizationDomain("pelagicore.com");
    app.setOrganizationName("Pelagicore");

    parseArguments(app.arguments());

    QQuickView view;

    QStringList defaultImports = view.engine()->importPathList();

    RuntimeLiveNodeEngine engine;
    engine.setUpdateMode(LiveNodeEngine::RecreateView);
    engine.setWorkspace(options.workspace);
    engine.setPluginPath(options.pluginPath);
    engine.setImportPaths(options.importPaths + defaultImports);
    engine.loadDocument(QUrl("qrc:/qml/qmlsplash/splash-qt5.qml"));
    RemoteReceiver receiver;
    receiver.listen(options.ipcPort);
    receiver.setWorkspace(options.workspace);
    receiver.setWorkspaceWriteable(options.allowUpdates);
    receiver.registerNode(&engine);

    engine.connect(&engine, SIGNAL(logErrors(QList<QQmlError>)), &receiver, SLOT(appendToLog(QList<QQmlError>)));

    receiver.connect(&receiver, SIGNAL(xOffsetChanged(int)), &engine, SLOT(setXOffset(int)));
    receiver.connect(&receiver, SIGNAL(yOffsetChanged(int)), &engine, SLOT(setYOffset(int)));
    receiver.connect(&receiver, SIGNAL(rotationChanged(int)), &engine, SLOT(setRotation(int)));

    int ret = app.exec();

    return ret;
}
