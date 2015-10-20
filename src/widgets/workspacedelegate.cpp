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

#include "workspacedelegate.h"

WorkspaceDelegate::WorkspaceDelegate(FileSystemModel* model, QObject *parent) :
    QStyledItemDelegate(parent),
    m_model(model)
{
}

void WorkspaceDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);

    if (option) {
        if (m_model->isDir(index))
            return;

        QString path = m_model->filePath(index);
        foreach (QString type, m_model->allowedTypesFilter())
        {
            if (path.contains(QRegExp(type, Qt::CaseInsensitive, QRegExp::Wildcard)))
                return;
        }

        option->state &= ~QStyle::State_Enabled;

        QColor disabled = option->palette.color(QPalette::Disabled, QPalette::Text);
        QColor enabled = option->palette.color(QPalette::Normal, QPalette::Text);

        if (disabled == enabled) {
            QColor t = option->palette.color(QPalette::Disabled, QPalette::Text);
            QColor b = option->palette.color(QPalette::Disabled, QPalette::Background);

            QColor selection = QColor::fromRgb((t.red() + b.red()) / 2, (t.green() + b.green()) / 2, (t.blue() + b.blue()) / 2);

            option->palette.setColor(QPalette::Disabled, QPalette::Text, selection);
        }
    }
}
