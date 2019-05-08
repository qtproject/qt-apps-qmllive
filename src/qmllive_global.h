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

#ifndef QMLLIVELIB_GLOBAL_H
#define QMLLIVELIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if !defined(QT_STATIC)
#  if defined(QMLLIVE_LIBRARY)
#    define QMLLIVESHARED_EXPORT Q_DECL_EXPORT
#  else
#    define QMLLIVESHARED_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define QMLLIVESHARED_EXPORT
#endif

#if defined(QMLLIVE_VERSION)
#  define QMLLIVE_SOURCE
#endif

#if defined(QMLLIVE_SOURCE)

#  if !defined(QT_NO_DEBUG) || defined(QT_FORCE_ASSERTS)
#    define LIVE_ASSERT(cond, action) Q_ASSERT(cond)
#  else
#    define LIVE_ASSERT(cond, action) if (cond) {} else {               \
        QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO).critical()      \
            << "Assertion \"" #cond "\" failed";                        \
        action;                                                         \
    } do {} while (0)
#  endif

#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
#  define QtInfoMsg QtWarningMsg
#  define qInfo qWarning
#  define qCInfo qCWarning
#endif

#endif // defined(QMLLIVE_SOURCE)

#endif // QMLLIVELIB_GLOBAL_H
