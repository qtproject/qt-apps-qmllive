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

#include <QQuickImageProvider>
#include <QMutex>
#include <QHash>
#include <QDateTime>

class ContentAdapterInterface;
class PreviewImageProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT

public:
    explicit PreviewImageProvider(QObject *engine);

    QImage requestImage (const QString& id, QSize* size, const QSize& requestedSize);

    void setPlugins(QList<ContentAdapterInterface*> plugins);
    void setIgnoreCache(bool enabled);
    bool ignoreCache();

private:

    QList<ContentAdapterInterface*> m_plugins;
    QMutex m_mutex;
    QObject* m_engine;
    bool m_ignoreCache;
};
