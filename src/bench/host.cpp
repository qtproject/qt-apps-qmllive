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

#include "host.h"
#include <QDebug>
#include <QSettings>

Host::Host(Type type, QObject *parent) :
    QObject(parent),
    m_port(10234),
    m_xOffset(0),
    m_yOffset(0),
    m_rotation(0),
    m_type(type),
    m_online(false),
    m_followTreeSelection(false)
{
}

Host::Host(const Host &host, QObject *parent) :
    QObject(parent),
    m_name(host.name()),
    m_address(host.address()),
    m_port(host.port()),
    m_currentFile(host.currentFile()),
    m_xOffset(host.xOffset()),
    m_yOffset(host.yOffset()),
    m_rotation(host.rotation()),
    m_type(host.type()),
    m_online(host.online()),
    m_followTreeSelection(host.followTreeSelection()),
    m_autoDiscoveryId(host.autoDiscoveryId()),
    m_productVersion(host.productVersion()),
    m_systemName(host.systemName())
{
}

QString Host::name() const
{
    return m_name;
}

QString Host::address() const
{
    return m_address;
}

QString Host::currentFile() const
{
    return m_currentFile;
}

int Host::xOffset() const
{
    return m_xOffset;
}

int Host::yOffset() const
{
    return m_yOffset;
}

int Host::rotation() const
{
    return m_rotation;
}

void Host::setName(QString arg)
{
    if (m_name != arg) {
        m_name = arg;
        emit nameChanged(arg);
    }
}

void Host::setAddress(QString arg)
{
    if (m_address != arg) {
        m_address = arg;
        emit addressChanged(arg);
    }
}

void Host::setCurrentFile(QString arg)
{
    m_currentFile = arg;
    emit currentFileChanged(arg);
}

void Host::setXOffset(int arg)
{
    if (m_xOffset != arg) {
        m_xOffset = arg;
        emit xOffsetChanged(arg);
    }
}

void Host::setYOffset(int arg)
{
    if (m_yOffset != arg) {
        m_yOffset = arg;
        emit yOffsetChanged(arg);
    }
}

void Host::setRotation(int arg)
{
    if (m_rotation != arg) {
        m_rotation = arg;
        emit rotationChanged(arg);
    }
}

void Host::setOnline(bool arg)
{
    if (m_online != arg) {
        m_online = arg;
        emit onlineChanged(arg);
    }
}

void Host::setFollowTreeSelection(bool arg)
{
    if (m_followTreeSelection != arg) {
        m_followTreeSelection = arg;
        emit followTreeSelectionChanged(arg);
    }
}

void Host::setAutoDiscoveryId(QUuid arg)
{
    if (m_autoDiscoveryId != arg) {
        m_autoDiscoveryId = arg;
        emit autoDiscoveryIdChanged(arg);
    }
}

void Host::setProductVersion(QString arg)
{
    m_productVersion = arg;
}

void Host::setSystemName(QString arg)
{
    m_systemName = arg;
}

void Host::setPort(int arg)
{
    if (m_port != arg) {
        m_port = arg;
        emit portChanged(arg);
    }
}

Host::Type Host::type() const
{
    return m_type;
}

bool Host::online() const
{
    return m_online;
}

bool Host::followTreeSelection() const
{
    return m_followTreeSelection;
}

void Host::saveToSettings(QSettings *s)
{
    Q_ASSERT(s);

    s->setValue("name", name());
    s->setValue("address", address());
    s->setValue("port", port());
    s->setValue("type", type());
    s->setValue("followTreeSelection", followTreeSelection());
    s->setValue("xOffset", xOffset());
    s->setValue("yOffset", yOffset());
    s->setValue("rotation", rotation());
    s->setValue("currentFile", currentFile());
    s->setValue("autoDiscoveryId", autoDiscoveryId().toString());
    s->setValue("systemName", systemName());
    s->setValue("productVersion", productVersion());
}

void Host::restoreFromSettings(QSettings *s)
{
    Q_ASSERT(s);

    setName(s->value("name").toString());
    setAddress(s->value("address").toString());
    setPort(s->value("port").toInt());
    m_type = static_cast<Host::Type>(s->value("type").toInt());
    setFollowTreeSelection(s->value("followTreeSelection").toBool());
    setXOffset(s->value("xOffset").toInt());
    setYOffset(s->value("yOffset").toInt());
    setRotation(s->value("rotation").toInt());
    setCurrentFile(s->value("currentFile").toString());
    setAutoDiscoveryId(QUuid(s->value("autoDiscoveryId").toString()));
    setSystemName(s->value("systemName").toString());
    setProductVersion(s->value("productVersion").toString());
}

QString Host::productVersion() const
{
    return m_productVersion;
}

QString Host::systemName() const
{
    return m_systemName;
}

QUuid Host::autoDiscoveryId() const
{
    return m_autoDiscoveryId;
}

int Host::port() const
{
    return m_port;
}
