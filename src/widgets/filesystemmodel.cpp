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


