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

#include "directorypreviewadapter.h"
#include <QFileInfo>
#include <QDir>
#include <QQmlContext>
#include <QQmlEngine>

DirectoryPreviewAdapter::DirectoryPreviewAdapter(QObject *parent) :
    QObject(parent)
{
}

bool DirectoryPreviewAdapter::canPreview(const QString &path) const
{
    Q_UNUSED(path);

    return false;
}

QImage DirectoryPreviewAdapter::preview(const QString &path, const QSize &requestedSize)
{
    Q_UNUSED(path);
    Q_UNUSED(requestedSize);

    return QImage();
}


bool DirectoryPreviewAdapter::isFullScreen() const
{
    return true;
}

bool DirectoryPreviewAdapter::canAdapt(const QUrl &url) const
{
    QFileInfo info(url.toLocalFile());
    return info.isDir();
}
QUrl DirectoryPreviewAdapter::adapt(const QUrl &url, QQmlContext *context)
{
    QDir dir(url.toLocalFile());

    context->setContextProperty("files", dir.entryList(QDir::Files | QDir::NoDotDot | QDir::NoDot | QDir::NoSymLinks));
    context->setContextProperty("path", url.toLocalFile());
    context->setContextProperty("adapter", this);

    if (availableFeatures().testFlag(QtQuickControls))
        return QUrl("qrc:/livert/folderview_qt5_controls.qml");

    return QUrl("qrc:/livert/folderview_qt5.qml");
}

