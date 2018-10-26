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

#include "hostwidget.h"

#include "host.h"
#include "livehubengine.h"

#include <QMessageBox>

Q_DECLARE_LOGGING_CATEGORY(csLog)
Q_LOGGING_CATEGORY(csLog, "QMLLive.Bench.ConnectionState", QtInfoMsg)

const int LABEL_STACK_INDEX=0;
const int PROGRESS_STACK_INDEX=1;

HostWidget::HostWidget(QWidget *parent) :
    QWidget(parent)
{
    setContentsMargins(0,0,0,0);
    setAcceptDrops(true);

    m_connectDisconnectAction = new QAction("Offline", this);
    m_connectDisconnectAction->setIcon(QIcon(":images/error_ball.svg"));
    connect(m_connectDisconnectAction, &QAction::triggered, this, &HostWidget::connectToServer);

    m_refreshAction = new QAction("Refresh", this);
    m_refreshAction->setIcon(QIcon(":images/refresh.svg"));
    connect(m_refreshAction, &QAction::triggered, this, &HostWidget::refresh);

    m_publishAction = new QAction("Publish", this);
    m_publishAction->setIcon(QIcon(":/images/publish.svg"));
    connect(m_publishAction, &QAction::triggered, this, &HostWidget::publishAll);

    m_followTreeSelectionAction = new QAction("Follow", this);
    m_followTreeSelectionAction->setIcon(QIcon(":images/linked.svg"));
    m_followTreeSelectionAction->setCheckable(true);

    m_editHostAction = new QAction("Setup", this);
    m_editHostAction->setIcon(QIcon(":images/edit.svg"));
    connect(m_editHostAction, &QAction::triggered, this, &HostWidget::onEditHost);

    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(0,0,0,0);
    m_groupBox = new QGroupBox("NONAME", this);
    layout->addWidget(m_groupBox);

    QVBoxLayout *vbox = new QVBoxLayout(m_groupBox);
    vbox->setContentsMargins(0,0,0,0);

    m_stackedLayout = new QStackedLayout();
    m_documentLabel = new QLabel(m_groupBox);
    m_documentLabel->setAlignment(Qt::AlignCenter);
    m_documentLabel->setContentsMargins(4,4,4,4);
    m_documentLabel->setFrameShape(QFrame::StyledPanel);
    m_stackedLayout->insertWidget(LABEL_STACK_INDEX, m_documentLabel);
    vbox->addLayout(m_stackedLayout, 1);


    m_groupBox->installEventFilter(this);

    QToolBar *toolBar = new QToolBar(m_groupBox);
    toolBar->setIconSize(QSize(16,16));
    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addAction(m_connectDisconnectAction);
    toolBar->addAction(m_refreshAction);
    toolBar->addAction(m_publishAction);
    toolBar->addAction(m_followTreeSelectionAction);
    toolBar->addAction(m_editHostAction);

    m_sendProgress = new QProgressBar(m_groupBox);
    m_sendProgress->setMaximum(1);
    m_sendProgress->setValue(1);
    m_stackedLayout->insertWidget(PROGRESS_STACK_INDEX, m_sendProgress);

    vbox->addWidget(toolBar);;

    connect(&m_publisher, &RemotePublisher::connected, this, &HostWidget::connected);
    connect(&m_publisher, &RemotePublisher::connected, this, &HostWidget::onConnected);
    connect(&m_publisher, &RemotePublisher::disconnected, this, &HostWidget::onDisconnected);
    connect(&m_publisher, &RemotePublisher::connectionError, this, &HostWidget::onConnectionError);
    connect(&m_publisher, &RemotePublisher::sendingError, this, &HostWidget::onSendingError);
    connect(&m_publisher, &RemotePublisher::sentSuccessfully, this, &HostWidget::onSentSuccessfully);
    connect(&m_publisher, &RemotePublisher::needsPinAuthentication, this, &HostWidget::showPinDialog);
    connect(&m_publisher, &RemotePublisher::pinOk, this, &HostWidget::onPinOk);
    connect(&m_publisher, &RemotePublisher::remoteLog, this, &HostWidget::remoteLog);
    connect(&m_publisher, &RemotePublisher::clearLog, this, &HostWidget::clearLog);
}

