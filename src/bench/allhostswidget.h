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

#pragma once

#include <QWidget>
#include <QDir>

class QGroupBox;
class QPushButton;
class QMenu;
class AllHostsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AllHostsWidget(QWidget *parent = 0);

    void setWorkspace(const QString& workspace);

signals:
    void refreshAll();
    void publishAll();
    void currentFileChanged(const QString file);

protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
private slots:
    void showMenu();
    void onPublishTriggered();
private:
    QDir m_workspace;
    QGroupBox* m_groupBox;
    QPushButton* m_menuButton;
    QMenu* m_menu;
    QAction* m_publishAction;
    QAction* m_refreshAction;
};

