/****************************************************************************
**
** Copyright (C) 2016 Pelagicore AG
** Contact: http://www.qt.io/ or http://www.pelagicore.com/
**
** This file is part of the QmlLive tool.
**
** $QT_BEGIN_LICENSE:GPL3-PELAGICORE$
** Commercial License Usage
** Licensees holding valid commercial Pelagicore QmlLive
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

#include "qmlhelper.h"


/*!
 * \class QmlHelper
 * \brief Provides a set of helper functions to setup your qml viewer
 * \group qmllive
 */

/*!
 * \brief Standard constructor using \a parent as parent
 */
QmlHelper::QmlHelper(QObject *parent) :
    QObject(parent)
{
}

/*!
 * Loads dummy data from a "dummydata" folder in the workspace folder
 * \a view defines the View where you want to export the dummy data to
 * The "dummydata" will be searched in the "dummydata" sub directory of \a workspace
 */
void QmlHelper::loadDummyData(QQuickView *view, const QString &workspace)
{
    Q_ASSERT(view);
    QDir dir(workspace + "/dummydata", "*.qml");
    foreach (QString entry, dir.entryList()) {

        QQmlComponent comp(view->engine(), dir.filePath(entry));
        QObject* obj = comp.create();

        if (comp.isError()) {
            foreach (const QQmlError error, comp.errors()) {
                qWarning() << error;
            }
        }
        if (obj) {
            qWarning() << "loaded dummy data: " << dir.filePath(entry);
            entry.chop(4);
            view->rootContext()->setContextProperty(entry, obj);
            obj->setParent(view);
        }
    }
}

/*!
 * \fn void QmlHelper::addImportPath(QQuickView *view, const QString &path)
 * Adds an import path \a path to your qml viewer \a view
 */
void QmlHelper::addImportPath(QQuickView *view, const QString &path)
{
    Q_ASSERT(view);
    view->engine()->addImportPath(path);
}