void HostWidget::setHost(Host *host)
{
    m_host = host;

    updateTitle();
    updateFile(m_host->currentFile());
    m_followTreeSelectionAction->setChecked(m_host->followTreeSelection());

    connect(host, &Host::addressChanged, this, &HostWidget::updateTitle);
    connect(host, &Host::addressChanged, this, &HostWidget::scheduleConnectToServer);
    connect(host, &Host::portChanged, this, &HostWidget::updateTitle);
    connect(host, &Host::portChanged, this, &HostWidget::scheduleConnectToServer);
    connect(host, &Host::availableChanged, this, &HostWidget::updateAvailableState);
    connect(host, &Host::currentFileChanged, this, &HostWidget::updateFile);
    connect(host, &Host::nameChanged, this, &HostWidget::updateTitle);
    connect(host, &Host::xOffsetChanged, this, &HostWidget::sendXOffset);
    connect(host, &Host::yOffsetChanged, this, &HostWidget::sendYOffset);
    connect(host, &Host::rotationChanged, this, &HostWidget::sendRotation);
    connect(host, &Host::followTreeSelectionChanged, this, &HostWidget::updateFollowTreeSelection);

    connect(m_followTreeSelectionAction, &QAction::triggered, host, &Host::setFollowTreeSelection);
    connect(&m_publisher, &RemotePublisher::activeDocumentChanged, host, &Host::setCurrentFile);

    updateAvailableState(m_host->available());
    updateRemoteActions();
}

void HostWidget::setLiveHubEngine(LiveHubEngine *engine)
{
    m_engine = engine;

    m_publisher.setWorkspace(m_engine->workspace());

    connect(m_engine.data(), &LiveHubEngine::workspaceChanged, &m_publisher, &RemotePublisher::setWorkspace);
    connect(m_engine.data(), &LiveHubEngine::workspaceChanged, this, &HostWidget::refreshDocumentLabel);
    connect(m_engine.data(), &LiveHubEngine::fileChanged, this, &HostWidget::sendDocument);
    connect(m_engine.data(), &LiveHubEngine::beginPublishWorkspace, &m_publisher, &RemotePublisher::beginBulkSend);
    connect(m_engine.data(), &LiveHubEngine::endPublishWorkspace, &m_publisher, &RemotePublisher::endBulkSend);
    connect(&m_publisher, &RemotePublisher::needsPublishWorkspace, this, &HostWidget::publishWorkspace);
}

void HostWidget::setCurrentFile(const LiveDocument &currentFile)
{
    if (m_publisher.state() != QAbstractSocket::ConnectedState)
        return;

    m_publisher.activateDocument(currentFile);
}

bool HostWidget::followTreeSelection() const
{
    return m_followTreeSelectionAction->isChecked();
}

void HostWidget::updateTitle()
{
    m_groupBox->setTitle(QString("%1 (%2:%3)")
                         .arg(m_host->name())
                         .arg(m_host->address())
                         .arg(m_host->port()));
}

