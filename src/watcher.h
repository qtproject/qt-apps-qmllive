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

#pragma once

#include <QtCore>

class Watcher : public QObject
{
    Q_OBJECT
public:
    enum Error {
        NoError,
        MaximumReached,
        SystemError,
    };

    explicit Watcher(QObject *parent = 0);
    void setDirectory(const QString& path);
    QString directory() const;
    bool hasError() const { return m_error != NoError; }
    Error error() const { return m_error; }
    static int maximumWatches() { return s_maximumWatches; }
    static void setMaximumWatches(int maximumWatches);
private Q_SLOTS:
    void recordChange(const QString &path);
    void notifyChanges();
Q_SIGNALS:
    void directoriesChanged(const QStringList& changes);
    void errorChanged();
private:
    void addDirectoriesRecursively(const QString& path);
    void addDirectory(const QString &path);
    void removeAllPaths();
    void setError(Error error);
    static int s_maximumWatches;
    QFileSystemWatcher *m_watcher;
    QDir m_rootDir;
    QTimer *m_waitTimer;
    QStringList m_changes;
    Error m_error = NoError;
};

