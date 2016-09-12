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
 * \inmodule qmllive
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
 * Sets the active document path to \a path.
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
    emit beginPublishWorkspace();
    QDirIterator iter(m_watcher->directory(), QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    publishDirectory(m_watcher->directory(), false);
    while (iter.hasNext()) {
        publishDirectory(iter.next(), false);
    }
    emit endPublishWorkspace();
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
 * \fn void LiveHubEngine::beginPublishWorkspace()
 *
 * This signal is emitted at the beginning of \l publishWorkspace() call before
 * any \l publishFile signal is emitted.
 */

/*!
 * \fn void LiveHubEngine::endPublishWorkspace()
 *
 * This signal is emitted at the end of \l publishWorkspace() call after
 * all \l publishFile signals were emitted.
 */

/*!
 * \fn void LiveHubEngine::publishFile(const QString& document)
 *
 * This signal is emitted during publishing the directory to inform a connected
 * node to publish the \a document to the remote device form the hub
 */

/*!
 * \fn void LiveHubEngine::fileChanged(const QString& document)
 *
 * This signal is emitted during publishing a directory to inform a connected
 * node that \a document has changed on the hub.
 */

/*!
 * \fn void LiveHubEngine::activateDocument(const QString& document)
 * The signal is emitted when the document identified by \a document has been activated
 */

/*!
 * \fn void LiveHubEngine::workspaceChanged(const QString& workspace)
 * The signal is emitted when the workspace identified by \a workspace has changed
 */

