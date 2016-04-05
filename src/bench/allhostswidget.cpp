/****************************************************************************
**
** Copyright (C) 2016 Pelagicore AG
** Contact: http://www.qt.io/ or http://www.pelagicore.com/
**
** This file is part of the QmlLive tool.
**
** $QT_BEGIN_LICENSE:GPL3-PELAGICORE$
** Commercial License Usage
** Licensees holding valid commercial Pelagicore QmlLive
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

#include "allhostswidget.h"

#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QMenu>
#include <QGridLayout>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QMessageBox>

AllHostsWidget::AllHostsWidget(QWidget *parent) :
    QWidget(parent)
{
    setAcceptDrops(true);

    setFixedHeight(155);

    QGridLayout* layout = new QGridLayout(this);
    m_groupBox = new QGroupBox("All Hosts");
    layout->addWidget(m_groupBox);

    m_menuButton = new QPushButton("...", m_groupBox);
    m_menuButton->setMaximumWidth(30);
    m_menuButton->setCheckable(true);
    connect(m_menuButton, SIGNAL(clicked()), this, SLOT(showMenu()));

    QVBoxLayout* hbox = new QVBoxLayout(m_groupBox);
    hbox->addWidget(new QLabel("Drop File"));
    hbox->addWidget(m_menuButton);

    m_menu = new QMenu(this);
    m_publishAction = m_menu->addAction("Publish All", this, SLOT(onPublishTriggered()));
    connect(m_menu, SIGNAL(aboutToHide()), m_menuButton, SLOT(toggle()));

    m_refreshAction = m_menu->addAction("Refresh All", this, SIGNAL(refreshAll()));
}

void AllHostsWidget::setWorkspace(const QString &workspace)
{
    m_workspace.setPath(workspace);
}

void AllHostsWidget::showMenu()
{
    QPoint p = mapToGlobal(m_menuButton->pos());
    p.ry() += m_menuButton->height() + 5;
    p.rx() += 5;
    m_menu->exec(p);
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
