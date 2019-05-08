/****************************************************************************
**
** Copyright (C) 2019 Luxoft Sweden AB
** Copyright (C) 2018 Pelagicore AG
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QML Live tool.
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

#include "watcher.h"


/*!
 \class Watcher
 \internal
 \brief A class which watches Directories and notifies you about changes

 A class which watches Directories and notifies you about every change in this Directory or in it's SubDirectories
 */

/*!
 \enum Watcher::Error
 \brief Describes error state of a Watcher

 \value NoError
        No error
 \value MaximumReached
        The maximum number of watches set with setMaximumWatches() was exceeded
 \value SystemError
        QFileSystemWatcher::addPath failed for an unspecified reason
 */

int Watcher::s_maximumWatches = -1;

/*!
 Default Constructor using parent as parent
 */
Watcher::Watcher(QObject *parent)
    : QObject(parent)
    , m_watcher(new QFileSystemWatcher(this))
    , m_waitTimer(new QTimer(this))
{
    connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &Watcher::recordChange);
    connect(m_waitTimer, &QTimer::timeout, this, &Watcher::notifyChanges);
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
    removeAllPaths();
    setError(NoError);
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
 \fn Watcher::maximumWatches()

 Returns the maximum number of watched directories
 */

/*!
 Sets the maximum number of watched directories

 This will only take effect with next setDirectory() call.
 */
void Watcher::setMaximumWatches(int maximumWatches)
{
    s_maximumWatches = maximumWatches;
}

/*!
 \fn Watcher::hasError() const

 Returns true if error() is not NoError
 */

/*!
 \fn Watcher::error() const

 Describes the current error state of this watcher
 */

/*!
 \fn Watcher::errorChanged()

 Notifies about error() change
 */

/*!
 Add path and all it's SubDirectory to the internal used QFileSystemWatcher
 */
void Watcher::addDirectoriesRecursively(const QString &path)
{
//    qDebug() << "scan: " << path;
    addDirectory(path);
    QDirIterator iter(path, QDir::Dirs|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (iter.hasNext() && !hasError()) {
        QDir entry(iter.next());
        addDirectory(entry.absolutePath());
    }
}

void Watcher::addDirectory(const QString &path)
{
    if (m_watcher->directories().contains(path))
        return;

    if (s_maximumWatches > 0 &&
            m_watcher->directories().count() + m_watcher->files().count() > s_maximumWatches) {
        removeAllPaths();
        setError(MaximumReached);
        return;
    }

    m_watcher->addPath(path);

    if (!m_watcher->directories().contains(path)) {
        removeAllPaths();
        setError(SystemError);
    }
}

void Watcher::removeAllPaths()
{
    if (!m_watcher->directories().isEmpty()) {
        m_watcher->removePaths(m_watcher->directories());
    }
    if (!m_watcher->files().isEmpty()) {
        m_watcher->removePaths(m_watcher->files());
    }
}

void Watcher::setError(Watcher::Error error)
{
    if (m_error == error)
        return;

    m_error = error;
    emit errorChanged();
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

