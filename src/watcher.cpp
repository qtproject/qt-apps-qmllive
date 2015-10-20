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

#include "watcher.h"


/*!
 \class Watcher
 \internal
 \brief A class which watches Directories and notifies you about changes

 A class which watches Directories and notifies you about every change in this Directory or in it's SubDirectories
 */

/*!
 Default Constructor using parent as parent
 */
Watcher::Watcher(QObject *parent)
    : QObject(parent)
    , m_watcher(new QFileSystemWatcher(this))
    , m_waitTimer(new QTimer(this))
{
    connect(m_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(recordChange(QString)));
    connect(m_waitTimer, SIGNAL(timeout()), this, SLOT(notifyChanges()));
    m_waitTimer->setInterval(100);
    m_waitTimer->setSingleShot(true);
}

/*!
 Set the watching Directory to path.

 Every change within this Directory and it's sub Directories will be reported by
 the directoriesChanged() signal
 */
void Watcher::setDirectory(const QString &path)
{
    m_rootDir = QDir(path);
    if (!m_watcher->directories().isEmpty()) {
        m_watcher->removePaths(m_watcher->directories());
    }
    if (!m_watcher->files().isEmpty()) {
        m_watcher->removePaths(m_watcher->files());
    }
    addDirectoriesRecursively(m_rootDir.absolutePath());
}

/*!
 Returns the Directory watched for changes
 */
QString Watcher::directory() const
{
    return m_rootDir.absolutePath();
}

/*!
 Add path and all it's SubDirectory to the internal used QFileSystemWatcher
 */
void Watcher::addDirectoriesRecursively(const QString &path)
{
//    qDebug() << "scan: " << path;
    if (!m_watcher->directories().contains(path)) {
        m_watcher->addPath(path);
    }
    QDirIterator iter(path, QDir::Dirs|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (iter.hasNext()) {
        QDir entry(iter.next());
        if (!m_watcher->directories().contains(entry.absolutePath())) {
            m_watcher->addPath(entry.absolutePath());

            //If we couldn't add it we reached the filesystem limit
            if (!m_watcher->directories().contains(entry.absolutePath())) {
                qWarning() << "Watcher limit reached. Please reduce the number of files you are watching !!!";
                return;
            }
        }
    }

}

/*!
  Returns the given path relative to the watcher's Directory
  /sa setDirectory, directory()
  */
QString Watcher::relativeFilePath(const QString &path)
{
    return m_rootDir.relativeFilePath(path);
}

void Watcher::recordChange(const QString &path)
{
//    qDebug() << "Watcher::recordChange: " << path;
    m_changes.append(path);
    m_waitTimer->start();
}

/*!
  Filters all the Directory changes.

  It tries to minimize the List of changes to the minimal common path.
  Example:

  Changes:

  /home/qmllive/test/images
  /home/qmllive/test
  /home/user

  Will be filtered to:

  /home/qmllive/test
  /home/user

  */
void Watcher::notifyChanges()
{
//    qDebug() << "changes" << m_changes;
    // sort to be able to avoid re-scan of recorded sub-folders
    QStringList final;
    QString top;
    // sort changes so top-most dirs stay first

    //TODO: wrong assumption, since "aaa<a-umlaut>/" > "aaa/very/deep/hierarchy"
    // we need to sort according to the number of sections when split by QDir::Separator

    m_changes.sort();
    foreach (const QString& entry, m_changes) {
        if (!QDir(entry).exists()) {
            // dir was removed
            if (m_watcher->directories().contains(entry)) {
                m_watcher->removePath(entry);
            }
        } else {
            if (top.isNull() || !entry.startsWith(top)) {
                top = entry;
                final.append(entry);
            }
        }
    }
    m_changes.clear();
    // need to rescan these top-most dirs
    foreach (const QString& entry, final) {
        addDirectoriesRecursively(entry);
    }
    emit directoriesChanged(final);
//    qDebug() << "watcher directories: " << m_watcher->directories();
}

