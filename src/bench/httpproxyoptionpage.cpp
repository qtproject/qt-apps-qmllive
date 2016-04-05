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

#include "httpproxyoptionpage.h"
#include "ui_httpproxyoptionpage.h"
#include <QtNetwork>

HttpProxyOptionPage::HttpProxyOptionPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HttpProxyOptionPage)
{
    ui->setupUi(this);
    QSettings s;
    ui->proxyGroup->setChecked(s.value("http_proxy/enabled").toBool());
    ui->serverField->setText(s.value("http_proxy/hostname").toString());
    ui->portField->setText(s.value("http_proxy/port").toString());
    ui->userField->setText(s.value("http_proxy/username").toString());
    ui->passwordField->setText(s.value("http_proxy/password").toString());
}

HttpProxyOptionPage::~HttpProxyOptionPage()
{
    delete ui;
}

void HttpProxyOptionPage::apply()
{
    bool enabled = ui->proxyGroup->isChecked();
    QString hostname = ui->serverField->text();
    int port = ui->portField->text().toInt();
    QString username = ui->userField->text();
    QString password = ui->passwordField->text();
    if (enabled) {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(hostname);
        proxy.setPort(port);
        proxy.setUser(username);
        proxy.setPassword(password);
        QNetworkProxy::setApplicationProxy(proxy);
    } else {
        QNetworkProxy::setApplicationProxy(QNetworkProxy());
    }
    QSettings s;
    s.setValue("http_proxy/enabled", enabled);
    s.setValue("http_proxy/hostname", hostname);
    s.setValue("http_proxy/port", port);
    s.setValue("http_proxy/username", username);
    s.setValue("http_proxy/password", password);

}
