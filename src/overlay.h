/****************************************************************************
**
** Copyright (C) 2019 Luxoft Sweden AB
** Copyright (C) 2018 Jolla Ltd
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QML Live tool.
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

#ifndef OVERLAY_H
#define OVERLAY_H

#include <QObject>
#include "qmllive_global.h"
#include "livedocument.h"

// Overlay uses temporary directory to allow parallel execution
class QMLLIVESHARED_EXPORT Overlay : public QObject
{
    Q_OBJECT
public:
    Overlay(const QString &basePath, QObject *parent);
    ~Overlay();

    QString reserve(const LiveDocument &document);
    QDir overlay() const;
    QString map(const QString &file) const;

private:
    static QString overlayTemplatePath();

private:
    mutable QReadWriteLock m_lock;
    QHash<QString, QString> m_mappings;
    QString m_basePath;
    QTemporaryDir m_overlay;
};

#endif // OVERLAY_H
