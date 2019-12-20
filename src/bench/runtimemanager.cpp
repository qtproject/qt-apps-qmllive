/****************************************************************************
**
** Copyright (C) 2018 Luxoft Sweden AB
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

#include "runtimemanager.h"
#include "widgets/logview.h"
#include "constants.h"

#include <QDockWidget>

RuntimeManager::RuntimeManager(QObject *parent) : QObject(parent)
  ,m_port(Constants::DEFAULT_BENCH_PORT())
  ,m_engine(nullptr)
  ,m_currentFile(nullptr)
  ,m_imports(nullptr)
  ,m_pluginPath(nullptr)
{
    QSettings s;
    QString path = s.value(Constants::RUNTIME_SETTINGS_KEY()).toString();
    if (!path.isEmpty()) {
        m_runtimeBinaryPath = path;
    } else {
        // Assuming the qmlliveruntime binary located at the same folder with qmllivebench
        m_runtimeBinaryPath = Constants::DEFAULT_RUNTIME_LOCATION();
    }
}
RuntimeManager::~RuntimeManager()
{
    finishProcesses();
}

void RuntimeManager::setRuntimeBinaryPath(const QString &path)
{
    m_runtimeBinaryPath = path;
}

void RuntimeManager::setLiveHubEngine(LiveHubEngine *engine)
{
    m_engine = engine;
}

void RuntimeManager::setWorkspace(const QString &workspace)
{
    m_workspace = workspace;
}

void RuntimeManager::initConnectToServer()
{
    if (m_curproc->state() == QProcess::Running) {
        m_curproc->connectToServer();
    }
}

void RuntimeManager::newRuntimeWindow(const QString &document)
{
    m_currentFile = new LiveDocument(document);

    if (m_engine == nullptr) {
        qWarning() << "Unable to start runtime: QML engine is null";
        return;
    }

    m_curproc = new RuntimeProcess(this, m_port);

    QStringList arguments = argumentsList(m_port, m_currentFile->relativeFilePath());

    LiveHubEngine *engine = new LiveHubEngine(this);
    engine->setMaximumWatches(m_engine->maximumWatches());
    engine->setWorkspace(m_workspace);
    engine->setActivePath(*m_currentFile);

    qInfo()<<"ENGINE workspace= "<<engine->workspace()<<" doc = "<<engine->activePath().relativeFilePath()<<" abspath = "<<engine->activePath().absoluteFilePathIn(engine->workspace());

    m_curproc->setLiveHubEngine(engine);

    QDockWidget *dock = new QDockWidget(document);
    dock->setObjectName(document + "LogDock");
    m_logDocks.append(dock);
    LogView *view = new LogView(false, dock);

    connect(m_curproc, &RuntimeProcess::remoteLog, view, &LogView::appendToLog);
    connect(m_curproc, &RuntimeProcess::clearLog, view, &LogView::clear);
    connect(m_curproc, &RuntimeProcess::connected, view, &LogView::clear);
    dock->setWidget(view);
    emit logWidgetAdded(dock);

    connect(m_curproc, &RuntimeProcess::started, this, &RuntimeManager::onRuntimeStarted);
    connect(m_curproc, &RuntimeProcess::connected, this, &RuntimeManager::onConnected);
    connect(m_curproc, &RuntimeProcess::errorOccurred, this, &RuntimeManager::onError);

    m_curproc->start(m_runtimeBinaryPath, arguments);
}

void RuntimeManager::onConnected()
{
    m_curproc->setCurrentFile(*m_currentFile);
    m_runtimes.append(m_curproc);
    m_port++;
}

void RuntimeManager::onRuntimeStarted()
{
    m_curproc->connectToServer(1500);
}

void RuntimeManager::finishProcesses()
{
    qInfo() << "RuntimeManager::finishProcesses ---------TERMINATE ALL CHILD PROCESSES__________";

    for (int i = 0; i < m_runtimes.count(); i++) {
        if (m_runtimes.at(i)->state() != QProcess::NotRunning) {
            m_runtimes.at(i)->terminate();
            if (!m_runtimes.at(i)->waitForFinished(500)) {
                qInfo() << "QML Live Runtime with port " + QString::number(m_runtimes.at(i)->port()) + " was not finished within 500 msec, KILLING...";
                m_runtimes.at(i)->kill();
            }
        }
    }
}

QStringList RuntimeManager::argumentsList(const int& port, const QString& title, const bool hideButtons)
{
    QStringList arguments;
    arguments << "--ipcport" << QString::number(port);
    if (!title.isEmpty()) {
        arguments << "--title" << title;
    }
    if (hideButtons) {
        arguments << "--hidebuttons";
    }
    if (m_pluginPath) {
        arguments << "--pluginpath" << *m_pluginPath;
    }
    if (m_imports) {
        for (int i = 0; i < m_imports->count(); i++) {
            arguments << "--importpath" << m_imports->at(i);
        }
    }
    arguments << m_workspace;
    qInfo() << "ARGUMENTS: " << arguments;

    return arguments;
}

void RuntimeManager::updateRuntimePath(const QString& path)
{
    m_runtimeBinaryPath = path;
}

void RuntimeManager::onError(QProcess::ProcessError error)
{
    qWarning() << "RuntimeManager::onError ProcessError = " << error;
}

void RuntimeManager::setPluginPath(const QString& path)
{
    if (!m_pluginPath) {
        m_pluginPath = new QString(path);
    } else {
        *m_pluginPath = path;
    }
}

void RuntimeManager::setImportPathList(const QStringList& paths)
{
    if (!m_imports) {
        m_imports = new QStringList(paths);
    } else {
        *m_imports = paths;
    }
}

void RuntimeManager::restartAll()
{
    for (int i = 0; i < m_runtimes.count(); i++) {
        m_runtimes.at(i)->terminate();
        if (!m_runtimes.at(i)->waitForFinished(500)) {
            m_runtimes.at(i)->kill();
        }
    }
    for (int i = 0; i < m_logDocks.count(); i++) {
        emit logWidgetRemoved(m_logDocks.at(i));
    }
}
