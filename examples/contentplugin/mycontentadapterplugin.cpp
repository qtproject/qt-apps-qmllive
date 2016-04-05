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
#include "mycontentadapterplugin.h"

#include <QtCore/QtPlugin>
#include <QtDeclarative/QDeclarativeContext>

MyContentAdapterPlugin::MyContentAdapterPlugin(QObject *parent) :
    QObject(parent)
{
}

//! [2]
bool MyContentAdapterPlugin::canPreview(const QString &path) const
{
    Q_UNUSED(path)

    return false;
}

QImage MyContentAdapterPlugin::preview(const QString &path, const QSize &requestedSize)
{
    Q_UNUSED(path);
    Q_UNUSED(requestedSize);

    return QImage();
}
//! [2]

//! [0]
bool MyContentAdapterPlugin::canAdapt(const QUrl &url) const
{
    return url.toLocalFile().endsWith(".png");
}
//! [0]

//! [1]
QUrl MyContentAdapterPlugin::adapt(const QUrl &url, QDeclarativeContext *context)
{
    context->setContextProperty("imageSource", url);

    return QString("qrc:/mycontentadatperplugin/plugin.qml");
}
//! [1]

Q_EXPORT_PLUGIN2(myContentAdapterPlugin, MyContentAdapterPlugin)
