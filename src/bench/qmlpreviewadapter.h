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

#pragma once

#include "contentadapterinterface.h"
#include <QMutex>
#include <QStringList>
#include <QProcess>

class QmlContext;
class QDeclarativeContext;
class QmlPreviewAdapter : public QObject, public ContentAdapterInterface
{
    Q_OBJECT
    Q_INTERFACES(ContentAdapterInterface)
public:
    explicit QmlPreviewAdapter(QObject *parent = 0);

    bool canPreview(const QString& path) const;
    QImage preview(const QString& path, const QSize &requestedSize);

    bool canAdapt(const QUrl& url) const;
    virtual QUrl adapt(const QUrl& url, QQmlContext* context);

    void setImportPaths(QStringList importPaths);
    void setPluginPaths(QStringList pluginPaths);

private:

    QStringList m_pluginPaths;
    QStringList m_importPaths;
    QMutex m_mutex;
};
