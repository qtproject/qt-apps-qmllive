/****************************************************************************
**
** Copyright (C) 2019 Luxoft Sweden AB
** Copyright (C) 2018 Pelagicore AG
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

#pragma once

#include <QtCore>

#include "qmllive_global.h"

class ResourceMap;

class QMLLIVESHARED_EXPORT LiveDocument
{
    Q_DECLARE_TR_FUNCTIONS(LiveDocument)

public:
    LiveDocument();
    explicit LiveDocument(const QString &relativeFilePath);

    bool isNull() const { return m_relativeFilePath.isEmpty(); }
    QString errorString() const { return m_errorString; }

    bool existsIn(const QDir &workspace) const;
    bool isFileIn(const QDir &workspace) const;

    QString relativeFilePath() const;
    QString absoluteFilePathIn(const QDir &workspace) const;

    bool mapsToResource(const ResourceMap &resourceMap) const;
    QUrl runtimeLocation(const QDir &workspace, const ResourceMap &resourceMap) const;

    static LiveDocument resolve(const QDir &workspace, const QString &filePath);
    static LiveDocument resolve(const QDir &workspace, const ResourceMap &resourceMap, const QString
            &filePath);
    static LiveDocument resolve(const QDir &workspace, const ResourceMap &resourceMap, const QUrl &fileUrl);

    static QString toFilePath(const QUrl &url);
    static QUrl toUrl(const QString &filePath);

    friend inline bool operator==(const LiveDocument &d1, const LiveDocument &d2)
    { return d1.m_relativeFilePath == d2.m_relativeFilePath; }
    friend inline bool operator!=(const LiveDocument &d1, const LiveDocument &d2)
    { return !(d1 == d2); }

private:
    QString m_relativeFilePath;
    mutable QString m_errorString;
};

QDebug QMLLIVESHARED_EXPORT operator<<(QDebug dbg, const LiveDocument &document);
