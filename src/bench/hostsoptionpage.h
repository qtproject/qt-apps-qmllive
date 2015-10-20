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

#include <QWidget>
#include <QModelIndex>

namespace Ui {
class HostsOptionPage;
}

class Host;
class HostModel;
class QMenu;
class AutoDiscoveryHostsDialog;

class HostsOptionsPage : public QWidget
{
    Q_OBJECT

public:
    explicit HostsOptionsPage(QWidget *parent = 0);
    ~HostsOptionsPage();

    void setHostModel(HostModel* model);
    void setDiscoveredHostsModel(HostModel* model);

    void apply();

    void setHostSelected(Host* host);

private slots:
    void onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);

    void updateName(const QString& name);
    void updateAddress(const QString& address);
    void updatePort(int port);
    void updateFollowTreeSelection(bool enabled);
    void updateXOffset(int offset);
    void updateYOffset(int offset);
    void updateRotation(int rotation);

    void addHost(Host* host = 0);
    void removeHost();

    void showAutoDiscoveryDialog();

private:
    Ui::HostsOptionPage *ui;

    int m_currentIndex;
    HostModel* m_model;
    AutoDiscoveryHostsDialog* m_autoDiscoveryDialog;
};

