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

#include "autodiscoveryhostsdialog.h"
#include "ui_autodiscoveryhostsdialog.h"
#include "hostmodel.h"

AutoDiscoveryHostsDialog::AutoDiscoveryHostsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoDiscoveryHostsDialog),
    m_model(0)
{
    ui->setupUi(this);
}

AutoDiscoveryHostsDialog::~AutoDiscoveryHostsDialog()
{
    delete ui;
}

QList<QPointer<Host> > AutoDiscoveryHostsDialog::selectedHosts() const
{
    QList<QPointer<Host> > hostList;

    QModelIndexList selectedRows = ui->hostView->selectionModel()->selectedRows();
    foreach (QModelIndex index, selectedRows) {
        Host* host = m_model->hostAt(index.row());
        hostList.append(QPointer<Host>(host));
    }

    return hostList;
}

void AutoDiscoveryHostsDialog::clearSelection()
{
    ui->hostView->clearSelection();
}

void AutoDiscoveryHostsDialog::setDiscoveredHostsModel(HostModel *model)
{
    m_model = model;

    ui->hostView->setModel(model);
    ui->hostView->hideColumn(0);
}