void HostWidget::updateFile(const LiveDocument &file)
{
    QFont font(this->font());
    QPalette palette(this->palette());
    QString text;
    QString toolTip;

    if (file.isNull()) {
        if (m_publisher.state() != QAbstractSocket::ConnectedState) {
            text = tr("Host offline");
        } else {
            text = tr("No active document");
            toolTip = tr("No active document. Drop one.");
        }
        font.setItalic(true);
    } else {
        text = file.relativeFilePath();

        if (file.isFileIn(m_engine->workspace())) {
            toolTip = file.absoluteFilePathIn(m_engine->workspace());
        } else {
            // red color from http://clrs.cc
            palette.setColor(QPalette::Text, QColor(0xff, 0x41, 0x36));
            toolTip = file.errorString();
        }
    }

    QFontMetrics metrics(font);
    m_documentLabel->setFont(font);
    m_documentLabel->setPalette(palette);
    m_documentLabel->setText(metrics.elidedText(text, Qt::ElideLeft,
                                                m_documentLabel->width()));
    m_documentLabel->setToolTip(toolTip);

    if (m_host->followTreeSelection() && !file.isNull() && file != m_engine->activePath()) {
        if (m_publisher.state() == QAbstractSocket::ConnectedState)
            m_publisher.activateDocument(m_engine->activePath());
    }

    updateRemoteActions();
}

void HostWidget::refreshDocumentLabel()
{
    updateFile(m_host->currentFile());
}

void HostWidget::updateAvailableState(bool available)
{
    qCDebug(csLog) << "updateAvailableState()" << available;

    if (available)
        scheduleConnectToServer();
    else
        onDisconnected();
}

void HostWidget::updateFollowTreeSelection(bool follow)
{
    m_followTreeSelectionAction->setChecked(follow);

    if (follow && m_publisher.state() == QAbstractSocket::ConnectedState
            && m_host->currentFile() != m_engine->activePath()) {
        m_publisher.activateDocument(m_engine->activePath());
    }
}

void HostWidget::updateRemoteActions()
{
    m_refreshAction->setEnabled(m_publisher.state() == QAbstractSocket::ConnectedState
                                && !m_host->currentFile().isNull());
    m_publishAction->setEnabled(m_publisher.state() == QAbstractSocket::ConnectedState);
}

void HostWidget::scheduleConnectToServer()
{
    qCDebug(csLog) << "scheduleConnectToServer()" << m_host->name();

    m_connectToServerTimer.start(0, this);
}

void HostWidget::connectToServer()
{
    qCDebug(csLog) << "connectToServer()" << m_host->name()
                   << m_publisher.state() << "available:" << m_host->available();

    if (m_publisher.state() != QAbstractSocket::UnconnectedState)
        return;

    if (m_host->available()) {
        m_publisher.connectToServer(m_host->address(), m_host->port());
        m_activateId = QUuid();
        m_rotationId = QUuid();
        m_xOffsetId = QUuid();
        m_yOffsetId = QUuid();
        m_changeIds.clear();
    }
}

void HostWidget::onConnected()
{
    qCDebug(csLog) << "Host connected:" << m_host->name();

    m_connectDisconnectAction->setIcon(QIcon(":images/okay_ball.svg"));
    m_connectDisconnectAction->setToolTip("Host online");
    m_connectDisconnectAction->setText("Online");

    updateFile(m_host->currentFile());
    updateRemoteActions();

    sendXOffset(m_host->xOffset());
    sendYOffset(m_host->yOffset());
    sendRotation(m_host->rotation());

    disconnect(m_connectDisconnectAction, &QAction::triggered, 0, 0);
    connect(m_connectDisconnectAction, &QAction::triggered, &m_publisher, &RemotePublisher::disconnectFromServer);

    // Send .qrc files to let the node fill its resourceMap
    // TODO not using bulkSend as this could be misinterpreted as a response to
    // needsPublishWorkspace
    QDirIterator it(m_engine->workspace(), {"*.qrc"}, QDir::AllEntries | QDir::NoDotAndDotDot,
            QDirIterator::Subdirectories);
    while (it.hasNext()) {
        LiveDocument qrcFile = LiveDocument::resolve(m_engine->workspace(), it.next());
        sendDocument(qrcFile);
    }

    m_publisher.initComplete();
}

