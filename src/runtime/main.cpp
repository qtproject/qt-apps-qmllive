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

static void usage()
{
    qWarning("Usage qmlliveruntime [options] <workspace>");
    qWarning(" ");
    qWarning(" options:");
    qWarning("  -ipcport <port> ....................the port the ipc shall listen on");
    qWarning("  -no-file-updates ...................do not write to files on changes");
    qWarning("  -pluginpath ........................path to qmllive plugins");
    qWarning("  -importpath ........................path to the qml import path");
    qWarning("  -fullscreen ........................shows in fullscreen mode");
    qWarning("  -transparent .......................Make the window transparent");
    qWarning("  -frameless .........................run with no window frame");
    qWarning("  -stayontop .........................keep viewer window on top");
    qWarning(" ");
    exit(1);
}

static void parseArguments(const QStringList& arguments)
{
    for (int i = 1; i < arguments.count(); ++i) {
        bool lastArg = (i == arguments.count() - 1);
        QString arg = arguments.at(i);
        if (arg == QLatin1String("-ipcport")) {
            if (lastArg || arguments.at(++i).startsWith(QLatin1Char('-'))) usage();
            options.ipcPort = arguments.at(i).toInt();
        } else if (arg == QLatin1String("-pluginpath")) {
            if (lastArg || arguments.at(++i).startsWith(QLatin1Char('-'))) usage();
            options.pluginPath = arguments.at(i);
        } else if (arg == QLatin1String("-importpath")) {
            if (lastArg || arguments.at(++i).startsWith(QLatin1Char('-'))) usage();
            options.importPaths.append(QDir(arguments.at(i)).absolutePath());
            continue;
        } else if (arg == QLatin1String("-no-file-updates")) {
            options.allowUpdates = false;
        } else if (arg == QLatin1String("-fullscreen")) {
            options.fullscreen = true;
        } else if (arg == QLatin1String("-transparent")) {
            options.transparent = true;
        } else if (arg == QLatin1String("-frameless")) {
            options.frameless = true;
        } else if (arg == QLatin1String("-stayontop")) {
            options.stayontop = true;
        } else if (!arg.startsWith(QLatin1Char('-'))) {
            options.workspace = arg;
        } else if (true || arg == QLatin1String("-help")) {
            usage();
        }
    }
}

class RuntimeLiveNodeEngine : public LiveNodeEngine {

    virtual QQuickView* initView()
    {
        QQuickView* view = new QQuickView();
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

        if (options.frameless)
            view->setFlags(view->flags() | Qt::FramelessWindowHint);

        if (options.fullscreen)
            view->setWindowState(Qt::WindowFullScreen);

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

    QMap<QString, QString> extraSyslog;
    extraSyslog.insert("LOCATION", QString("qmllive://${HOST}:%1").arg(options.ipcPort));

    int ret = app.exec();

    return ret;
}
