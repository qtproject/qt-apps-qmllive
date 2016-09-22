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

#include <QtGlobal>

const char *const QMLLIVE_COPYRIGHT_NOTICE = "Copyright 2016 Pelagicore AG.  All rights reserved.";

const char *const QMLLIVE_VERSION_STR = QT_STRINGIFY(QMLLIVE_VERSION);

#ifdef QMLLIVE_VERSION_EXTRA
const char *const QMLLIVE_VERSION_EXTRA_STR = QT_STRINGIFY(QMLLIVE_VERSION_EXTRA);
#else
const char *const QMLLIVE_VERSION_EXTRA_STR = "";
#endif

#ifdef QMLLIVE_REVISION
const char *const QMLLIVE_REVISION_STR = QT_STRINGIFY(QMLLIVE_REVISION);
#else
const char *const QMLLIVE_REVISION_STR = "";
#endif

#ifdef QMLLIVE_SETTINGS_VARIANT
const char *const QMLLIVE_ORGANIZATION_NAME = QT_STRINGIFY(QMLLIVE_SETTINGS_VARIANT);
const char *const QMLLIVE_ORGANIZATION_DOMAIN = "";
#else
const char *const QMLLIVE_ORGANIZATION_NAME = "Pelagicore";
const char *const QMLLIVE_ORGANIZATION_DOMAIN = "pelagicore.com";
#endif
