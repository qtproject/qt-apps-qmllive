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

#include "contentpluginfactory.h"
#include "contentadapterinterface.h"

#include <QPluginLoader>
#include <QDirIterator>
#include <QDebug>

ContentPluginFactory::ContentPluginFactory(QObject *parent) :
    QObject(parent) ,
    m_loaded(false)
{
    m_pluginPath = QDir::currentPath() + QDir::separator() + "plugins";
}

void ContentPluginFactory::setPluginPath(const QString &path)
{
    m_pluginPath = path;
}

QString ContentPluginFactory::pluginPath()
{
    return m_pluginPath;
}


QList<ContentAdapterInterface *> ContentPluginFactory::plugins()
{
    return m_plugins;
}

bool ContentPluginFactory::isLoaded()
{
    return m_loaded;
}

void ContentPluginFactory::load()
{
    if (m_loaded)
        return;

    QDirIterator it(m_pluginPath);

    while (it.hasNext()) {
        const QString path = it.next();

        if (!it.fileInfo().isFile())
            continue;

        QPluginLoader loader(path);

        ContentAdapterInterface* plugin = qobject_cast<ContentAdapterInterface*>(loader.instance());
        if (plugin)
            m_plugins.append(plugin);

        if (!loader.isLoaded())
            qWarning() << "Error while trying to load" <<path << ":" << loader.errorString();
    }

    if (!m_plugins.isEmpty())
        m_loaded = true;
}
