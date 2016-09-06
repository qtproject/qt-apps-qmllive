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
#include <QUrl>
#include <QImage>

#include "qmllive_global.h"

QT_FORWARD_DECLARE_CLASS(QDeclarativeContext);
QT_FORWARD_DECLARE_CLASS(QQmlContext);

//! [0]
class QMLLIVESHARED_EXPORT ContentAdapterInterface
{
public:

    enum Feature {
        QtQuickControls = 0x1
    };
    Q_DECLARE_FLAGS(Features, Feature)

    virtual ~ContentAdapterInterface() {}

    virtual void cleanUp() {}

    virtual bool canPreview(const QString& path) const = 0;
    virtual QImage preview(const QString& path, const QSize &requestedSize) = 0;

    virtual bool canAdapt(const QUrl& url) const = 0;
    virtual bool isFullScreen() const { return false; }

    void setAvailableFeatures(ContentAdapterInterface::Features features) { m_features = features; }
    ContentAdapterInterface::Features availableFeatures() { return m_features; }

    virtual QUrl adapt(const QUrl& url, QQmlContext* context) = 0;

private:
    Features m_features;
};
//! [0]

Q_DECLARE_OPERATORS_FOR_FLAGS(ContentAdapterInterface::Features)

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(ContentAdapterInterface, "com.pelagicore.qmllive.ContentAdapterInterface/1.0")
QT_END_NAMESPACE

