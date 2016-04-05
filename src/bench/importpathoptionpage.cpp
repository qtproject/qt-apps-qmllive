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

#include "importpathoptionpage.h"
#include "ui_importpathoptionpage.h"

ImportPathOptionPage::ImportPathOptionPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImportPathOptionPage)
{
    ui->setupUi(this);
    QSettings s;
    int count = s.beginReadArray("imports");
    QListWidgetItem* item;
    for (int i=0; i<count; i++) {
        s.setArrayIndex(i);
        item = new QListWidgetItem(s.value("path").toString());
        item->setFlags(item->flags () | Qt::ItemIsEditable);
        ui->importList->addItem(item);
    }
    s.endArray();

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addItem()));
    connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(removeItem()));
    connect(ui->editButton, SIGNAL(clicked()), this, SLOT(editItem()));
}

ImportPathOptionPage::~ImportPathOptionPage()
{
    delete ui;
}

void ImportPathOptionPage::apply()
{
    QStringList paths;
    QSettings s;
    s.beginWriteArray("imports");
    for (int i=0; i<ui->importList->count(); i++) {
        QString path(ui->importList->item(i)->text());
        paths << path;
        s.setArrayIndex(i);
        s.setValue("path", path);
    }
    s.endArray();
}

void ImportPathOptionPage::addItem()
{
    QString path = QFileDialog::getExistingDirectory(this, "Add Import Path");
    if (path.isEmpty()) {
        return;
    }
    QListWidgetItem* item = new QListWidgetItem(path);
    item->setFlags(item->flags () | Qt::ItemIsEditable);
    ui->importList->addItem(item);
}

void ImportPathOptionPage::removeItem()
{
    QListWidgetItem *item = ui->importList->currentItem();
    if (item) {
        delete item;
    }
}

void ImportPathOptionPage::editItem()
{
    QListWidgetItem *item = ui->importList->currentItem();
    if (item) {
        ui->importList->editItem(item);
    }
}
