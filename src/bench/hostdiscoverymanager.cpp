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

void HostDiscoveryManager::updateHostFromExtraValues(Host *host, QMap<QString, QStringList> extraValues)
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
