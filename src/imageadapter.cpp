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

#include "imageadapter.h"
#include <QImageReader>
#include <QDebug>
#include <QFileInfo>
#include <QQmlContext>

ImageAdapter::ImageAdapter(QObject *parent) :
    QObject(parent)
{
}

bool ImageAdapter::canPreview(const QString &path) const
{
    QString format = QImageReader::imageFormat(path);
    if (!format.isEmpty()) {
        if (format == "pcx") {
            if (QFileInfo(path).suffix() == "pcx")
                return true;
        } else {
            return true;
        }
    }

    return false;
}

QImage ImageAdapter::preview(const QString &path, const QSize &requestedSize)
{
    QImage img(path);

    if (requestedSize.isValid())
        return img.scaled(requestedSize, Qt::KeepAspectRatio);
    return img;
}

bool ImageAdapter::canAdapt(const QUrl &url) const
{
    return !QImageReader::imageFormat(url.toLocalFile()).isEmpty();
}

QUrl ImageAdapter::adapt(const QUrl &url, QQmlContext *context)
{
    context->setContextProperty("imageViewerBackgroundColor", "black");
    context->setContextProperty("imageViewerSource", url);

    return QUrl("qrc:/livert/imageviewer_qt5.qml");
}
