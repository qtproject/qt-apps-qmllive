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

#include "overlay.h"

namespace {
const char *const OVERLAY_PATH_PREFIX = "qml-live-overlay--";
const char OVERLAY_PATH_SEPARATOR = '-';
}

Overlay::Overlay(const QString &basePath, QObject *parent) : QObject(parent)
  , m_basePath(basePath)
  , m_overlay(overlayTemplatePath())
{
  if (!m_overlay.isValid())
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
      qFatal("Failed to create overlay directory: %s", qPrintable(m_overlay.errorString()));
#else
      qFatal("Failed to create overlay directory");
#endif

}

Overlay::~Overlay()
{

}

QDir Overlay::overlay() const
{
    return m_overlay.path();
}

QString Overlay::reserve(const LiveDocument &document)
{
    QWriteLocker locker(&m_lock);
    QString overlayingPath = document.absoluteFilePathIn(m_overlay.path());
    m_mappings.insert(document.absoluteFilePathIn(m_basePath), overlayingPath);
    return overlayingPath;
}

QString Overlay::map(const QString &file) const
{
    QReadLocker locker(&m_lock);
    return m_mappings.value(file, file);
}

QString Overlay::overlayTemplatePath()
{
    QSettings settings;
    QString overlayTemplatePath = QDir::tempPath() + QDir::separator() + QLatin1String(OVERLAY_PATH_PREFIX);

    if (!settings.organizationName().isEmpty()) // See QCoreApplication::organizationName's docs
        overlayTemplatePath += settings.organizationName() + QLatin1Char(OVERLAY_PATH_SEPARATOR);

    overlayTemplatePath += settings.applicationName();
    return overlayTemplatePath;
}
