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
#include <QtWidgets>

#include "mainwindow.h"


struct Options
{
    Options()
        : stayontop(false)
    {}

    QString activeDocument;
    QString workspace;
    QString pluginPath;
    QStringList importPaths;
    bool stayontop;
};

static Options options;

static void usage()
{
    qWarning("Usage qmllivebench [options] <workspace> <file.qml>");
    qWarning("Usage qmllivebench [options] <workspace/file.qml>");
    qWarning(" ");
    qWarning(" options:");
    qWarning("  -pluginpath ........................path to qmllive plugins");
    qWarning("  -importpath ........................path to the qml import path");
    qWarning("  -stayontop .........................keep viewer window on top");
    qWarning(" ");
    exit(1);
}

static void parseArguments(const QStringList& arguments)
{
    for (int i = 1; i < arguments.count(); ++i) {
        bool lastArg = (i == arguments.count() - 1);
        QString arg = arguments.at(i);

        if (arg == QLatin1String("-pluginpath")) {
            if (lastArg || arguments.at(++i).startsWith(QLatin1Char('-'))) usage();
            options.pluginPath = arguments.at(i);;
            continue;
        } else if (arg == QLatin1String("-importpath")) {
            if (lastArg || arguments.at(++i).startsWith(QLatin1Char('-'))) usage();
            options.importPaths.append(QDir(arguments.at(i)).absolutePath());
            continue;
        } else if (arg == QLatin1String("-stayontop")) {
            options.stayontop = true;
            continue;
        } else if (arg.startsWith(QLatin1Char('-'))) {
            usage();
            return;
        }

        if (arg.endsWith(".qml")) {
            qDebug() << "Found argument ending with \".qml\". Assuming it's a file.";
            if (options.workspace.isEmpty()) {
                qDebug() << "No workspace is set yet, assuming it's the whole path";
                QFileInfo fi(arg);

                options.workspace = fi.path();

                if (!fi.exists()) {
                    qWarning() << "File does not exist: " << arg << " trying to set workspace anyway...";
                }
                else {
                    options.activeDocument = fi.absoluteFilePath();
                }
            }
            else {
                QFileInfo fi(arg);
                if (!fi.exists())
                    qWarning() << "File does not exist: " << arg << "ignoring it.";
                else
                    options.activeDocument = fi.absoluteFilePath();
            }
        }
        else {
            options.workspace = arg;
        }
    }
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setApplicationName("QmlLiveBench");
    app.setOrganizationDomain("pelagicore.com");
    app.setOrganizationName("Pelagicore");
    app.setAttribute(Qt::AA_NativeWindows, true);
    app.setAttribute(Qt::AA_ImmediateWidgetCreation, true);
    parseArguments(app.arguments());

    //qDebug () << "apply settings...";
    MainWindow win;
    if (!options.workspace.isEmpty()) {
        //qDebug() << "set workspace: " << options.workspace;
        win.setWorkspace(QDir(options.workspace).absolutePath());
    }
    if (!options.pluginPath.isEmpty()) {
        //qDebug() << "set pluginPath: " << options.pluginPath;
        win.setPluginPath(QDir(options.pluginPath).absolutePath());
    }
    if (!options.importPaths.isEmpty()) {
        //qDebug() << "set importPaths: " << options.importPaths;
        win.setImportPaths(options.importPaths);
    }
    if (!options.activeDocument.isEmpty()) {
        //qDebug() << "set active document: " << options.activeDocument;
        win.activateDocument(options.activeDocument);
    }

    if (options.stayontop)
        win.setStaysOnTop(true);

    win.readSettings();

    win.show();

    return app.exec();
}
