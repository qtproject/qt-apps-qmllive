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

#include "livedocument.h"
#include "resourcemap.h"

#include <QDebug>

/*!
 * \class LiveDocument
 * \brief Identifies a workspace document
 * \inmodule qmllive
 *
 * On the LiveHubEngine side, i.e. where the QmlLive Bench is running, it always
 * represents a document on file system, under the workspace directory.
 *
 * On the LiveNodeEngine side, i.e. on the QmlLive Runtime side, it represents a
 * document that may exist either on file system or in a Qt resource. In either
 * case the document is identified by the relative path of the original file
 * under the workspace directory on the LiveHubEngine side.  See
 * runtimeLocation().
 */

/*!
 * Constructs a null instance.
 *
 * \sa isNull(), errorString()
 */
LiveDocument::LiveDocument()
{
    m_errorString = tr("Internal error: A null LiveDocument passed");
}

/*!
 * Constructs instance for the given \a relativeFilePath.
 *
 * The \a relativeFilePath MUST NOT be an empty string, it MUST be a relative
 * path, and when resolved relatively to a directory it MUST NOT resolve to a
 * path outside of the directory.
 */
LiveDocument::LiveDocument(const QString &relativeFilePath)
{
    LIVE_ASSERT(!relativeFilePath.isEmpty(), return);
    LIVE_ASSERT(QDir::isRelativePath(relativeFilePath), return);
    LIVE_ASSERT(!QDir::cleanPath(relativeFilePath).startsWith(QLatin1String("../")), return);

    qInfo() << "LiveDocument::LiveDocument: relativeFilePath= " << relativeFilePath;
    m_relativeFilePath = relativeFilePath;
}

/*!
 * \fn bool LiveDocument::isNull() const
 *
 * Returns true if this is a null instance.
 *
 * A null instance has been either contructed with the default constructor or
 * the resolve() call failed.
 *
 * \sa errorString()
 */

/*!
 * \fn QString LiveDocument::errorString() const
 *
 * When called just after resolve(), existsIn(), isFileIn() or mapsToResource()
 * failed, returns a descriptive message suitable for displaying in user
 * interface. When called in other context, the result is undefined.
 */

/*!
 * Returns \c true if this document exists in the given \a workspace directory.
 *
 * \sa errorString()
 */
bool LiveDocument::existsIn(const QDir &workspace) const
{
    LIVE_ASSERT(!isNull(), return false);

    bool exists = QFileInfo(workspace, m_relativeFilePath).exists();
    if (!exists) {
        m_errorString = tr("Document '%1' does not exist in workspace '%2'")
            .arg(m_relativeFilePath)
            .arg(workspace.path());
    }
    return exists;
}

/*!
 * Returns \c true if this document exists as a regular file (or a symbolic link
 * to a regular file) in the given \a workspace directory.
 *
 * Symbolic links resolution applies.
 *
 * \sa errorString()
 */
bool LiveDocument::isFileIn(const QDir &workspace) const
{
    LIVE_ASSERT(!isNull(), return false);

    if (!existsIn(workspace))
        return false;

    bool isFile = QFileInfo(workspace, m_relativeFilePath).isFile();
    if (!isFile) {
        m_errorString = tr("Document '%1' is a non-regular file in workspace '%2'")
            .arg(m_relativeFilePath)
            .arg(workspace.path());
    }
    return isFile;
}

/*!
 * Returns the relative file path including the file name.
 */
QString LiveDocument::relativeFilePath() const
{
    LIVE_ASSERT(!isNull(), return QString());

    return m_relativeFilePath;
}

/*!
 * Returns the absolute file path within a \a workspace, including the file name.
 */
QString LiveDocument::absoluteFilePathIn(const QDir &workspace) const
{
    LIVE_ASSERT(!isNull(), return QString());

    return QDir::cleanPath(workspace.absoluteFilePath(m_relativeFilePath));
}

/*!
 * Returns \c true if the runtime location of the document is in a Qt resource.
 * This is determined using the given \a resourceMap.
 *
 * \sa runtimeLocation(), LiveNodeEngine::resourceMap(), errorString()
 */
bool LiveDocument::mapsToResource(const ResourceMap &resourceMap) const
{
    LIVE_ASSERT(!isNull(), return false);

    QString resource = resourceMap.toResource(*this);
    if (resource.isEmpty()) {
        m_errorString = tr("Document '%1' does not exist as a Qt resource")
            .arg(m_relativeFilePath);
        return false;
    }

    // TODO Necessary to check if it actually exists? ResourceMap may not be in sync.

    return true;
}

/*!
 * Determines the runtime location of the document. On LiveNodeEngine side a
 * document may exist either on file system (under the given \a workspace) or in
 * a Qt resource (determined using the given \a resourceMap).
 *
 * Example:
 *
 * \code
 * LiveDocument document = ...;
 * LiveNodeEngine *engine = ...;
 * QFile file(document.runtimeLocation(engine->workspace(), *engine->resourceMap()));
 * \endcode
 *
 * \sa mapsToResource(), LiveNodeEngine::resourceMap()
 */
