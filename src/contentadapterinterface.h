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
#include <QUrl>
#include <QImage>

class QDeclarativeContext;
class QQmlContext;
//! [0]
class ContentAdapterInterface
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
Q_DECLARE_INTERFACE(ContentAdapterInterface, "com.pelagicore.qmllive.ContentAdapterInterface/1.0")