void HostWidget::onDisconnected()
{
    qCDebug(csLog) << "Host disconnected:" << m_host->name();

    m_connectDisconnectAction->setIcon(QIcon(":images/error_ball.svg"));
    m_connectDisconnectAction->setToolTip("Host Offline");
    m_connectDisconnectAction->setText("Offline");
    resetProgressBar();

    m_host->setCurrentFile(LiveDocument());
    updateRemoteActions();

    disconnect(m_connectDisconnectAction, &QAction::triggered, 0, 0);
    connect(m_connectDisconnectAction, &QAction::triggered, this, &HostWidget::connectToServer);
}

void HostWidget::onConnectionError(QAbstractSocket::SocketError error)
{
    qCDebug(csLog) << "Host connection error:" << m_host->name() << error;

    m_connectDisconnectAction->setToolTip(m_publisher.errorToString(error));
    m_connectDisconnectAction->setIcon(QIcon(":images/warning_ball.svg"));

    if (error == QAbstractSocket::RemoteHostClosedError)
        m_host->setOnline(false);

    if (m_publisher.state() != QAbstractSocket::ConnectedState)
        onDisconnected();
}

void HostWidget::refresh()
{
    if (m_publisher.state() != QAbstractSocket::ConnectedState)
        return;

    if (!m_host->currentFile().isNull())
        m_publisher.activateDocument(m_host->currentFile());
}

void HostWidget::probe()
{
    connectToServer();
}

void HostWidget::publishWorkspace()
{
    if (m_publisher.state() != QAbstractSocket::ConnectedState)
        return;

    connect(m_engine.data(), &LiveHubEngine::publishFile, this, &HostWidget::sendDocument);
    m_engine->publishWorkspace();
    disconnect(m_engine.data(), &LiveHubEngine::publishFile, this, &HostWidget::sendDocument);
}

void HostWidget::sendDocument(const LiveDocument& document)
{
    if (m_publisher.state() != QAbstractSocket::ConnectedState)
        return;

    m_stackedLayout->setCurrentIndex(PROGRESS_STACK_INDEX);
    m_changeIds.append(m_publisher.sendDocument(document));
    m_sendProgress->setMaximum(m_sendProgress->maximum() + 1);
}

void HostWidget::sendXOffset(int offset)
{
    m_xOffsetId = m_publisher.setXOffset(offset);
}

void HostWidget::sendYOffset(int offset)
{
    m_yOffsetId = m_publisher.setYOffset(offset);
}

void HostWidget::sendRotation(int rotation)
{
    m_rotationId = m_publisher.setRotation(rotation);
}

void HostWidget::onSendingError(const QUuid &uuid, QAbstractSocket::SocketError socketError)
{
    if (uuid == m_activateId) {
        m_connectDisconnectAction->setToolTip(QString("Activating file failed: %1").arg(m_publisher.errorToString(socketError)));
        m_connectDisconnectAction->setIcon(QIcon(":images/warning_ball.svg"));
        m_activateId = QUuid();
    } else if (uuid == m_xOffsetId) {
        m_connectDisconnectAction->setToolTip(QString("Setting the X Offset failed: %1").arg(m_publisher.errorToString(socketError)));
        m_connectDisconnectAction->setIcon(QIcon(":images/warning_ball.svg"));
        m_xOffsetId = QUuid();
    } else if (uuid == m_yOffsetId) {
        m_connectDisconnectAction->setToolTip(QString("Setting the Y Offset failed: %1").arg(m_publisher.errorToString(socketError)));
        m_connectDisconnectAction->setIcon(QIcon(":images/warning_ball.svg"));
        m_yOffsetId = QUuid();
    } else if (uuid == m_rotationId) {
        m_connectDisconnectAction->setToolTip(QString("Setting the Rotation failed: %1").arg(m_publisher.errorToString(socketError)));
        m_connectDisconnectAction->setIcon(QIcon(":images/warning_ball.svg"));
        m_rotationId = QUuid();
    } else if (m_changeIds.contains(uuid)) {
        m_connectDisconnectAction->setToolTip(QString("Not all files were synced successfully: %1").arg(m_publisher.errorToString(socketError)));
        m_connectDisconnectAction->setIcon(QIcon(":images/warning_ball.svg"));
        m_changeIds.removeAll(uuid);
        resetProgressBar();
    }
}

