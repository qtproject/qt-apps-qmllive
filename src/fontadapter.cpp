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

#include "fontadapter.h"
#include <QDebug>
#include <QQmlContext>

FontAdapter::FontAdapter(QObject *parent) :
    QObject(parent),
    fontId(-1)
{
    fontExtensions.append(".ttf");
    fontExtensions.append(".otf");
}

void FontAdapter::cleanUp()
{
    base.removeAllApplicationFonts();
}

bool FontAdapter::canPreview(const QString &path) const
{
    Q_UNUSED(path);

    return false;
}

QImage FontAdapter::preview(const QString &path, const QSize &requestedSize)
{
    Q_UNUSED(path);
    Q_UNUSED(requestedSize);

    return QImage();
}

bool FontAdapter::canAdapt(const QUrl &url) const
{
    QString path = url.toLocalFile();

    foreach (const QString& extension, fontExtensions) {
        if (path.endsWith(extension))
            return true;
    }

    return false;
}

bool FontAdapter::isFullScreen() const
{
    return true;
}

QUrl FontAdapter::adapt(const QUrl &url, QQmlContext *context)
{
    fontId = base.addApplicationFont(url.toLocalFile());

    QStringList families = base.applicationFontFamilies(fontId);

    QVariantList styles;
    foreach (QString family, families) {
        QVariantMap style;
        QVariantList weights;
        foreach (QString styleName, base.styles(family)) {
            QVariantMap newWeight;
            int styleWeight = base.weight(family, styleName);

            newWeight.insert("name", styleName);
            newWeight.insert("weight", styleWeight);

            bool added = false;
            for (int i=0; i < weights.count(); i++) {
                if (styleWeight <= weights.at(i).toMap().value("weight").toInt()) {
                    added = true;
                    weights.insert(i, newWeight);
                    break;
                }
            }

            if (!added)
                weights.append(newWeight);
        }

        style.insert("family", family);
        style.insert("weights", weights);
        styles.append(style);
    }

    context->setContextProperty("styles", styles);

    if (availableFeatures().testFlag(QtQuickControls))
        return QUrl("qrc:/livert/fontviewer_qt5_controls.qml");

    return QUrl("qrc:/livert/fontviewer_qt5.qml");
}
