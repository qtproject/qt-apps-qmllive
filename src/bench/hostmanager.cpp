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

#include "hostmanager.h"
#include "hostmodel.h"
#include "hostwidget.h"
#include "dummydelegate.h"
#include "livehubengine.h"
#include "logreceiver.h"
#include "widgets/logview.h"
#include <QDockWidget>

#include <QDebug>

HostManager::HostManager(QWidget *parent) :
    QListView(parent)
{
    setItemDelegate(new DummyDelegate(this));
    setFlow(QListView::LeftToRight);
    setUniformItemSizes(true);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setDragEnabled(true);
    setDefaultDropAction(Qt::MoveAction);
    setDragDropMode(QAbstractItemView::InternalMove);
    viewport()->setAcceptDrops(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);

    viewport()->setAutoFillBackground(false);
}

void HostManager::setModel(HostModel *model)
{
    m_model = model;
    QListView::setModel(model);

    connect(model, SIGNAL(modelReset()), this, SLOT(modelReseted()));
}

void HostManager::setLiveHubEngine(LiveHubEngine *engine)
{
    m_engine = engine;

    for (int i=0; i < m_model->rowCount(); i++) {
        HostWidget* widget = qobject_cast<HostWidget*>(indexWidget(m_model->index(i, 0)));
        if (widget)
            widget->setLiveHubEngine(engine);
    }
}

void HostManager::followTreeSelection(const QString &currentFile)
{
    for (int i=0; i < m_model->rowCount(); i++) {
        HostWidget* widget = qobject_cast<HostWidget*>(indexWidget(m_model->index(i, 0)));
        if (widget && widget->followTreeSelection())
            widget->setCurrentFile(currentFile);
    }
}

void HostManager::setCurrentFile(const QString &currentFile)
{
    for (int i=0; i < m_model->rowCount(); i++) {
        HostWidget* widget = qobject_cast<HostWidget*>(indexWidget(m_model->index(i, 0)));
        if (widget)
            widget->setCurrentFile(currentFile);
    }
}

void HostManager::publishAll()
{
    for (int i=0; i < m_model->rowCount(); i++) {
        HostWidget* widget = qobject_cast<HostWidget*>(indexWidget(m_model->index(i, 0)));
        if (widget)
            widget->publishWorkspace();
    }
}

void HostManager::refreshAll()
{
    for (int i=0; i < m_model->rowCount(); i++) {
        HostWidget* widget = qobject_cast<HostWidget*>(indexWidget(m_model->index(i, 0)));
        if (widget)
            widget->refresh();
    }
}

void HostManager::rowsInserted(const QModelIndex &parent, int start, int end)
{
    if (parent.isValid())
        return;

    for (int i=start; i<= end; i++) {
        addHost(i);
    }
}

void HostManager::addHost(int index)
{
    HostWidget* widget = new HostWidget();
    Host* host = m_model->hostAt(index);
    widget->setLiveHubEngine(m_engine.data());
    widget->setHost(host);
    setIndexWidget(m_model->index(index,0), widget);
    connect(widget, SIGNAL(openHostConfig(Host*)), this, SIGNAL(openHostConfig(Host*)));

    QDockWidget* dock = new QDockWidget(host->name());
    dock->setObjectName(host->name() + "LogDock");
    connect(host, SIGNAL(nameChanged(QString)), dock, SLOT(setWindowTitle(QString)));
    LogView* view = new LogView(false, dock);
    connect(widget, SIGNAL(remoteLog(int,QString,QUrl,int,int)), view, SLOT(appendToLog(int,QString,QUrl,int,int)));
    dock->setWidget(view);
    m_logList.append(dock);
    emit logWidgetAdded(dock);
}

void HostManager::rowsAboutToBeRemoved(const QModelIndex & parent, int start, int end)
{
    if (parent.isValid())
        return;

    for (int i=start; i<= end; i++) {
        QDockWidget* dock = m_logList.takeAt(i);
        delete dock;
    }
}

void HostManager::modelReseted()
{
    for (int i=0; i < m_model->rowCount(); i++) {
        addHost(i);
    }
}
