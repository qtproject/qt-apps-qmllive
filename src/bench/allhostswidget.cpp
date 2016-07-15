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

#include "allhostswidget.h"


AllHostsWidget::AllHostsWidget(QWidget *parent) :
    QWidget(parent)
{
    setContentsMargins(0,0,0,0);
    m_publishAction = new QAction("Publish", this);
    m_publishAction->setIcon(QIcon(":images/publish.svg"));
    connect(m_publishAction, SIGNAL(triggered(bool)), this, SLOT(onPublishTriggered()));

    m_refreshAction = new QAction("Refresh", this);
    m_refreshAction->setIcon(QIcon(":images/refresh.svg"));
    connect(m_refreshAction, SIGNAL(triggered(bool)), this, SIGNAL(refreshAll()));


    setAcceptDrops(true);
    QHBoxLayout *contentLayout = new QHBoxLayout(this);
    contentLayout->setContentsMargins(0,0,0,0);
    QGroupBox *groupBox = new QGroupBox("All Hosts");
    contentLayout->addWidget(groupBox);

    QVBoxLayout *verticalLayout = new QVBoxLayout(groupBox);
    verticalLayout->setContentsMargins(0,0,0,0);

    QLabel *dropFileLabel = new QLabel("Drop\nDocument", groupBox);
    dropFileLabel->setAlignment(Qt::AlignCenter);
    dropFileLabel->setContentsMargins(4,4,4,4);
    dropFileLabel->setFrameStyle(QFrame::StyledPanel);
    verticalLayout->addWidget(dropFileLabel, 1);

    QToolBar *toolBar = new QToolBar(groupBox);
    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->setIconSize(QSize(16,16));

    toolBar->addAction(m_publishAction);
    toolBar->addAction(m_refreshAction);
    verticalLayout->addWidget(toolBar, 1);

}

void AllHostsWidget::setWorkspace(const QString &workspace)
{
    m_workspace.setPath(workspace);
}

void AllHostsWidget::onPublishTriggered()
{
    if (QMessageBox::question(this, QString("Publish %1").arg(m_workspace.path()),
                              QString("Do you really want to publish the content of %1").arg(m_workspace.path())) == QMessageBox::Yes) {
        emit publishAll();
    }
}

void AllHostsWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void AllHostsWidget::dropEvent(QDropEvent *event)
{
    QUrl url(event->mimeData()->text());

    if (url.isLocalFile())
        emit currentFileChanged(url.toLocalFile());
    event->acceptProposedAction();
}
