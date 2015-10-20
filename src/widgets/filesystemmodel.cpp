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

#include "filesystemmodel.h"

FileSystemModel::FileSystemModel(QObject *parent) :
    QFileSystemModel(parent) ,
    m_dirSelectable(true)
{
}

void FileSystemModel::setAllowedTypesFilter(QStringList allowed)
{
    m_allowedTypes = allowed;
}

QStringList FileSystemModel::allowedTypesFilter() const
{
    return m_allowedTypes;
}

void FileSystemModel::setDirectoriesSelectable(bool enabled)
{
    m_dirSelectable = enabled;
}

bool FileSystemModel::directoriesSelectable() const
{
    return m_dirSelectable;
}

Qt::ItemFlags FileSystemModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QFileSystemModel::flags(index);

    if (isDir(index)) {
        if (m_dirSelectable)
            return f;
        else
            return f & ~Qt::ItemIsSelectable;
    }

    QString path = filePath(index);

    foreach (QString type, m_allowedTypes) {
        if (path.contains(QRegExp(type, Qt::CaseInsensitive, QRegExp::Wildcard)))
            return f;
    }

    return f & ~Qt::ItemIsSelectable;
}


