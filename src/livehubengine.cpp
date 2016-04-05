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

#include "livehubengine.h"
#include "watcher.h"

#ifdef QMLLIVE_DEBUG
#define DEBUG qDebug()
#else
#define DEBUG if (0) qDebug()
#endif

/*!
 * \class LiveHubEngine
 * \brief The LiveHubEngine class watches over a workspace and notifies a node on changes
 * \group qmllive
 *
 * The live hub watches over a workspace and notifies a live node about changed files. A
 * node can run on the same device or even on a remote device using a RemotePublisher.
 */

/*!
 * Standard constructor using \a parent as parent
 */
LiveHubEngine::LiveHubEngine(QObject *parent)
    : QObject(parent)
    , m_watcher(new Watcher(this))
    , m_filePublishingActive(false)
{
    connect(m_watcher, SIGNAL(directoriesChanged(QStringList)), this, SLOT(directoriesChanged(QStringList)));
}

/*!
 * Sets the workspace folder to watch over to \a path
 */
void LiveHubEngine::setWorkspace(const QString &path)
{
    m_watcher->setDirectory(path);

    emit workspaceChanged(path);
}

/*!
 * Returns the current workspace
 */
QString LiveHubEngine::workspace() const
{
    return m_watcher->directory();
}

/*!
 * Sets the active document path to path.
 * Emits activateDocument() with the workspace relative path.
 */
void LiveHubEngine::setActivePath(const QString &path)
{
    m_activePath = path;
    emit activateDocument(m_watcher->relativeFilePath(path));
}

/*!
 * Returns the active Document
 */
QString LiveHubEngine::activePath() const
{
    return m_activePath;
}

/*!
 * Handles watcher changes signals.
 */
void LiveHubEngine::directoriesChanged(const QStringList &changes)
{
    DEBUG << "LiveHubEngine::workspaceChanged: " << changes;
    if (m_filePublishingActive) {
        foreach (const QString& change, changes) {
            publishDirectory(change, true);
        }
    }

    emit activateDocument(m_watcher->relativeFilePath(m_activePath));
}

/*!
 * Publish the whole workspace to a connected node.
 */
void LiveHubEngine::publishWorkspace()
{
    if (!m_filePublishingActive) { return; }
    QDirIterator iter(m_watcher->directory(), QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    publishDirectory(m_watcher->directory(), false);
    while (iter.hasNext()) {
        publishDirectory(iter.next(), false);
    }
}

/*!
 * Publish the directory \a dirPath to a connected node.
 */
void LiveHubEngine::publishDirectory(const QString& dirPath, bool fileChange)
{
    if (!m_filePublishingActive) { return; }
    QDirIterator iter(dirPath, QDir::Files);
    while (iter.hasNext()) {
        if (fileChange)
            emit fileChanged(iter.next());
        else
            emit publishFile(iter.next());
    }
}

/*!
 * Sets the file publishing to \a on
 */
void LiveHubEngine::setFilePublishingActive(bool on)
{
    m_filePublishingActive = on;
}

/*!
 * \fn void LiveHubEngine::workspaceChanged()
 * Emits that the workspace changed
 */

/*!
 * \fn void LiveHubEngine::activateDocument(const QString& document)
 * The document \a document is now active
 */

/*!
 * \fn void LiveHubEngine::sendDocument(const QString& document, const QByteArray& data)
 * The Document \a document with the content \a data was sent
 */