QUrl LiveDocument::runtimeLocation(const QDir &workspace, const ResourceMap &resourceMap) const
{
    LIVE_ASSERT(!isNull(), return QString());

    const QString resource = resourceMap.toResource(*this);
    if (!resource.isEmpty())
        return toUrl(resource);
    else
        return QUrl::fromLocalFile(absoluteFilePathIn(workspace));
}

/*!
 * Constructs a non-null instance unless the \a filePath resolves outside of the
 * \a workspace directory.
 *
 * \a filePath may be an absolute or relative file path to a file which is NOT
 * required to exist.
 *
 * \sa isNull(), errorString()
 */
LiveDocument LiveDocument::resolve(const QDir &workspace, const QString &filePath)
{
    LiveDocument retv;

    if (filePath.isEmpty()) {
        qWarning() << "filePath is an empty string";
        retv.m_errorString = tr("Not a valid file path: ''");
        return retv;
    }

    QString relativeFilePath = workspace.relativeFilePath(filePath);
    QString cleanPath = QDir::cleanPath(relativeFilePath);
    if (cleanPath.isEmpty()) {
        retv.m_relativeFilePath = QStringLiteral(".");
    } else if (!cleanPath.startsWith(QLatin1String("../"))) {
        retv.m_relativeFilePath = relativeFilePath;
    } else {
        retv.m_errorString = tr("Document path '%1' is outside the workspace directory '%2'")
            .arg(filePath).arg(workspace.path());
    }

    return retv;
}

/*!
 * Constructs a non-null instance for \a filePath that either resolves to a path
 * under the \a workspace directory or determines a Qt resource for which a
 * mapping exists in the given \a resourceMap.
 *
 * \a filePath may be an absolute or relative file path to a file or a Qt
 * resource path (starting with \c {":/"} ). In either case the file is NOT
 * required to exist,
 *
 * \sa isNull(), errorString()
 */
LiveDocument LiveDocument::resolve(const QDir &workspace, const ResourceMap &resourceMap, const QString &filePath)
{
    if (filePath.isEmpty()) {
        qWarning() << "filePath is an empty string";
        LiveDocument retv;
        retv.m_errorString = tr("Not a valid file path: ''");
        return retv;
    }

    if (filePath.startsWith(":/")) {
        LiveDocument retv = resourceMap.toDocument(filePath);
        if (retv.isNull()) {
            retv.m_errorString = tr("No mapping exists for resource: '%1'").arg(filePath);
        }
        return retv;
    } else {
        return resolve(workspace, filePath);
    }
}

/*!
 * Constructs a non-null instance for \a fileUrl that either resolves to a path
 * under the \a workspace directory or determines a Qt resource for which a
 * mapping exists in the given \a resourceMap.
 *
 * \a fileUrl may be an URL representing a local file or a Qt resource path
 * (e.g. \c {"qrc:/example/icon.png"}). In either case the file is NOT required
 * to exist,
 *
 * \sa isNull(), errorString()
 */
LiveDocument LiveDocument::resolve(const QDir &workspace, const ResourceMap &resourceMap, const QUrl &fileUrl)
{
    if (fileUrl.isEmpty()) {
        qWarning() << "fileUrl is an empty URL:" << fileUrl;
        LiveDocument retv;
        retv.m_errorString = tr("Not a valid URL: '%1'").arg(fileUrl.toString());
        return retv;
    }

    if (fileUrl.scheme() == QLatin1String("qrc")) {
        LiveDocument retv = resourceMap.toDocument(QLatin1String(":") + fileUrl.path());
        if (retv.isNull()) {
            retv.m_errorString = tr("No mapping exists for resource: '%1'").arg(fileUrl.toString());
        }
        return retv;
    } else if (fileUrl.scheme() == QLatin1String("file")) {
        return resolve(workspace, fileUrl.toLocalFile());
    } else {
        qWarning() << "fileUrl is not a supported URL:" << fileUrl;
        LiveDocument retv;
        retv.m_errorString = tr("Unsupported URL: '%1'").arg(fileUrl.toString());
        return retv;
    }
}

/*!
 * Converts a local/Qt resource \a url to file path
 */
QString LiveDocument::toFilePath(const QUrl &url)
{
    if (url.scheme() == QLatin1String("qrc"))
        return QLatin1String(":") + url.path();
    else
        return url.toLocalFile();
}

/*!
 * Converts a local/Qt resource \a filePath to URL
 */
QUrl LiveDocument::toUrl(const QString &filePath)
{
    if (filePath.startsWith(":/")) {
        QUrl url;
        url.setScheme("qrc");
        url.setPath(filePath.mid(1));
        return url;
    } else {
        return QUrl::fromLocalFile(filePath);
    }
}

/*!
 * Allows to print LiveDocument \a document via debug stream \a dbg.
 */
QDebug operator<<(QDebug dbg, const LiveDocument &document)
{
    if (document.isNull())
        dbg << QStringLiteral("<null>");
    else
        dbg << document.relativeFilePath();

    return dbg;
}
