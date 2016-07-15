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

#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include "httpproxyoptionpage.h"
#include "importpathoptionpage.h"
#include "hostsoptionpage.h"

OptionsDialog::OptionsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OptionsDialog)
    , m_httpProxyForm(new HttpProxyOptionPage(this))
    , m_importPathsForm(new ImportPathOptionPage(this))
    , m_hostsForm(new HostsOptionsPage(this))
{
    ui->setupUi(this);

    QListWidgetItem *item = new QListWidgetItem("HTTP Proxy");
    int index = ui->optionsStack->addWidget(m_httpProxyForm);
    item->setSelected(true);
    item->setData(Qt::UserRole, index);
    ui->optionsView->addItem(item);

    item = new QListWidgetItem("Import Paths");
    index = ui->optionsStack->addWidget(m_importPathsForm);
    item->setData(Qt::UserRole, index);
    ui->optionsView->addItem(item);

    item = new QListWidgetItem("Hosts");
    index = ui->optionsStack->addWidget(m_hostsForm);
    item->setData(Qt::UserRole, index);
    ui->optionsView->addItem(item);

    connect(ui->optionsView, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(optionSelected(QListWidgetItem*)));
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::setHostModel(HostModel *model)
{
    m_hostsForm->setHostModel(model);
}

void OptionsDialog::setDiscoveredHostsModel(HostModel *model)
{
    m_hostsForm->setDiscoveredHostsModel(model);
}

void OptionsDialog::openHostConfig(Host *host)
{
    ui->optionsView->setCurrentRow(2);
    m_hostsForm->setHostSelected(host);
}

void OptionsDialog::optionSelected(QListWidgetItem *current)
{
    int index = current->data(Qt::UserRole).toInt();
    ui->optionsStack->setCurrentIndex(index);
}

void OptionsDialog::accept()
{
    m_httpProxyForm->apply();
    m_importPathsForm->apply();
    m_hostsForm->apply();
    QDialog::accept();
}

void OptionsDialog::reject()
{
    QDialog::reject();
}


