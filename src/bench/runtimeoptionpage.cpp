/****************************************************************************
**
** Copyright (C) 2018 Luxoft Sweden AB
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

#include "runtimeoptionpage.h"
#include "ui_runtimeoptionpage.h"
#include "constants.h"

#include <QFileDialog>
#include <QPushButton>

RuntimeOptionPage::RuntimeOptionPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RuntimeOptionPage)
{
    ui->setupUi(this);

    QSettings s;
    if (s.value(Constants::RUNTIME_SETTINGS_KEY()).toString().isEmpty()) {
        QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("qmlliveruntime");
    } else {
        ui->runtimePathText->setText(s.value(Constants::RUNTIME_SETTINGS_KEY()).toString());
    }

    ui->warningLabel->setText("");
    QPalette palette = ui->warningLabel->palette();
    palette.setColor(ui->warningLabel->backgroundRole(), Qt::yellow);
    palette.setColor(ui->warningLabel->foregroundRole(), Qt::yellow);
    ui->warningLabel->setPalette(palette);

    connect(ui->select, &QPushButton::clicked, this, &RuntimeOptionPage::selectRuntimePath);
}

RuntimeOptionPage::~RuntimeOptionPage()
{
    delete ui;
}

void RuntimeOptionPage::selectRuntimePath()
{
    ui->warningLabel->setText("");
    QString runtimePath = QFileDialog::getOpenFileName(this, "Select QML Live runtime");
    if (!runtimePath.isEmpty() && ui->runtimePathText) {
        ui->runtimePathText->setText(runtimePath);
    }
}

bool RuntimeOptionPage::apply()
{
    QString path = ui->runtimePathText->text();

    if (QFileInfo(path).exists()) {
        ui->warningLabel->setText("");
        QSettings s;
        s.setValue(Constants::RUNTIME_SETTINGS_KEY(), path);
        emit updateRuntimePath(path);
        return true;
    } else {
        ui->warningLabel->setText("File doesn't exist.");
        return false;
    }
}
