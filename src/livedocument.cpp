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

#include "livedocument.h"

#include <QDebug>

/*!
 * \class LiveDocument
 * \brief Encapsulates a relative path to a workspace document.
 * \inmodule qmllive
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
 * When called just after resolve(), existsIn() or isFileIn() failed, returns a
 * descriptive message suitable for displaying in user interface. When called in
 * other context, the result is undefined.
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
