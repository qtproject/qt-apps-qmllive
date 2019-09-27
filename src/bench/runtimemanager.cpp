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
    m_primeRuntime = new RuntimeProcess(this, Constants::PRIMERUNTIME_PORT(), true);
    QSettings s;
    QString path = s.value(Constants::RUNTIME_SETTINGS_KEY()).toString();
    if (!path.isEmpty()) {
        m_runtimeBinaryPath = path;
    } else {
        // Assuming the qmlliveruntime binary located at the same folder with qmllivebench
        m_runtimeBinaryPath = Constants::DEFAULT_RUNTIME_LOCATION();
    }
    connect(m_primeRuntime, &RuntimeProcess::errorOccurred, this, &RuntimeManager::onPrimeRuntimeError);
    connect(m_primeRuntime, &RuntimeProcess::stateChanged, this, &RuntimeManager::onPrimeRuntimeChanged);
    connect(m_primeRuntime, &RuntimeProcess::errorOccurred, this, &RuntimeManager::onError);
}
RuntimeManager::~RuntimeManager()
{
    finishProcesses();
}

void RuntimeManager::startPrimeRuntime()
{
    QDockWidget *dock =new QDockWidget("PrimeRuntime");
    dock->setObjectName("PrimeRuntimeLogDock");
    LogView *view = new LogView(false, dock);
    connect(m_primeRuntime, &RuntimeProcess::remoteLog, view, &LogView::appendToLog);
    connect(m_primeRuntime, &RuntimeProcess::clearLog, view, &LogView::clear);
    connect(m_primeRuntime, &RuntimeProcess::connected, view, &LogView::clear);
    dock->setWidget(view);
    emit logWidgetAdded(dock);

    QStringList arguments = argumentsList(Constants::PRIMERUNTIME_PORT(), "Prime QML Live Runtime", true);

    if (m_engine == nullptr) {
        qWarning() << "Failed to start Prime QML Live Runtime: nullptr QML engine object";
        return;
    }
    m_primeRuntime->setLiveHubEngine(m_engine);
    m_primeRuntime->start(m_runtimeBinaryPath, arguments);
}

void RuntimeManager::setRuntimeBinaryPath(const QString &path)
{
    m_runtimeBinaryPath = path;
}

void RuntimeManager::setLiveHubEngine(LiveHubEngine *engine)
{
    m_engine = engine;
}

void RuntimeManager::onPrimeRuntimeError(QProcess::ProcessError error)
{
    qWarning()<<"RuntimeManager::onPrimeRuntimeError: "<<error;
}

void RuntimeManager::setWorkspace(const QString &workspace)
{
    m_workspace = workspace;
    m_primeRuntime->setWorkspace(workspace);
}

void RuntimeManager::onPrimeRuntimeChanged()
{
    switch (m_primeRuntime->state()) {
    case QProcess::Running:
        qInfo() << "Prime QML Live Runtime RUNNING";
        m_primeRuntime->connectToServer();
        break;
    case QProcess::Starting:
        qInfo() << "Prime QML Live Runtime STARTING";
        break;
    case QProcess::NotRunning:
        if (QFileInfo(m_runtimeBinaryPath).exists()) {
            qWarning() << "Prime QML Live Runtime NOT RUNNING: failed to start qmlliveruntime at: "<<m_runtimeBinaryPath<<" - you may have insufficient permissions to invoke it.";
        }
        else {
            qWarning() << "Prime QML Live Runtime NOT RUNNING: qmlliveruntime is missing at: "<<m_runtimeBinaryPath;
            qWarning() << "Please specify qmlliveruntime binary location at 'Preferences->QML Live Runtime'";
        }
        break;
    default:
        break;
    }
}

void RuntimeManager::setPrimeCurrentFile(const LiveDocument &currentFile)
{
    if (!currentFile.isFileIn(m_engine->workspace())) {
        qCritical() << "The selected file for preview is not located in the workspace. File: " << currentFile.relativeFilePath();
        return;
    }

    if (m_primeRuntime->state() != QProcess::Running) {
        QStringList arguments = argumentsList(Constants::PRIMERUNTIME_PORT(), "Prime QML Live Runtime", true);

        if (m_engine == nullptr) {
            qCritical() << "Failed to start Prime QML Live Runtime: nullptr QML engine object";
            return;
        }

        m_primeRuntime->setLiveHubEngine(m_engine);
        m_primeRuntime->start(m_runtimeBinaryPath, arguments);
    }

    m_primeRuntime->setCurrentFile(currentFile);
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
    qInfo() << "RuntimeManager::finishProcesses ---------KILL ALL CHILD PROCESSES__________";

    for (int i = 0; i < m_runtimes.count(); i++) {
        m_runtimes.at(i)->terminate();
        if (!m_runtimes.at(i)->waitForFinished(500)) {
            m_runtimes.at(i)->kill();
        }
    }

    if (m_primeRuntime->state() == QProcess::Running || m_primeRuntime->state() == QProcess::Starting){
        m_primeRuntime->terminate();
        if (!m_primeRuntime->waitForFinished(100)) {
            m_primeRuntime->kill();
        }
    }
    delete m_primeRuntime;
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
    if (m_primeRuntime->state() == QProcess::NotRunning){
        restartPrimeRuntime();
    }
}

void RuntimeManager::restartPrimeRuntime()
{
    QStringList arguments = argumentsList(Constants::PRIMERUNTIME_PORT(), "Prime QML Live Runtime", true);

    if (m_engine == nullptr) {
        qWarning()<<"Failed to start Prime QML Live Runtime: nullptr QML engine object";
        return;
    }
    if (m_primeRuntime->state() == QProcess::Running) {
        m_primeRuntime->terminate();
    }
    if (!m_primeRuntime->waitForFinished(1000)) {
        m_primeRuntime->kill();
    }

    m_primeRuntime->setLiveHubEngine(m_engine);
    m_primeRuntime->start(m_runtimeBinaryPath, arguments);
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

    stopPrimeRuntime();
}

void RuntimeManager::stopPrimeRuntime()
{
    if (m_primeRuntime->state() == QProcess::Running) {
        m_primeRuntime->terminate();
    }
    if (!m_primeRuntime->waitForFinished(1000)) {
        m_primeRuntime->kill();
    }
    m_primeRuntime->clearLog();
    m_primeRuntime->remoteLog(LogView::InternalMsgType::InternalInfo, "Please select file for preview.");
}
