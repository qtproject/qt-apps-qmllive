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

#include "hostwidget.h"

#include "host.h"
#include "livehubengine.h"


const int LABEL_STACK_INDEX=0;
const int PROGRESS_STACK_INDEX=1;

HostWidget::HostWidget(QWidget *parent) :
    QWidget(parent)
{
    setContentsMargins(0,0,0,0);
    setAcceptDrops(true);

    m_connectDisconnectAction = new QAction("Offline", this);
    m_connectDisconnectAction->setIcon(QIcon(":images/error_ball.svg"));
    connect(m_connectDisconnectAction, SIGNAL(triggered(bool)), this, SLOT(connectAndSendFile()));

    m_refreshAction = new QAction("Refresh", this);
    m_refreshAction->setIcon(QIcon(":images/refresh.svg"));
    connect(m_refreshAction, SIGNAL(triggered(bool)), this, SLOT(refresh()));

    m_publishAction = new QAction("Publish", this);
    m_publishAction->setIcon(QIcon(":/images/publish.svg"));
    connect(m_publishAction, SIGNAL(triggered(bool)), this, SLOT(publishAll()));

    m_followTreeSelectionAction = new QAction("Follow", this);
    m_followTreeSelectionAction->setIcon(QIcon(":images/linked.svg"));
    m_followTreeSelectionAction->setCheckable(true);

    m_editHostAction = new QAction("Setup", this);
    m_editHostAction->setIcon(QIcon(":images/edit.svg"));
    connect(m_editHostAction, SIGNAL(triggered(bool)), this, SLOT(onEditHost()));

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

    connect(&m_publisher, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(&m_publisher, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(&m_publisher, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(&m_publisher, SIGNAL(connectionError(QAbstractSocket::SocketError)),
            this, SLOT(onConnectionError(QAbstractSocket::SocketError)));
    connect(&m_publisher, SIGNAL(sendingError(QUuid,QAbstractSocket::SocketError)),
            this, SLOT(onSendingError(QUuid,QAbstractSocket::SocketError)));
    connect(&m_publisher, SIGNAL(sentSuccessfully(QUuid)),
            this, SLOT(onSentSuccessfully(QUuid)));
    connect(&m_publisher, SIGNAL(needsPinAuthentication()), this, SLOT(showPinDialog()));
    connect(&m_publisher, SIGNAL(pinOk(bool)), this, SLOT(onPinOk(bool)));
    connect(&m_publisher, SIGNAL(remoteLog(int,QString,QUrl,int,int)),
            this, SIGNAL(remoteLog(int,QString,QUrl,int,int)));
    connect(&m_publisher, SIGNAL(clearLog()), this, SIGNAL(clearLog()));

    onDisconnected();
}

void HostWidget::setHost(Host *host)
{
    m_host = host;

    updateName(m_host->name());
    updateIp(m_host->address());
    updateFile(m_host->currentFile());
    updateOnlineState(m_host->online());
    m_followTreeSelectionAction->setChecked(m_host->followTreeSelection());

    connect(host, SIGNAL(addressChanged(QString)), this, SLOT(updateIp(QString)));
    connect(host, SIGNAL(portChanged(int)), this, SLOT(updatePort(int)));
    connect(host, SIGNAL(onlineChanged(bool)), this, SLOT(updateOnlineState(bool)));
    connect(host, SIGNAL(currentFileChanged(QString)), this, SLOT(updateFile(QString)));
    connect(host, SIGNAL(nameChanged(QString)), this, SLOT(updateName(QString)));
    connect(host, SIGNAL(xOffsetChanged(int)), this, SLOT(sendXOffset(int)));
    connect(host, SIGNAL(yOffsetChanged(int)), this, SLOT(sendYOffset(int)));
    connect(host, SIGNAL(rotationChanged(int)), this, SLOT(sendRotation(int)));
    connect(host, SIGNAL(followTreeSelectionChanged(bool)),
            m_followTreeSelectionAction, SLOT(setChecked(bool)));

    connect(m_followTreeSelectionAction, SIGNAL(triggered(bool)), host, SLOT(setFollowTreeSelection(bool)));
}

void HostWidget::setLiveHubEngine(LiveHubEngine *engine)
{
    m_engine = engine;

    m_publisher.setWorkspace(m_engine->workspace());

    connect(m_engine.data(), SIGNAL(workspaceChanged(QString)), &m_publisher, SLOT(setWorkspace(QString)));
    connect(m_engine.data(), SIGNAL(fileChanged(QString)), this, SLOT(sendDocument(QString)));
    connect(m_engine.data(), SIGNAL(beginPublishWorkspace()), &m_publisher, SLOT(beginBulkSend()));
    connect(m_engine.data(), SIGNAL(endPublishWorkspace()), &m_publisher, SLOT(endBulkSend()));
    connect(&m_publisher, SIGNAL(needsPublishWorkspace()), this, SLOT(publishWorkspace()));
}

void HostWidget::setCurrentFile(const QString currentFile)
{
    m_host->setCurrentFile(currentFile);
}

bool HostWidget::followTreeSelection() const
{
    return m_followTreeSelectionAction->isChecked();
}

void HostWidget::updateName(const QString &name)
{
    Q_UNUSED(name)
    if(m_host) {
        m_groupBox->setTitle(QString("%1 (%2:%3)").arg(m_host->name(), m_host->address(), QString::number(m_host->port())));
    }
}

void HostWidget::updateIp(const QString &ip)
{
    Q_UNUSED(ip)
    if(m_host) {
        m_groupBox->setTitle(QString("%1 (%2:%3)").arg(m_host->name(), m_host->address(), QString::number(m_host->port())));
    }

    QTimer::singleShot(0, this, SLOT(connectToServer()));
}

void HostWidget::updatePort(int port)
{
    Q_UNUSED(port)
    if(m_host) {
        m_groupBox->setTitle(QString("%1 (%2:%3)").arg(m_host->name(), m_host->address(), QString::number(m_host->port())));
    }

    QTimer::singleShot(0, this, SLOT(connectToServer()));
}

void HostWidget::updateFile(const QString &file)
{
    QString relFile = QDir(m_engine->workspace()).relativeFilePath(file);
    setUpdateFile(relFile);
    m_documentLabel->setToolTip(relFile);

    connectAndSendFile();
}

void HostWidget::setUpdateFile(const QString &file)
{
    QFontMetrics metrics(font());
    m_documentLabel->setText(metrics.elidedText(file, Qt::ElideLeft, m_documentLabel->width()));
}

void HostWidget::updateOnlineState(bool online)
{
    qDebug() << "updateOnline";

    bool available = online || m_host->type() == Host::Manual;

    if (available)
        QTimer::singleShot(0, this, SLOT(connectToServer()));
    else
        onDisconnected();
}

void HostWidget::connectToServer()
{
    qDebug() << "connectToServer";

    if (m_publisher.state() == QAbstractSocket::ConnectedState || m_publisher.state() == QAbstractSocket::ConnectingState) {
        return;
    }

    if (m_host->online() || m_host->type() == Host::Manual) {
        m_publisher.connectToServer(m_host->address(), m_host->port());
        m_activateId = QUuid();
        m_rotationId = QUuid();
        m_xOffsetId = QUuid();
        m_yOffsetId = QUuid();
        m_changeIds.clear();
    }
}

void HostWidget::connectAndSendFile()
{
    connectToServer();
    m_activateId = m_publisher.activateDocument(QDir(m_engine->workspace()).relativeFilePath(m_host->currentFile()));
}

void HostWidget::onConnected()
{
    m_connectDisconnectAction->setIcon(QIcon(":images/okay_ball.svg"));
    m_connectDisconnectAction->setToolTip("Host online");
    m_connectDisconnectAction->setText("Online");

    sendXOffset(m_host->xOffset());
    sendYOffset(m_host->yOffset());
    sendRotation(m_host->rotation());

    disconnect(m_connectDisconnectAction, SIGNAL(triggered()), 0, 0);
    connect(m_connectDisconnectAction, SIGNAL(triggered()), &m_publisher, SLOT(disconnectFromServer()));
}

void HostWidget::onDisconnected()
{
    m_connectDisconnectAction->setIcon(QIcon(":images/error_ball.svg"));
    m_connectDisconnectAction->setToolTip("Host Offline");
    m_connectDisconnectAction->setText("Offline");
    resetProgressBar();

    disconnect(m_connectDisconnectAction, SIGNAL(triggered()), 0, 0);
    connect(m_connectDisconnectAction, SIGNAL(triggered()), this, SLOT(connectToServer()));
}

void HostWidget::onConnectionError(QAbstractSocket::SocketError error)
{
    m_connectDisconnectAction->setToolTip(m_publisher.errorToString(error));
    m_connectDisconnectAction->setIcon(QIcon(":images/warning_ball.svg"));

    if (error == QAbstractSocket::RemoteHostClosedError)
        m_host->setOnline(false);

    if (m_publisher.state() != QAbstractSocket::ConnectedState)
        onDisconnected();
}

void HostWidget::refresh()
{
    connectAndSendFile();
}

void HostWidget::probe()
{
    connectToServer();
}

void HostWidget::publishWorkspace()
{
    connectToServer();
    connect(m_engine.data(), SIGNAL(publishFile(QString)), this, SLOT(sendDocument(QString)));
    m_engine->publishWorkspace();
    disconnect(m_engine.data(), SIGNAL(publishFile(QString)), this, SLOT(sendDocument(QString)));
}

void HostWidget::sendDocument(const QString& document)
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

    setUpdateFile(m_documentLabel->toolTip());
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
    if (event->mimeData()->hasFormat("text/uri-list") && (m_host->online() || m_host->type() == Host::Manual)) {

        event->acceptProposedAction();
    }
}

void HostWidget::dropEvent(QDropEvent *event)
{
    QUrl url(event->mimeData()->text());

    if (url.isLocalFile())
        m_host->setCurrentFile(url.toLocalFile());
    event->acceptProposedAction();
}

bool HostWidget::eventFilter(QObject *object, QEvent *event)
{
    if ((event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonPress) && object == m_groupBox) {

        event->ignore();

        return true;
    }

    return false;
}
