/****************************************************************************
**
** Copyright (C) 2015 Pelagicore AG
** Contact: http://www.qt.io/ or http://www.pelagicore.com/
**
** This file is part of the QmlLive tool.
**
** $QT_BEGIN_LICENSE:GPL3-PELAGICORE$
** Commercial License Usage
** Licensees holding valid commercial Pelagicore Application Manager
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

#pragma once

#include <QtGui>
#include <QtWidgets>

class FileSystemModel;

class WorkspaceView : public QWidget
{
    Q_OBJECT
public:
    explicit WorkspaceView(QWidget *parent = 0);
    QString activeDocument() const;
    QString rootPath() const;
    void setDirectoriesSelectable(bool enabled);
    bool directoriesSelectable() const;

public Q_SLOTS:
    void setRootPath(const QString& dirPath);
    void activateDocument(const QString& path);
    void activateRootPath();
    void goUp();

Q_SIGNALS:
    void pathActivated(const QString& path);

private Q_SLOTS:
    void indexActivated(const QModelIndex& index);

private:
    void selectIndex(const QModelIndex& index);
    QTreeView *m_view;
    FileSystemModel *m_model;
    QModelIndex m_rootIndex;
    QString m_currentDocument;
};
