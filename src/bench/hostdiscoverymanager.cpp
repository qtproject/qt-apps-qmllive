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

#include "hostdiscoverymanager.h"
#include "hostmodel.h"

HostDiscoveryManager::HostDiscoveryManager(QObject *parent) :
    QObject(parent) ,
    m_discoverymodel(new HostModel(this)),
    m_knownhostsmodel(0)
{
}

void HostDiscoveryManager::rescan()
{
}

void HostDiscoveryManager::setKnownHostsModel(HostModel *model)
{
    m_knownhostsmodel = model;
}

HostModel *HostDiscoveryManager::knownHostsModel() const
{
    return m_knownhostsmodel;
}

HostModel *HostDiscoveryManager::discoveredHostsModel() const
{
    return m_discoverymodel;
}

void HostDiscoveryManager::deviceAdded(const QUuid &uuid, const QString &type, int version, const QString &domain)
{
    Q_UNUSED(uuid);
    Q_UNUSED(type);
    Q_UNUSED(version);
    Q_UNUSED(domain);
}

void HostDiscoveryManager::deviceChanged(const QUuid &uuid, const QString &type, int version, const QString &domain)
{
    Q_UNUSED(uuid);
    Q_UNUSED(type);
    Q_UNUSED(version);
    Q_UNUSED(domain);
}

void HostDiscoveryManager::deviceRemoved(const QUuid &uuid, const QString &type, int version, const QString &domain)
{
    Q_UNUSED(uuid);
    Q_UNUSED(type);
    Q_UNUSED(version);
    Q_UNUSED(domain);
}

void HostDiscoveryManager::serviceAdded(const QUuid &uuid, const QString &type, int version, const QString &domain)
{
    Q_UNUSED(uuid);
    Q_UNUSED(type);
    Q_UNUSED(version);
    Q_UNUSED(domain);}

void HostDiscoveryManager::serviceChanged(const QUuid &uuid, const QString &type, int version, const QString &domain)
{
    Q_UNUSED(uuid);
    Q_UNUSED(type);
    Q_UNUSED(version);
    Q_UNUSED(domain);}

void HostDiscoveryManager::serviceRemoved(const QUuid &uuid, const QString &type, int version, const QString &domain)
{
    Q_UNUSED(uuid);
    Q_UNUSED(type);
    Q_UNUSED(version);
    Q_UNUSED(domain);}

void HostDiscoveryManager::updateHostFromExtraValues(Host* host, QMap<QString, QStringList> extraValues)
{
    Q_UNUSED(host);
    Q_UNUSED(extraValues);
}

bool HostDiscoveryManager::checkExtraValues(QMap<QString, QStringList> extraValues)
{
    Q_UNUSED(extraValues);
    return true;
}

void HostDiscoveryManager::serviceAlive(const QUuid &uuid, const QString &type, int version, const QString &domain)
{
    Q_UNUSED(uuid);
    Q_UNUSED(type);
    Q_UNUSED(version);
    Q_UNUSED(domain);
}
