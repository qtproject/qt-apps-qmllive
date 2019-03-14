/****************************************************************************
**
** Copyright (C) 2019 Luxoft Sweden AB
** Copyright (C) 2018 Pelagicore AG
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

#include "projectmanager.h"

#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

const QLatin1String MainKey("main");
const QLatin1String WorkspaceKey("workspace");
const QLatin1String ImportsKey("imports");
const QLatin1String QmlLiveExtension(".qmllive");

ProjectManager::ProjectManager(QObject *parent)
    : QObject(parent)
    , m_mainDocument("main.qml")
    , m_workspace("")
    , m_projectName("")
    , m_projectLocation("")
{

}

bool ProjectManager::read(const QString &path)
{
    reset();

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to open document " << path;
        return false;
    }
    m_projectLocation = QFileInfo(path).absolutePath();
    m_projectName = file.fileName();
    QByteArray data = file.readAll();
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(data, &error);
    if (error.error) {
        qWarning() << "error parsing JSON document" << error.errorString();
        return false;
    }
    if (!document.isObject()) {
        qWarning() << "Document must be a JSON object";
        return false;
    }
    QJsonObject root = document.object();
    if (root.contains(MainKey))
        m_mainDocument = root.value(MainKey).toString();
    if (root.contains(WorkspaceKey))
    {
        m_workspace = root.value(WorkspaceKey).toString();
    }
    if (root.contains(ImportsKey) && root.value(ImportsKey).isArray()) {
        QJsonArray imports = root.value(ImportsKey).toArray();
        for (QJsonValue value : imports)
            m_imports.append(value.toString());
    }
    return true;
}

void ProjectManager::write(const QString &path)
{
    qInfo()<<"Writing into " << path;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Unable to write to document: " << path;
        return;
    }
    m_projectLocation = QFileInfo(path).absolutePath();
    QJsonObject root;
    root.insert(MainKey, QJsonValue(m_mainDocument));
    root.insert(WorkspaceKey, QJsonValue(m_workspace));
    QJsonArray imports;
    for (const QString &import : m_imports)
        imports.append(QJsonValue(import));
    root.insert(ImportsKey, imports);
    QJsonDocument document(root);
    file.write(document.toJson());
}

void ProjectManager::create(const QString &projectName)
{
    m_projectName = projectName;
    QString path = QString(projectName).append(QmlLiveExtension);
    qInfo()<<"CREATING Project------ "<<path;
    write(path);
}

QString ProjectManager::mainDocument() const
{
    return m_mainDocument;
}

QString ProjectManager::workspace() const
{
    return m_workspace;
}

QStringList ProjectManager::imports() const
{
    return m_imports;
}

QString ProjectManager::projectLocation() const
{
    return m_projectLocation;
}

void ProjectManager::reset()
{
    m_mainDocument = QString("main.qml");
    m_workspace = QString("");
    m_imports.clear();
}

void ProjectManager::setProjectName(const QString &projectName)
{
    m_projectName = projectName;
}
void ProjectManager::setMainDocument(const QString &mainDocument)
{
    m_mainDocument = mainDocument;
}
void ProjectManager::setWorkspace(const QString &workspace)
{
    m_workspace = workspace;
}
void ProjectManager::setImports(const QStringList &imports)
{
    m_imports = imports;
}

