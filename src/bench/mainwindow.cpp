/****************************************************************************
**
** Copyright (C) 2019 Luxoft Sweden AB
** Copyright (C) 2018 Pelagicore AG
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

#include "mainwindow.h"
#include "widgets/windowwidget.h"

#include <QToolBar>
#include <QtNetwork>
#include <private/qabstractanimation_p.h>

#include "widgets/workspaceview.h"
#include "widgets/logview.h"
#include "aboutdialog.h"
#include "livehubengine.h"
#include "benchlivenodeengine.h"
#include "qmlhelper.h"
#include "optionsdialog.h"
#include "hostmodel.h"
#include "hostmanager.h"
#include "allhostswidget.h"
#include "hostdiscoverymanager.h"
#include "options.h"
#include "newprojectwizard.h"
#include "projectmanager.h"

class ErrorBar : public QFrame
{
    Q_OBJECT

public:
    ErrorBar(QWidget *parent = nullptr)
        : QFrame(parent)
    {
        setFrameShape(QFrame::StyledPanel);
        setAutoFillBackground(true);
        QPalette p = palette();
        p.setColor(QPalette::Window, Qt::red);
        setPalette(p);

        auto layout = new QHBoxLayout(this);
        layout->setContentsMargins(4, 4, 4, 4);

        m_label = new QLabel;
        m_label->setWordWrap(true);
        layout->addWidget(m_label);

        auto button = new QToolButton;
        button->setAutoRaise(true);
        button->setIcon(QIcon(":images/refresh.svg"));
        connect(button, &QAbstractButton::clicked, this, &ErrorBar::retry);
        layout->addWidget(button);
    }

    void setError(const QString &errorString)
    {
        m_label->setText(errorString);
        setVisible(!errorString.isEmpty());
    }

signals:
    void retry();

private:
    QLabel *m_label;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_initialized(false)
    , m_workspace(new WorkspaceView())
    , m_log(new LogView(true, this))
    , m_hostManager(new HostManager(this))
    , m_hostModel(new HostModel(this))
    , m_discoveryManager(new HostDiscoveryManager(this))
    , m_allHosts(new AllHostsWidget(this))
    , m_hub(new LiveHubEngine(this))
    , m_node(new BenchLiveNodeEngine(this))
    , m_newProjectWizard(new NewProjectWizard(this))
    , m_projectManager(new ProjectManager(this))
{
    setupContent();
    setupMenuBar();
    setupToolBar();

    m_discoveryManager->setKnownHostsModel(m_hostModel);\

    m_hostManager->setModel(m_hostModel);
    m_hostManager->setLiveHubEngine(m_hub);
    m_hostManager->setFrameShape(QFrame::NoFrame);

    setWindowIcon(QIcon("://images/favicon.png"));

    m_hub->setFilePublishingActive(true);
    m_node->setWorkspaceView(m_workspace);

    connect(m_workspace, &WorkspaceView::pathActivated, m_hub, &LiveHubEngine::setActivePath);
    connect(m_workspace, &WorkspaceView::pathActivated, m_hostManager, &HostManager::followTreeSelection);
    connect(m_hub, &LiveHubEngine::activateDocument, this, &MainWindow::updateWindowTitle);
    connect(m_hub, &LiveHubEngine::activateDocument, m_node, &LiveNodeEngine::loadDocument);
    connect(m_node, &LiveNodeEngine::activeWindowChanged, this, &MainWindow::onActiveWindowChanged);
    connect(m_node->qmlEngine(), &QQmlEngine::quit, this, &MainWindow::logQuitEvent);
    connect(m_allHosts, &AllHostsWidget::publishAll, m_hostManager, &HostManager::publishAll);
    connect(m_allHosts, &AllHostsWidget::currentFileChanged, m_hostManager, &HostManager::setCurrentFile);
    connect(m_allHosts, &AllHostsWidget::refreshAll, m_hostManager, &HostManager::refreshAll);
    connect(m_hostManager, &HostManager::logWidgetAdded, this, &MainWindow::onLogWidgetAdded);
    connect(m_hostManager, &HostManager::openHostConfig, this, &MainWindow::openPreferences);
    connect(m_newProjectWizard, &NewProjectWizard::accepted, this, &MainWindow::newProject);

    m_qmlDefaultimportList = m_node->qmlEngine()->importPathList();
}

MainWindow::~MainWindow()
{
}


void MainWindow::setupContent()
{
    setupLogView();
    setupWorkspaceView();
    setupHostView();

    m_ww = new WindowWidget(this);
    m_ww->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_ww->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_ww->setBackgroundRole(QPalette::Dark);
    m_node->setWindowWidget(m_ww);

    setCentralWidget(m_ww);
}

void MainWindow::onActiveWindowChanged(QQuickWindow *activeWindow)
{
    m_ww->setCenteringEnabled(true);
    m_ww->setHostedWindow(activeWindow);
}

void MainWindow::onLogWidgetAdded(QDockWidget *logDock)
{
    m_logDockMenu->addAction(logDock->toggleViewAction());

    logDock->setFeatures(m_logDock->features());
    logDock->setAllowedAreas(m_logDock->allowedAreas());
    addDockWidget(dockWidgetArea(m_logDock), logDock);
    tabifyDockWidget(m_logDock, logDock);
}

void MainWindow::setupWorkspaceView()
{
    m_workspaceDock = new QDockWidget("Workspace", this);
    m_workspaceDock->setObjectName("workspace");
    m_workspaceDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_workspaceDock->setFeatures(QDockWidget::AllDockWidgetFeatures);

    auto contents = new QWidget;
    auto layout = new QVBoxLayout(contents);
    layout->setContentsMargins(1, 1, 1, 1);
    layout->setSpacing(1);

    auto errorBar = new ErrorBar;
    layout->addWidget(errorBar);

    auto updateErrorBar = [this, errorBar]() {
        QString error;
        switch (m_hub->error()) {
        case LiveHubEngine::NoError:
            break;
        case LiveHubEngine::WatcherMaximumReached:
            error = tr("Unable to monitor file changes: The configured limit of %1 directories was exceeded.")
                .arg(LiveHubEngine::maximumWatches());
            break;
        case LiveHubEngine::WatcherSystemError:
            error = tr("Unable to monitor file changes. System limit exceeded?");
            break;
        }
        errorBar->setError(error);
    };
    updateErrorBar();
    connect(m_hub, &LiveHubEngine::errorChanged, errorBar, updateErrorBar);

    connect(errorBar, &ErrorBar::retry, this, [this]() {
        m_hub->setWorkspace(m_hub->workspace());
    });

    layout->addWidget(m_workspace);

    m_workspaceDock->setWidget(contents);
    addDockWidget(Qt::LeftDockWidgetArea, m_workspaceDock);
}

void MainWindow::setupHostView()
{
    m_hostDock = new QDockWidget("Hosts", this);
    m_hostDock->setObjectName("hosts");
    m_hostDock->setFeatures(QDockWidget::AllDockWidgetFeatures);

    QFrame *hostContainer = new QFrame();
    hostContainer->setFrameStyle(QFrame::StyledPanel);
    QHBoxLayout *layout = new QHBoxLayout(hostContainer);
    layout->setSpacing(0);
    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->setContentsMargins(0,0,0,0);
    vbox->setSpacing(0);
    vbox->addWidget(m_allHosts, 1);
    layout->addLayout(vbox);
    layout->addWidget(m_hostManager);

    m_hostDock->setWidget(hostContainer);
    addDockWidget(Qt::TopDockWidgetArea, m_hostDock);
}

void MainWindow::setupLogView()
{
    m_logDock = new QDockWidget("Log Output", this);
    m_logDock->setObjectName("log");
    m_logDock->setWidget(m_log);
    m_logDock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    m_logDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    addDockWidget(Qt::BottomDockWidgetArea, m_logDock);

    connect(m_node, &LiveNodeEngine::clearLog, m_log, &LogView::clear);
    connect(m_node, &LiveNodeEngine::logIgnoreMessages, m_log, &LogView::setIgnoreMessages);
    connect(m_node, &LiveNodeEngine::logErrors, m_log, &LogView::appendAllToLog);
}

void MainWindow::setupMenuBar()
{
    QMenu *file = menuBar()->addMenu(tr("&File"));
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
    m_createProject = file->addAction(QIcon::fromTheme("folder-new"), tr("&New Project"), this, SLOT(newProject()), QKeySequence::New);
#else
    m_createProject = file->addAction(QIcon::fromTheme("folder-new"), tr("&New Project"),
                                      this, &MainWindow::newProjectWizard, QKeySequence::New);
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
    m_openProject = file->addAction(QIcon::fromTheme("folder-open"), tr("&Open Project..."), this, SLOT(openProject()), QKeySequence::Open);
#else
    m_openProject = file->addAction(QIcon::fromTheme("folder-open"), tr("&Open Project..."),
                                      this, &MainWindow::openProject, QKeySequence::Open);
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
    m_openWorkspace = file->addAction(QIcon::fromTheme("folder-open"), tr("&Open Workspace..."), this, SLOT(openWorkspace()), QKeySequence("Ctrl+W"));
#else
    m_openWorkspace = file->addAction(QIcon::fromTheme("folder-open"), tr("&Open Workspace..."),
                                      this, &MainWindow::openWorkspace, QKeySequence("Ctrl+W"));
#endif
    m_recentMenu = file->addMenu(QIcon::fromTheme("document-open-recent"), "&Recent");
    m_recentMenu->setEnabled(false);
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
    file->addAction(tr("Preferences..."), this, SLOT(openPreferences()), QKeySequence::Preferences);
#else
    file->addAction(tr("Preferences..."), this, [this] { openPreferences(); },
                    QKeySequence::Preferences);
#endif
    file->addSeparator();
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
    file->addAction(QIcon::fromTheme("application-exit"), tr("&Quit"), this, SLOT(close()), QKeySequence::Quit);
#else
    file->addAction(QIcon::fromTheme("application-exit"), tr("&Quit"),
                    this, &MainWindow::close, QKeySequence::Quit);
#endif

    QMenu *view = menuBar()->addMenu(tr("&View"));
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
    view->addAction(tr("Take Snapshot"), this, SLOT(takeSnapshot()), QKeySequence("Ctrl+F3"));
#else
    view->addAction(tr("Take Snapshot"), this, &MainWindow::takeSnapshot, QKeySequence("Ctrl+F3"));
#endif
    view->addSeparator();

#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
    QAction *slow = view->addAction(tr("Slow Down Animations"), this, SLOT(slowDownAnimations(bool)), QKeySequence(tr("Ctrl+.")));
#else
    QAction *slow = view->addAction(tr("Slow Down Animations"),
                                    this, &MainWindow::slowDownAnimations,
                                    QKeySequence(tr("Ctrl+.")));
#endif
    slow->setCheckable(true);
    view->addSeparator();

#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
    m_refresh = view->addAction(QIcon::fromTheme("view-refresh"), tr("Refresh"), m_node, SLOT(refresh()), QKeySequence::Refresh);
#else
    m_refresh = view->addAction(QIcon::fromTheme("view-refresh"), tr("Refresh"),
                                m_node, &BenchLiveNodeEngine::refresh, QKeySequence::Refresh);
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
    m_resizeFit = view->addAction(QIcon::fromTheme("zoom-fit-best"), tr("Resize to Fit"), this, SLOT(resizeToFit()));
#else
    m_resizeFit = view->addAction(QIcon::fromTheme("zoom-fit-best"), tr("Resize to Fit"),
                                  this, &MainWindow::resizeToFit);
#endif
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
    view->addAction(tr("Show Containing Folder"), m_workspace, SLOT(goUp()), QKeySequence("Ctrl+Esc"));
#else
    view->addAction(tr("Show Containing Folder"), m_workspace, &WorkspaceView::goUp,
                    QKeySequence("Ctrl+Esc"));
#endif
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
    m_stayOnTop = view->addAction(tr("Stay on Top"), this, SLOT(stayOnTop()));
#else
    m_stayOnTop = view->addAction(tr("Stay on Top"), this, &MainWindow::stayOnTop);
#endif
    m_stayOnTop->setCheckable(true);

    view->addSeparator();
    view->addAction(m_workspaceDock->toggleViewAction());
    view->addAction(m_hostDock->toggleViewAction());
    m_logDockMenu = view->addMenu("Logs");
    m_logDockMenu->addAction(m_logDock->toggleViewAction());

    QMenu *help = menuBar()->addMenu(tr("&Help"));
    QAction *about = help->addAction(tr("About Qt QmlLive..."));
    connect(about, &QAction::triggered, this, [this]() { AboutDialog::exec(this); });
    about->setMenuRole(QAction::AboutRole);
}

void MainWindow::init()
{
    QSettings s;
    restoreGeometry(s.value("geometry").toByteArray());
    //Only set the workspace if we didn't already set it by command line
    if (m_workspacePath.isEmpty()) {
        setWorkspace(s.value("workspace", QDir::currentPath()).toString(), false);
    }

    if (s.value("http_proxy/enabled").toBool()) {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(s.value("http_proxy/hostname").toString());
        proxy.setPort(s.value("http_proxy/port").toInt());
        proxy.setUser(s.value("http_proxy/username").toString());
        proxy.setPassword(s.value("http_proxy/password").toString());
        QNetworkProxy::setApplicationProxy(proxy);
    } else {
        QNetworkProxy::setApplicationProxy(QNetworkProxy());
    }

    int size = s.beginReadArray("recentFolder");
    for (int i = 0; i < size; i++) {
        s.setArrayIndex(i);
        m_recentFolder.append(s.value("folder").toString());
    }
    s.endArray();

    updateRecentFolder();

    //Only set the document if we didn't already set it by command line
    if (m_node->activeDocument().isNull()) {
        LiveDocument last;
        if (s.contains("activeDocument"))
            last = LiveDocument::resolve(m_workspacePath, s.value("activeDocument").toString());
        if (!last.isNull())
            activateDocument(last);
        else
            m_workspace->activateRootPath();
    }

    resetImportPaths();

    m_hostModel->restoreFromSettings(&s);
    restoreState(s.value("windowState").toByteArray());

    m_initialized = true;
}

void MainWindow::writeSettings()
{
    QSettings s;
    s.setValue("geometry", saveGeometry());
    s.setValue("windowState", saveState());
    s.setValue("workspace", m_workspacePath);
    if (!m_node->activeDocument().isNull())
        s.setValue("activeDocument", m_node->activeDocument().relativeFilePath());

    s.beginWriteArray("recentFolder");
    for (int i = 0; i < m_recentFolder.count(); i++) {
        s.setArrayIndex(i);
        s.setValue("folder", m_recentFolder.at(i));
    }
    s.endArray();

    m_hostModel->saveToSettings(&s);
}

void MainWindow::resetImportPaths()
{
    QStringList importPaths;
    QSettings s;
    int count = s.beginReadArray("imports");
    for (int i=0; i<count; i++) {
        s.setArrayIndex(i);
        importPaths.append(s.value("path").toString());
    }
    s.endArray();

    setImportPaths(importPaths);
}

void MainWindow::setupToolBar()
{
    m_toolBar = addToolBar("ToolBar");
    m_toolBar->setObjectName("toolbar");
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_toolBar->addAction(m_createProject);
    m_toolBar->addAction(m_openProject);
    m_toolBar->addAction(m_openWorkspace);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_refresh);
    m_toolBar->addAction(m_resizeFit);
}

void MainWindow::activateDocument(const LiveDocument &path)
{
    m_workspace->activateDocument(path);
}

void MainWindow::resizeToFit()
{
    QSize diff = m_ww->sizeHint() - m_ww->rect().size();
    resize(rect().size() + diff);
    updateWindowTitle();
}

void MainWindow::takeSnapshot()
{
    QImage img;
    if (m_node->activeWindow())
        img = m_node->activeWindow()->grabWindow();

    if (img.isNull()) {
        m_log->appendToLog(LogView::InternalError, tr("QmlLive: could not retrieve snapshot pixmap"));
    } else {
        static int counter = 1;
        QString fileName = QString::fromLatin1("snapshot%1.png").arg(counter++);
        bool ok = img.save(fileName);

        if (!ok) // log
            m_log->appendToLog(LogView::InternalError, tr("QmlLive: could not save snapshot as \"%1\"").arg(fileName));
        else
            m_log->appendToLog(LogView::InternalInfo, tr("QmlLive: created snapshot \"%1\"").arg(fileName));
    }
}

void MainWindow::slowDownAnimations(bool enable)
{
    QUnifiedTimer::instance()->setSlowModeEnabled(enable);
}


void MainWindow::setWorkspace(const QString& path, bool activateRootPath)
{
    m_workspacePath = path;
    m_workspace->setRootPath(path);
    m_node->setWorkspace(path);
    m_hub->setWorkspace(path);
    m_allHosts->setWorkspace(path);
    if (activateRootPath)
        m_workspace->activateRootPath();
    updateRecentFolder(path);
}

void MainWindow::setPluginPath(const QString &path)
{
    m_node->setPluginPath(path);
}

void MainWindow::setImportPaths(const QStringList &pathList)
{
    m_node->qmlEngine()->setImportPathList(pathList + m_qmlDefaultimportList);
}

void MainWindow::setStaysOnTop(bool enabled)
{
    m_stayOnTop->setChecked(enabled);
    stayOnTop();
}

void MainWindow::setProject(const QString &projectFile)
{
    openProjectFile(projectFile);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    raise();
}

void MainWindow::openWorkspace()
{
    QString path = QFileDialog::getExistingDirectory(this, "Open Workspace");
    if (path.isEmpty()) {
        return;
    }
    setWorkspace(path);
}

void MainWindow::logQuitEvent()
{
    m_log->appendToLog(LogView::InternalInfo, tr("Qml Viewer tries to quit."));
}

void MainWindow::updateWindowTitle()
{
    setWindowFilePath(QString());
    if (m_hub->activePath().isNull()) {
        setWindowTitle(QApplication::applicationName());
    } else {
        setWindowTitle(QString());
        setWindowFilePath(m_hub->activePath().absoluteFilePathIn(m_workspacePath));
    }
}

void MainWindow::openPreferences(Host *host)
{
    OptionsDialog dialog;
    dialog.setHostModel(m_hostModel);
    dialog.setDiscoveredHostsModel(m_discoveryManager->discoveredHostsModel());

    if (host)
        dialog.openHostConfig(host);
    if (dialog.exec()) {
        resetImportPaths();
        m_discoveryManager->rescan();
    }
}

void MainWindow::clearRecentFolder()
{
    m_recentFolder.clear();
    m_recentMenu->setEnabled(false);
    updateRecentFolder();
}

void MainWindow::openRecentFolder()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        setWorkspace(action->text());
    }
}

void MainWindow::updateRecentFolder(const QString& path)
{
    if (!path.isEmpty())
        m_recentFolder.prepend(path);
    m_recentFolder.removeDuplicates();

    if (m_recentFolder.count())
        m_recentMenu->setEnabled(true);

    while (m_recentFolder.count() > 7)
        m_recentFolder.removeAt(m_recentFolder.count() - 1);

    m_recentMenu->clear();
    foreach (const QString file, m_recentFolder) {
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
        m_recentMenu->addAction(file, this, SLOT(openRecentFolder()));
#else
        m_recentMenu->addAction(file, this, &MainWindow::openRecentFolder);
#endif
    }

    m_recentMenu->addSeparator();
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
    m_recentMenu->addAction("Clear Menu", this, SLOT(clearRecentFolder()));
#else
    m_recentMenu->addAction("Clear Menu", this, &MainWindow::clearRecentFolder);
#endif
}

void MainWindow::stayOnTop()
{
    Qt::WindowFlags flags = windowFlags();
    if (m_stayOnTop->isChecked()) {
        setWindowFlags(flags | Qt::WindowStaysOnTopHint);
    } else {
        setWindowFlags(flags ^ (Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
    }
    show();
}

void MainWindow::openProject()
{
    QString filter = tr("QmlLive (*.qmllive);; All files (*.*)");
    QString path = QFileDialog::getOpenFileName(this, "Open Project", filter, filter);
    if (path.isEmpty()) {
        return;
    }
    openProjectFile(path);
}

void MainWindow::openProjectFile(const QString &path)
{
    if (m_projectManager->read(path))
    {
        QStringList paths;
        QSettings s;
        int count = s.beginReadArray("imports");
        for (int i=0; i<count; i++) {
            s.setArrayIndex(i);
            paths.append(s.value("path").toString());
        }
        s.endArray();
        paths.append(m_projectManager->imports());
        paths.removeDuplicates();

        //write Application settings
        s.beginWriteArray("imports");
        for (int i = 0; i < paths.count(); i++) {
            s.setArrayIndex(i);
            s.setValue("path", paths.at(i));
        }
        s.endArray();

        setImportPaths(paths);
        QString path = QDir(m_projectManager->projectLocation()).absoluteFilePath(m_projectManager->workspace());
        setWorkspace(path);
        activateDocument(LiveDocument(m_projectManager->mainDocument()));
    }
    else {
        qWarning() << "Unable to read project document: "<<path;
    }
}

void MainWindow::newProjectWizard()
{
    if (!m_newProjectWizard) {
        m_newProjectWizard = new NewProjectWizard(this);
    } else {
        m_newProjectWizard->restart();
    }
    m_newProjectWizard->show();
}

void MainWindow::newProject()
{
    m_projectManager->setImports(m_newProjectWizard->imports());
    m_projectManager->setMainDocument(m_newProjectWizard->mainDocument());
    m_projectManager->setWorkspace(m_newProjectWizard->workspace());
    m_projectManager->create(m_newProjectWizard->projectName());

    setImportPaths(m_newProjectWizard->imports());
    QString path = QDir(m_projectManager->projectLocation()).absoluteFilePath(m_newProjectWizard->workspace());
    setWorkspace(path);
    activateDocument(LiveDocument(m_newProjectWizard->mainDocument()));
}

#include "mainwindow.moc"
