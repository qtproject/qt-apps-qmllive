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

    // Prevent view highlighting background of a selected row. Only the
    // active-document's row should be highlighted. See also
    // WorkspaceDelegate::initStyleOption()
    QPalette noHighlightPalette = palette();
    noHighlightPalette.setColor(QPalette::Highlight, palette().color(QPalette::Base));
    noHighlightPalette.setColor(QPalette::HighlightedText, palette().color(QPalette::Text));
    m_view->setPalette(noHighlightPalette);

    m_view->setItemDelegate(new WorkspaceDelegate(this));
    connect(m_view, &QTreeView::activated, this, &WorkspaceView::indexActivated);

    m_model->setAllowedTypesFilter(QStringList() << "*.qml" << "*.png" << "*.otf" << "*.ttf");

    // setup layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_view);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    m_view->setDragEnabled(true);
    m_view->setDragDropMode(QAbstractItemView::DragOnly);

    m_view->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_newWindow = new QAction(tr("Open in New Window"), this);
    connect(m_newWindow, &QAction::triggered, this, &WorkspaceView::onNewRuntimeWindow);
    m_view->addAction(m_newWindow);
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
void WorkspaceView::activateDocument(const LiveDocument &path)
{
    QModelIndex index = m_model->index(path.absoluteFilePathIn(rootPath()));
    selectIndex(index);
}

void WorkspaceView::activateRootPath()
{
    selectIndex(m_rootIndex);
    emit pathActivated(LiveDocument(QStringLiteral(".")));
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
LiveDocument WorkspaceView::activeDocument() const
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

    LiveDocument oldDocument = m_currentDocument;

    m_currentDocument = LiveDocument::resolve(m_model->rootDirectory(), path);
    emit pathActivated(m_currentDocument);
    m_view->update(index);

    if (!oldDocument.isNull())
        m_view->update(m_model->index(oldDocument.absoluteFilePathIn(rootPath())));
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

void WorkspaceView::onNewRuntimeWindow()
{
    QString path = m_model->rootDirectory().relativeFilePath(m_model->filePath(m_view->currentIndex()));
    qInfo() << "Opening new QML Live Runtime with file path: " << path;
    emit newRuntimeWindow(path);
}
void WorkspaceView::onConnectToServer()
{
    emit initConnectToServer(m_view->model()->data(m_view->currentIndex()).toString());
}

void WorkspaceView::restoreFromSettings(QSettings *s)
{
    hideNonQMLFiles(s->value("only_qml_files/enabled").toBool());
}

void WorkspaceView::hideNonQMLFiles(bool hide)
{
    QStringList filters;
    if (hide) {
        filters << "*.qml";
    }
    else {
        filters << "*.*";
    }
    m_model->setNameFilters(filters);
    m_model->setNameFilterDisables(false);
    m_view->setModel(m_model);
}
