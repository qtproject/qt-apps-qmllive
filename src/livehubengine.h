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

#include "livedocument.h"
#include "qmllive_global.h"

class Watcher;
class ContentPluginFactory;

class QMLLIVESHARED_EXPORT LiveHubEngine : public QObject
{
    Q_OBJECT
public:
    enum Error {
        NoError,
        WatcherMaximumReached,
        WatcherSystemError,
    };

    explicit LiveHubEngine(QObject *parent = 0);
    void setWorkspace(const QString& path);
    QString workspace() const;

    LiveDocument activePath() const;

    bool hasError();
    Error error();

    static int maximumWatches();
    static void setMaximumWatches(int maximumWatches);
public Q_SLOTS:
    void setActivePath(const LiveDocument& path);
    void setFilePublishingActive(bool on);
    void publishWorkspace();
Q_SIGNALS:
    void beginPublishWorkspace();
    void endPublishWorkspace();
    void publishFile(const LiveDocument& document);
    void fileChanged(const LiveDocument& document);
    void activateDocument(const LiveDocument& document);
    void workspaceChanged(const QString& workspace);
    void errorChanged();
private Q_SLOTS:
    void directoriesChanged(const QStringList& changes);
    void watcherErrorChanged();
private:
    void publishDirectory(const QString& dirPath, bool fileChange);
private:
    Watcher *m_watcher;
    bool m_filePublishingActive;
    LiveDocument m_activePath;
    Error m_error = NoError;
};

