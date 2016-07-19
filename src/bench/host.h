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

#pragma once

#include <QObject>
#include <QUuid>
#include <QMetaType>

QT_FORWARD_DECLARE_CLASS(QSettings);

class Host : public QObject
{
    Q_OBJECT
public:
    enum Type {
        AutoDiscovery,
        Manual
    };

    //If you add properties here, you maybe also have to change the hostform.ui and the hostform.h (HostChanges)

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QString currentFile READ currentFile WRITE setCurrentFile NOTIFY currentFileChanged)
    Q_PROPERTY(int xOffset READ xOffset WRITE setXOffset NOTIFY xOffsetChanged)
    Q_PROPERTY(int yOffset READ yOffset WRITE setYOffset NOTIFY yOffsetChanged)
    Q_PROPERTY(int rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(bool online READ online WRITE setOnline NOTIFY onlineChanged)
    Q_PROPERTY(bool followTreeSelection READ followTreeSelection WRITE setFollowTreeSelection NOTIFY followTreeSelectionChanged)
    Q_PROPERTY(QUuid autoDiscoveryId READ autoDiscoveryId WRITE setAutoDiscoveryId NOTIFY autoDiscoveryIdChanged)
    Q_PROPERTY(QString productVersion READ productVersion WRITE setProductVersion)
    Q_PROPERTY(QString systemName READ systemName WRITE setSystemName)

    explicit Host(Type type = Manual, QObject *parent = 0);
    Host(const Host& host, QObject *parent = 0);

    QString name() const;
    QString address() const;
    int port() const;
    QString currentFile() const;
    int xOffset() const;
    int yOffset() const;
    int rotation() const;
    Type type() const;

    bool online() const;
    bool followTreeSelection() const;
    QUuid autoDiscoveryId() const;
    QString productVersion() const;
    QString systemName() const;


    void saveToSettings(QSettings *s);
    void restoreFromSettings(QSettings *s);

signals:

    void nameChanged(QString arg);
    void addressChanged(QString arg);
    void portChanged(int arg);
    void currentFileChanged(QString arg);
    void xOffsetChanged(int arg);
    void yOffsetChanged(int arg);
    void rotationChanged(int arg);
    void onlineChanged(bool arg);
    void followTreeSelectionChanged(bool arg);
    void autoDiscoveryIdChanged(QUuid arg);


public slots:

    void setName(QString arg);
    void setAddress(QString arg);
    void setPort(int arg);
    void setCurrentFile(QString arg);
    void setXOffset(int arg);
    void setYOffset(int arg);
    void setRotation(int arg);
    void setOnline(bool arg);
    void setFollowTreeSelection(bool arg);
    void setAutoDiscoveryId(QUuid arg);
    void setProductVersion(QString arg);
    void setSystemName(QString arg);

private:

    QString m_name;
    QString m_address;
    int m_port;
    QString m_currentFile;
    int m_xOffset;
    int m_yOffset;
    int m_rotation;
    Type m_type;
    bool m_online;
    bool m_followTreeSelection;
    QUuid m_autoDiscoveryId;
    QString m_productVersion;
    QString m_systemName;
};

Q_DECLARE_METATYPE( Host* )

