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

#include <QAbstractListModel>
#include "host.h"

QT_FORWARD_DECLARE_CLASS(QSettings);
class HostModel : public QAbstractListModel
{
    Q_OBJECT
public:

    enum HostRoles {
        NameRole = Qt::UserRole,
        AddressRole
    };

    explicit HostModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void addHost(Host* host);
    void removeHost(Host* host);
    void removeHost(int index);
    int indexOf(Host* host);
    int indexOf(const QString &hostName);
    void clear();

    QList<Host*> findByAutoDiscoveryId(QUuid id) const;

    Host* host(const QString &name) const;
    Host* hostAt(int index) const;

    void restoreFromSettings(QSettings* s);
    void saveToSettings(QSettings* s);

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
protected:
    Qt::DropActions supportedDropActions() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

private slots:
    void onHostChanged();
private:
    QList<Host*> m_hosts;
};

