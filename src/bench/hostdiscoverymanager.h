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

#include <QObject>
#include <QtCore>

class HostModel;
class Host;
class HostDiscoveryManager : public QObject
{
    Q_OBJECT
public:
    explicit HostDiscoveryManager(QObject *parent = 0);

    void rescan();

    void setKnownHostsModel(HostModel* model);
    HostModel *knownHostsModel() const;

    HostModel *discoveredHostsModel() const;

private slots:
    void deviceAdded(const QUuid &uuid, const QString &type, int version, const QString &domain);
    void deviceChanged(const QUuid &uuid, const QString &type, int version, const QString &domain);
    void deviceRemoved(const QUuid &uuid, const QString &type, int version, const QString &domain);

    void serviceAdded(const QUuid &uuid, const QString &type, int version, const QString &domain);
    void serviceChanged(const QUuid &uuid, const QString &type, int version, const QString &domain);
    void serviceRemoved(const QUuid &uuid, const QString &type, int version, const QString &domain);
    void serviceAlive(const QUuid &uuid, const QString &type, int version, const QString &domain);

    void updateHostFromExtraValues(Host *host, QMap<QString, QStringList> extraValues);
    bool checkExtraValues(QMap<QString, QStringList> extraValues);

private:
    HostModel* m_discoverymodel;
    HostModel* m_knownhostsmodel;
};

