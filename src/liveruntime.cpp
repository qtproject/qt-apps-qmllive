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

#include "liveruntime.h"

// TODO: create a variant model for dynamic passing of key value pairs with notify
// TODO: create support for background and overlay layer image files livert.background, livert.overlay

/*!
 * \class LiveRuntime
 * \brief Collects properties to be used for an enhanced live runtime.
 * \inmodule qmllive
 *
 * This runtime is used in an live enhanced qml project to be able to access more
 * advanced features. Currently it does nothing
 */


/*!
 * Standard constructor using \a parent as parent
 */
LiveRuntime::LiveRuntime(QObject *parent) :
    QObject(parent),
    m_screenWidth(0),
    m_screenHeight(0)
{
}

/*!
 * Sets the screen width value \a arg
 */
void LiveRuntime::setScreenWidth(qreal arg)
{
    if (m_screenWidth != arg) {
        m_screenWidth = arg;
        emit screenWidthChanged(arg);
    }
}

/*!
 * Sets the screen height value \a arg
 */
void LiveRuntime::setScreenHeight(qreal arg)
{
    if (m_screenHeight != arg) {
        m_screenHeight = arg;
        emit screenHeightChanged(arg);
    }
}

/*!
 * Return the screen width
 */
qreal LiveRuntime::screenWidth() const
{
    return m_screenWidth;
}

/*!
 * Return the screen height
 */
qreal LiveRuntime::screenHeight() const
{
    return m_screenHeight;
}

/*!
 * \property LiveRuntime::screenWidth
 * This propety defines the screen width
 */

/*!
 * \property LiveRuntime::screenHeight
 * This propety defines the screen height
 */