void HostWidget::onSentSuccessfully(const QUuid &uuid)
{
    if (uuid == m_activateId) {
        m_connectDisconnectAction->setIcon(QIcon(":images/okay_ball.svg"));
        m_activateId = QUuid();
    } else if (uuid == m_xOffsetId) {
        m_connectDisconnectAction->setIcon(QIcon(":images/okay_ball.svg"));
        m_xOffsetId = QUuid();
    } else if (uuid == m_yOffsetId) {
        m_connectDisconnectAction->setIcon(QIcon(":images/okay_ball.svg"));
        m_yOffsetId = QUuid();
    } else if (uuid == m_rotationId) {
        m_connectDisconnectAction->setIcon(QIcon(":images/okay_ball.svg"));
        m_rotationId = QUuid();
    } else if (m_changeIds.contains(uuid)) {
        m_changeIds.removeAll(uuid);
        m_sendProgress->setValue(m_sendProgress->value() + 1);
        if (m_changeIds.isEmpty()) {
            m_connectDisconnectAction->setIcon(QIcon(":images/okay_ball.svg"));
            resetProgressBar();
        }
    }
}

void HostWidget::resetProgressBar()
{
    m_sendProgress->setValue(1);
    m_sendProgress->setMaximum(1);
    m_stackedLayout->setCurrentIndex(LABEL_STACK_INDEX);
}

void HostWidget::onPinOk(bool ok)
{
    if (!ok) {
        m_publisher.disconnectFromServer();
        m_connectDisconnectAction->setIcon(QIcon(":images/warning_ball.svg"));
        m_connectDisconnectAction->setToolTip("The Host didn't accept your pin");
        QMessageBox::warning(this, "Pin not accepted", "The Host didn't accept your pin");
    }
}

void HostWidget::publishAll()
{
    if (QMessageBox::question(this, QString("Publish %1").arg(m_engine->workspace()),
                              QString("Do you really want to publish the content of %1").arg(m_engine->workspace())) == QMessageBox::Yes) {
        publishWorkspace();
    }
}

void HostWidget::onEditHost()
{
    emit openHostConfig(m_host);
}

void HostWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    refreshDocumentLabel();
}

void HostWidget::showPinDialog()
{
    bool ok = false;
    int pin = QInputDialog::getInt(this, "The Host needs a Pin Authentication", "Pin", 0, 0, 9999, 1, &ok);

    if (!ok)
        m_publisher.disconnectFromServer();

    m_publisher.checkPin(QString::number(pin));
}

void HostWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (m_publisher.state() != QAbstractSocket::ConnectedState)
        return;

    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void HostWidget::dropEvent(QDropEvent *event)
{
    if (m_publisher.state() != QAbstractSocket::ConnectedState)
        return;

    event->acceptProposedAction();

    QUrl url(event->mimeData()->urls().first());
    if (url.isLocalFile()) {
        LiveDocument document = LiveDocument::resolve(m_engine->workspace(), url.toLocalFile());
        if (!document.isNull() && document.isFileIn(m_engine->workspace())) {
            if (m_host->followTreeSelection() && document != m_engine->activePath())
                m_host->setFollowTreeSelection(false);
            m_publisher.activateDocument(document);
        } else {
            QMessageBox::warning(this, tr("Not a workspace document"),
                    tr("The dropped document is not a file in the current workspace:<br/>%1")
                        .arg(url.toString()));
        }
    }
}

bool HostWidget::eventFilter(QObject *object, QEvent *event)
{
    if ((event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonPress) && object == m_groupBox) {

        event->ignore();

        return true;
    }

    return false;
}

void HostWidget::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_connectToServerTimer.timerId()) {
        m_connectToServerTimer.stop();
        connectToServer();
    }
}
