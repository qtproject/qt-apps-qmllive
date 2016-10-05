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

#include "options.h"

Options::Options(QObject *parent)
    : QObject(parent)
    , m_noRemote(false)
    , m_remoteOnly(false)
    , m_ping(false)
    , m_stayOnTop(false)
{

}

bool Options::hasNoninteractiveOptions() const
{
    if (!m_hostsToAdd.isEmpty())
        return true;

    if (!m_hostsToRemove.isEmpty())
        return true;

    if (!m_hostsToProbe.isEmpty())
        return true;

    return false;
}

bool Options::noRemote() const
{
    return m_noRemote;
}

void Options::setNoRemote(bool noRemote)
{
    m_noRemote = noRemote;
}

bool Options::remoteOnly() const
{
    return m_remoteOnly;
}

void Options::setRemoteOnly(bool remoteOnly)
{
    m_remoteOnly = remoteOnly;
}

bool Options::ping() const
{
    return m_ping;
}

void Options::setPing(bool ping)
{
    m_ping = ping;
}

QString Options::activeDocument() const
{
    return m_activeDocument;
}

void Options::setActiveDocument(const QString &activeDocument)
{
    m_activeDocument = activeDocument;
}

QString Options::workspace() const
{
    return m_workspace;
}

void Options::setWorkspace(const QString &workspace)
{
    m_workspace = workspace;
}

QString Options::pluginPath() const
{
    return m_pluginPath;
}

void Options::setPluginPath(const QString &pluginPath)
{
    m_pluginPath = pluginPath;
}

QStringList Options::importPaths() const
{
    return m_importPaths;
}

void Options::setImportPaths(const QStringList &importPaths)
{
    m_importPaths = importPaths;
}

void Options::addImportPath(const QString &path)
{
    m_importPaths.append(path);
}

void Options::clearImportPaths()
{
    m_importPaths.clear();
}

bool Options::stayOnTop() const
{
    return m_stayOnTop;
}

void Options::setStayOnTop(bool stayOnTop)
{
    m_stayOnTop = stayOnTop;
}

QList<Options::HostOptions> Options::hostsToAdd() const
{
    return m_hostsToAdd;
}

void Options::addHostToAdd(const HostOptions &hostOptions)
{
    m_hostsToAdd.append(hostOptions);
}

QStringList Options::hostsToRemove() const
{
    return m_hostsToRemove;
}

void Options::setHostsToRemove(const QStringList &hostNames)
{
    m_hostsToRemove = hostNames;
}

QStringList Options::hostsToProbe() const
{
    return m_hostsToProbe;
}

void Options::setHostsToProbe(const QStringList &hostNames)
{
    m_hostsToProbe = hostNames;
}
