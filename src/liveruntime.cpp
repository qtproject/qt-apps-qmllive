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

#include "liveruntime.h"

// TODO: create a variant model for dynamic passing of key value pairs with notify
// TODO: create support for background and overlay layer image files livert.background, livert.overlay

/*!
 * \class LiveRuntime
 * \brief Collects properties to be used for an enhanced live runtime.
 * \group qmllive
 *
 * This runtime is used in an live enhanced qml project to be able to access more
 * advanced features. Currently it does nothing
 */

/*!
 * Standard constructor using \a parent as parent
 */
LiveRuntime::LiveRuntime(QObject *parent) :
    QObject(parent)
{
}

void LiveRuntime::setScreenHeight(qreal arg)
{
    if (m_screenHeight != arg) {
        m_screenHeight = arg;
        emit screenHeightChanged(arg);
    }
}

qreal LiveRuntime::screenWidth() const
{
    return m_screenWidth;
}

qreal LiveRuntime::screenHeight() const
{
    return m_screenHeight;
}

void LiveRuntime::setScreenWidth(qreal arg)
{
    if (m_screenWidth != arg) {
        m_screenWidth = arg;
        emit screenWidthChanged(arg);
    }
}
