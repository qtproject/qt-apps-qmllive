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

        if (!loader.load()) {
            qWarning() << "Error while trying to load" <<path << ":" << loader.errorString();
            continue;
        }

        ContentAdapterInterface *plugin = qobject_cast<ContentAdapterInterface*>(loader.instance());
        if (plugin) {
            loader.instance()->setParent(this);
            m_plugins.append(plugin);
        } else {
            qWarning() << "Error while trying to load" <<path << ": Unsupported root component type"
                       << loader.instance()->metaObject()->className();
            loader.unload();
        }
    }

    if (!m_plugins.isEmpty())
        m_loaded = true;
}
