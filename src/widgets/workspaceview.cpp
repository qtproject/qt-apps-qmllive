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

#include "workspaceview.h"
#include "filesystemmodel.h"
#include "workspacedelegate.h"

/*!
  \class WorkspaceView
  \internal
  \brief A TreeView showing the Local Filesystem.

  The activateDocument() signal can be used to connect to a LiveHubEngine.
 */


/*!
 Standard constructor using \a parent as parent
 */
WorkspaceView::WorkspaceView(QWidget *parent)
    : QWidget(parent)
    , m_view(new QTreeView(this))
    , m_model(new FileSystemModel(this))
{
    // setup view
//    m_view->setFocusPolicy(Qt::NoFocus);
    m_view->setModel(m_model);
    m_view->hideColumn(1); // size
    m_view->hideColumn(2); // type
    m_view->hideColumn(3); // modified time

    m_view->setItemDelegate(new WorkspaceDelegate(m_model, this));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(indexActivated(QModelIndex)));

    m_model->setAllowedTypesFilter(QStringList() << "*.qml" << "*.png" << "*.otf" << "*.ttf");

    // setup layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_view);
    layout->setMargin(1);
    setLayout(layout);


    m_view->setDragEnabled(true);
    m_view->setDragDropMode(QAbstractItemView::DragOnly);
}

/*!
 * Sets the root workspace path to be displayed in the view to \a dirPath
 */
void WorkspaceView::setRootPath(const QString &dirPath)
{
    m_rootIndex = m_model->setRootPath(dirPath);
    m_view->setRootIndex(m_model->index(dirPath));
}

/*!
 * Activates the document by the given \a path
 */
void WorkspaceView::activateDocument(const QString &path)
{
    //qDebug() << "WorkspaceView::activateDocument" << path;
    QModelIndex index = m_model->index(path);
    selectIndex(index);
}

void WorkspaceView::activateRootPath()
{
    selectIndex(m_rootIndex);
    emit pathActivated(m_model->rootPath());
}

void WorkspaceView::goUp()
{
    QModelIndex index = m_view->currentIndex().parent();
    if (!index.isValid() || index == m_rootIndex)
        return;

    selectIndex(index);
}

/*!
 * Returns the active, selected document.
 */
QString WorkspaceView::activeDocument() const
{
    return m_currentDocument;
}

QString WorkspaceView::rootPath() const
{
    return m_model->rootPath();
}

void WorkspaceView::setDirectoriesSelectable(bool enabled)
{
    m_model->setDirectoriesSelectable(enabled);
}

bool WorkspaceView::directoriesSelectable() const
{
    return m_model->directoriesSelectable();
}

/*!
 * Emits a document activated signal based on a given \a index
 */
void WorkspaceView::indexActivated(const QModelIndex &index)
{
    if (!(m_model->flags(index) & Qt::ItemIsSelectable))
        return;

    QString path = m_model->filePath(index);

    m_currentDocument = path;
    emit pathActivated(path);
}

void WorkspaceView::selectIndex(const QModelIndex &index)
{
    QModelIndex parentIndex = index.parent();
    while (parentIndex.isValid()) {
        m_view->expand(parentIndex);
        parentIndex = parentIndex.parent();
    }
    m_view->setCurrentIndex(index);
    indexActivated(index);
    //m_view->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
}
