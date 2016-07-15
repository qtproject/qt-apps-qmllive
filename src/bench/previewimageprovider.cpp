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

#include "previewimageprovider.h"
#include "contentadapterinterface.h"

#include <QDebug>
#include <QFileIconProvider>
#include <QPainter>


class CacheImage {
public:
    QImage image;
    QDateTime time;
};

typedef QHash<QString, CacheImage> ImageHash;
typedef QHash<QString, QImage> QImageHash;

Q_GLOBAL_STATIC(ImageHash, imageCache)
Q_GLOBAL_STATIC(QImageHash, iconCache)

PreviewImageProvider::PreviewImageProvider(QObject *engine)
    : QQuickImageProvider(QQuickImageProvider::Image, QQmlImageProviderBase::ForceAsynchronousImageLoading)
    , m_engine(engine)
    , m_ignoreCache(false)
{
}

QImage PreviewImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_ASSERT(size);
    QFileInfo info(id);
    info.refresh();
    QList<ContentAdapterInterface*> plugins;
    QString hashKey = id + QString("%1x%2").arg(QString::number(requestedSize.width())).arg(QString::number(requestedSize.height()));

    {
        QMutexLocker m(&m_mutex);
        if (!m_ignoreCache && imageCache()->contains(hashKey)) {
            CacheImage i = imageCache()->value(hashKey);
            if (info.lastModified() <= i.time) {
                *size = i.image.size();
                return i.image;
            }
        }

        plugins = m_plugins;
    }

    foreach (ContentAdapterInterface *plugin, plugins) {
        if (plugin->canPreview(id)) {
            QImage img = plugin->preview(id, requestedSize);
            *size = img.size();
            CacheImage i;
            i.image = img;
            i.time = info.lastModified();
            imageCache()->insert(hashKey, i);

            return img;
        }
    }


    QImage img;
    QString type = info.suffix();
    QSize iconSize = requestedSize;
    if (!iconSize.isValid() || iconSize.isNull())
        iconSize = QSize(512, 512);

    if (iconCache()->contains(type)) {
        img = iconCache()->value(type);
        *size = img.size();
    } else if (m_engine){
        QMetaObject::invokeMethod(m_engine, "convertIconToImage",
                                  Qt::BlockingQueuedConnection,
                                  Q_RETURN_ARG(QImage, img),
                                  Q_ARG(QFileInfo, info),
                                  Q_ARG(QSize, iconSize));
        iconCache()->insert(type, img);
        *size = iconSize;
    }

    return img;
}

void PreviewImageProvider::setPlugins(QList<ContentAdapterInterface *> plugins)
{
    QMutexLocker m(&m_mutex);
    m_plugins = plugins;
}

void PreviewImageProvider::setIgnoreCache(bool enabled)
{
    QMutexLocker m(&m_mutex);
    m_ignoreCache = enabled;
}

bool PreviewImageProvider::ignoreCache()
{
    QMutexLocker m(&m_mutex);
    return m_ignoreCache;
}
