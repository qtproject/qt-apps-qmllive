/****************************************************************************
**
** Copyright (C) 2018 Jolla Ltd
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

#include "resourcemap.h"

#include "livedocument.h"

Q_DECLARE_LOGGING_CATEGORY(rmLog)
Q_LOGGING_CATEGORY(rmLog, "QmlLive.ResourceMap", QtInfoMsg)

/*!
 * \class ResourceMap
 * \brief Maps compiled-in Qt resources to workspace documents and vice versa
 * \inmodule qmllive
 *
 * A ResourceMap is managed and owned by a LiveNodeEngine. LiveDocument uses
 * ResourceMap to enable convenient handling of documents that may reside
 * either under workspace on file system or in Qt resources at run time.
 *
 * \sa LiveNodeEngine::resourceMap(), LiveDocument
 */

class QrcReader : public QObject
{
    Q_OBJECT

public:
    explicit QrcReader(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    bool read(QIODevice *input, QString *errorString)
    {
        Q_ASSERT(errorString);

        m_xml.setDevice(input);

        if (m_xml.readNextStartElement()) {
            if (m_xml.name() == QLatin1String("RCC")
                    && (!m_xml.attributes().hasAttribute(QLatin1String("version"))
                        || m_xml.attributes().value(QLatin1String("version")) == QLatin1String("1.0"))) {
                readRCC();
            } else {
                m_xml.raiseError(QObject::tr("The file is not an RCC (version 1.0) file."));
            }
        }

        if (m_xml.error())
            *errorString = m_xml.errorString();

        return !m_xml.error();
    }

signals:
    void fileRead(const QString &resource, const QString &file,
            QLocale::Language language, QLocale::Country country);

private:
    void readRCC()
    {
        Q_ASSERT(m_xml.isStartElement() && m_xml.name() == QLatin1String("RCC"));

        while (m_xml.readNextStartElement()) {
            if (m_xml.name() == QLatin1String("qresource"))
                readQresource();
            else
                raiseUnexpectedTagError();
        }
    }

    void readQresource()
    {
        Q_ASSERT(m_xml.isStartElement() && m_xml.name() == QLatin1String("qresource"));

        m_prefix = m_xml.attributes().value(QLatin1String("prefix")).toString();

        m_language = QLocale::c().language();
        m_country = QLocale::c().country();
        QString lang = m_xml.attributes().value(QLatin1String("lang")).toString();
        if (!lang.isEmpty()) {
            QLocale locale = QLocale(lang);
            m_language = locale.language();
            m_country = lang.length() > 2 ? locale.country() : QLocale::AnyCountry;
        }

        while (m_xml.readNextStartElement()) {
            if (m_xml.name() == QLatin1String("file"))
                readFile();
            else
                raiseUnexpectedTagError();
        }
    }

    void readFile()
    {
        Q_ASSERT(m_xml.isStartElement() && m_xml.name() == QLatin1String("file"));

        QString resource = m_xml.attributes().value(QLatin1String("alias")).toString();
        QString file = m_xml.readElementText();

        if (resource.isEmpty())
            resource = file;
        if (!m_prefix.isEmpty())
            resource = m_prefix + QLatin1Char('/') + resource;
        if (resource.at(0) != QLatin1Char('/'))
            resource.prepend(QLatin1String(":/"));
        else
            resource.prepend(QLatin1Char(':'));

        emit fileRead(resource, file, m_language, m_country);
    }

    void raiseUnexpectedTagError()
    {
        m_xml.raiseError(QStringLiteral("Unexpected tag %1").arg(m_xml.name().toString()));
    }

private:
    QXmlStreamReader m_xml;
    QString m_prefix;
    QLocale::Language m_language{QLocale::AnyLanguage};
    QLocale::Country m_country{QLocale::AnyCountry};
};

/*!
 * Constructs with the given \a parent object.
 */
ResourceMap::ResourceMap(QObject *parent)
    : QObject(parent)
{
}

/*!
 * Returns the resource that maps to the given \a document or an empty QString
 * if no corresponding mapping exists.
 *
 * This is a low level API. LiveDocument provides more convenient API.
 */
QString ResourceMap::toResource(const LiveDocument &document) const
{
    LIVE_ASSERT(!document.isNull(), return QString());

    QReadLocker locker(&m_lock);
    auto localizedIt = m_resourcesByDocument.find(systemLocalePrefix() + document.relativeFilePath());
    if (localizedIt != m_resourcesByDocument.end())
        return *localizedIt;
    auto nonlocalizedIt = m_resourcesByDocument.find(cLocalePrefix() + document.relativeFilePath());
    if (nonlocalizedIt != m_resourcesByDocument.end())
        return *nonlocalizedIt;
    return QString();
}

/*!
 * Returns the document that maps to the given \a resource or a null
 * LiveDocument if no corresponding mapping exists.
 *
 * This is a low level API. LiveDocument provides more convenient API.
 */
LiveDocument ResourceMap::toDocument(const QString &resource) const
{
    LIVE_ASSERT(!resource.isEmpty(), return LiveDocument());

    QReadLocker locker(&m_lock);
    auto localizedIt = m_documentByResource.find(systemLocalePrefix() + resource);
    if (localizedIt != m_documentByResource.end())
        return LiveDocument(*localizedIt);
    auto nonlocalizedIt = m_documentByResource.find(cLocalePrefix() + resource);
    if (nonlocalizedIt != m_documentByResource.end())
        return LiveDocument(*nonlocalizedIt);
    return LiveDocument();
}

/*!
 * Returns an user readable description of the last error occurred.
 *
 * The error string may be set by the updateMapping() call.
 */
QString ResourceMap::errorString() const
{
    return m_errorString;
}

/*!
 * Updates mapping from the given \c .qrc document
 *
 * Old mappings for the given \a qrcDocument will be removed, then new mappings
 * will be added based on the content of the \a qrcFile.
 *
 * Returns \c true on success. Otherwise errorString() is set and \c false
 * returned.
 */
bool ResourceMap::updateMapping(const LiveDocument &qrcDocument, QIODevice *qrcFile)
{
    LIVE_ASSERT(!qrcDocument.isNull(), return false);
    LIVE_ASSERT(qrcFile, return false);
    LIVE_ASSERT(qrcFile->openMode() & QIODevice::ReadOnly, return false);

    qCDebug(rmLog) << "Updating resource map for" << qrcDocument;

    const QString qrcPath = QFileInfo(qrcDocument.relativeFilePath()).path();

    QWriteLocker locker(&m_lock);

    removeMapping(qrcDocument);

    auto addMapping = [&](const QString &resource, const QString &file,
            QLocale::Language language, QLocale::Country country) {
        QString localePrefix = toLocalePrefix(language, country);
        QString prefixedResourceName = localePrefix + resource;
        QString filePath = QDir::cleanPath(QFileInfo(qrcPath, file).filePath());

        // The bench sends all .qrc files found in the workspace, not just those actually built-in
        if (!QFileInfo(resource).exists()) {
            qCDebug(rmLog) << "Not mapping" << prefixedResourceName << "to" << filePath << "(resource does not exist)";
            return;
        }
        qCDebug(rmLog) << "Mapping" << prefixedResourceName << "to" << filePath;

        m_resourcesByDocument.insert(localePrefix + filePath, resource);
        m_documentByResource.insert(prefixedResourceName, filePath);
        m_resourcesByQrc.insert(qrcDocument.relativeFilePath(), prefixedResourceName);
    };

    QrcReader reader;
    connect(&reader, &QrcReader::fileRead, addMapping);

    bool ok = reader.read(qrcFile, &m_errorString);
    if (!ok)
        removeMapping(qrcDocument);

    return ok;
}

void ResourceMap::removeMapping(const LiveDocument &qrcDocument)
{
    LIVE_ASSERT(!qrcDocument.isNull(), return);

    auto it = m_resourcesByQrc.find(qrcDocument.relativeFilePath());
    while (it != m_resourcesByQrc.end() && it.key() == qrcDocument.relativeFilePath()) {
        QString localePrefix = it->left(it->indexOf(QLatin1Char('/')));
        QString resource = it->mid(localePrefix.length() + 1);
        QString file = m_documentByResource.take(*it);
        m_resourcesByDocument.remove(localePrefix + file, resource);
        it = m_resourcesByQrc.erase(it);
    }
}

QString ResourceMap::toLocalePrefix(QLocale::Language language, QLocale::Country country)
{
    return QLocale(language, country).name() + QLatin1Char('/');
}

QString ResourceMap::cLocalePrefix()
{
    return QLocale::c().name() + QLatin1Char('/');
}

QString ResourceMap::systemLocalePrefix()
{
    return QLocale::system().name() + QLatin1Char('/');
}

#include "resourcemap.moc"
