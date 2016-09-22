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
#include <QtWidgets>

#include "options.h"
#include "mainwindow.h"
#include "qmllive_version.h"

static void setDarkStyle(QApplication *app)
{
    QStyle *style = QStyleFactory::create("fusion");
    if (!style) {
        return;
    }
    app->setStyle(style);

    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#3D3D3D"));
    palette.setColor(QPalette::WindowText, QColor("#FFFFFF"));
    palette.setColor(QPalette::Base, QColor("#303030"));
    palette.setColor(QPalette::AlternateBase, QColor("#4A4A4A"));
    palette.setColor(QPalette::ToolTipBase, QColor("#FFFFFF"));
    palette.setColor(QPalette::ToolTipText, QColor("#3D3D3D"));
    palette.setColor(QPalette::Text, QColor("#F0F0F0"));
    palette.setColor(QPalette::Button, QColor("#353535"));
    palette.setColor(QPalette::ButtonText, QColor("#FFFFFF"));
    palette.setColor(QPalette::BrightText, QColor("#D0021B"));
    palette.setColor(QPalette::Highlight, QColor("#F19300"));
    palette.setColor(QPalette::HighlightedText, QColor("#1C1C1C"));
    app->setPalette(palette);
}

static void parseArguments(const QStringList& arguments, Options *options)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("QmlLive reloading workbench");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("workspace", "workspace folder to watch. If this points to a QML document, than the directory is asssumed to be the workspace and the file the active document.");
    parser.addPositionalArgument("document", "main QML document to load initially.");

    QCommandLineOption pluginPathOption("pluginpath", "path to qmllive plugins", "pluginpath");
    parser.addOption(pluginPathOption);
    QCommandLineOption importPathOption("importpath", "path to qml import path. Can appear multiple times", "importpath");
    parser.addOption(importPathOption);
    QCommandLineOption stayOnTopOption("stayontop", "keep viewer window on top");
    parser.addOption(stayOnTopOption);

    parser.process(arguments);

    options->setPluginPath(parser.value(pluginPathOption));
    options->setImportPaths(parser.values(importPathOption));
    options->setStayOnTop(parser.isSet(stayOnTopOption));

    const QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.count() >= 1) {
        QString argument = positionalArguments.value(0);
        QFileInfo fi(argument);
        if (argument.endsWith(".qml")) {
            qDebug() << "First argument ends with \".qml\". Assuming it is a file.";
            if (!fi.exists() || !fi.isFile()) {
                qWarning() << "Document does not exist or is not a file: " << fi.absoluteFilePath();
                parser.showHelp(-1);
            }
            options->setWorkspace(fi.absolutePath());
            options->setActiveDocument(fi.absoluteFilePath());
        } else {
            qDebug() << "First argument does not ending with \".qml\". Assuming it is a workspace.";
            if (!fi.exists() || !fi.isDir()) {
                qWarning() << "Workspace does not exist or is not a directory: " << fi.absoluteFilePath();
                parser.showHelp(-1);
            }
            options->setWorkspace(fi.absoluteFilePath());
        }
    }
    if (positionalArguments.count() == 2) {
        QString argument = positionalArguments.value(1);
        QFileInfo fi(argument);
        if (argument.endsWith(".qml")) {
            qDebug() << "Second argument ends with \".qml\". Assuming it is a file.";
            if (!fi.exists() || !fi.isFile()) {
                qWarning() << "Document does not exist or is not a file: " << fi.absoluteFilePath();
                parser.showHelp(-1);
            }
            options->setActiveDocument(fi.absoluteFilePath());
        } else {
            qWarning() << "If second argument is present it needs to be a QML document: " << fi.absoluteFilePath();
            parser.showHelp(-1);
        }
    }
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setApplicationName("QmlLiveBench");
    app.setOrganizationDomain(QLatin1String(QMLLIVE_ORGANIZATION_DOMAIN));
    app.setOrganizationName(QLatin1String(QMLLIVE_ORGANIZATION_NAME));
    app.setAttribute(Qt::AA_NativeWindows, true);
    app.setAttribute(Qt::AA_ImmediateWidgetCreation, true);

    setDarkStyle(&app);

    Options *options = Options::instance();
    parseArguments(app.arguments(), options);

    MainWindow win;
    win.init(options); // Parse and apply command line and settings file options
    win.show();

    return app.exec();
}
