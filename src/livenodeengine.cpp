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

#include "livenodeengine.h"
#include "liveruntime.h"
#include "qmlhelper.h"
#include "contentpluginfactory.h"
#include "imageadapter.h"
#include "fontadapter.h"

#include "QtQml/qqml.h"
#include "QtQuick/private/qquickpixmapcache_p.h"

// TODO: create proxy configuration settings, controlled by command line and ui

#ifdef QMLLIVE_DEBUG
#define DEBUG qDebug()
#else
#define DEBUG if (0) qDebug()
#endif

/*!
 * \class LiveNodeEngine
 * \brief The LiveNodeEngine class instruments a qml viewer in cooperation with LiveHubEngine.
 * \inmodule qmllive
 *
 * LiveNodeEngine provides ways to reload qml documents based incoming requests
 * from a hub. A hub can be connected via a local mode ( LocalPublisher) or
 * remote mode (RemotePublisher with a RemoteReceiver).
 *
 * In Addition to showing qml-Files the LiveNodeEngine can be extended by plugins to show any other datatype.
 * One need to set the Plugin path to the right destination and the LiveNodeEngine will load all the plugins
 * it finds there.
 *
 * \sa {ContentPlugin Example}
 */

/*!
    \enum LiveNodeEngine::UpdateMode

    This enum type specifies the update behavior for the view:

    \value ReloadDocument
           Clears the component cache and reloads the active document
    \value RecreateView
           Destroys the view and re-creates the view with the active document
    \value RecreateProcess
           Restarts the process with the active document
*/

/*!
 * Standard constructor using \a parent as parent
 */
LiveNodeEngine::LiveNodeEngine(QObject *parent)
    : QObject(parent)
    , m_runtime(new LiveRuntime(this))
    , m_xOffset(0)
    , m_yOffset(0)
    , m_rotation(0)
    , m_view(0)
    , m_recreateView(0)
    , m_delayReload(new QTimer(this))
    , m_mode(ReloadDocument)
    , m_pluginFactory(new ContentPluginFactory(this))
    , m_activePlugin(0)
    , m_reloadPlugins(false)
{
    m_delayReload->setInterval(250);
    m_delayReload->setSingleShot(true);
    connect(m_delayReload, SIGNAL(timeout()), this, SLOT(reloadDocument()));
}

/*!
 * Sets the view which should be used to \a view
 */
    void LiveNodeEngine::setView(QQuickView *view)
{
    m_view = view;
    m_view->rootContext()->setContextProperty("livert", m_runtime);
    m_view->engine()->setOutputWarningsToStandardError(false); // log
    if (!m_importPaths.isEmpty())
        m_view->engine()->setImportPathList(m_importPaths);

    connect(m_view->engine(), SIGNAL(warnings(QList<QQmlError>)),
            this, SIGNAL(logErrors(QList<QQmlError>)));
    connect(m_view, SIGNAL(statusChanged(QQuickView::Status)),
            this, SLOT(onStatusChanged(QQuickView::Status)));
    connect(m_view, SIGNAL(widthChanged(int)),
            this, SLOT(onSizeChanged()));
    connect(m_view, SIGNAL(heightChanged(int)),
            this, SLOT(onSizeChanged()));
}

/*!
 * Sets the x-offset \a offset of window
 */
void LiveNodeEngine::setXOffset(int offset)
{
    QQuickView *view = 0;

    if (m_view)
        view = m_view;
    if (m_recreateView)
        view = m_recreateView;
    if (view)
        view->contentItem()->setX(offset);

    m_xOffset = offset;
}

/*!
 * Returns the current x-offset of the window
 */
int LiveNodeEngine::xOffset() const
{
    return m_xOffset;
}

/*!
 * Sets the y-offset \a offset of window
 */
void LiveNodeEngine::setYOffset(int offset)
{
    QQuickView *view = 0;

    if (m_view)
        view = m_view;
    if (m_recreateView)
        view = m_recreateView;
    if (view)
        view->contentItem()->setY(offset);

    m_yOffset = offset;
}

/*!
 * Returns the current y-offset of the window
 */
int LiveNodeEngine::yOffset() const
{
    return m_yOffset;
}

/*!
 * Sets the rotation \a rotation of window around the center
 */

void LiveNodeEngine::setRotation(int rotation)
{
    QQuickView *view = 0;

    if (m_view)
        view = m_view;
    if (m_recreateView)
        view = m_recreateView;
    if (view) {
        view->contentItem()->setRotation(0);
        view->contentItem()->setTransformOriginPoint(QPointF(view->width()/2, view->height()/2));
        view->contentItem()->setRotation(rotation);
    }

    m_rotation = rotation;
}

/*!
 * Return the current rotation angle
 */
int LiveNodeEngine::rotation() const
{
    return m_rotation;
}

/*!
 * Sets the update mode for the scene to \a mode.
 *
 * Either to reload just the document, or
 * the view, or the whole process.
 */
void LiveNodeEngine::setUpdateMode(LiveNodeEngine::UpdateMode mode)
{
    m_mode = mode;
}

 /*!
  * Returns the current update mode for the scene
  */
LiveNodeEngine::UpdateMode LiveNodeEngine::updateMode() const
{
    return m_mode;
}

/*!
 * Loads the given \a url onto the qml view. Clears any caches and reloads
 * the dummy data initially.
 */
void LiveNodeEngine::loadDocument(const QUrl& url)
{
    DEBUG << "LiveNodeEngine::loadDocument: " << url;
    m_activeFile = url;

    if (!m_activeFile.isEmpty())
        reloadDocument();
}

/*!
 * Starts a timer to reload the view with a delay.
 *
 * A delay reload is important to avoid constant reloads, while many changes
 * appear.
 */
void LiveNodeEngine::delayReload()
{
    m_delayReload->start();
}

/*!
 * Sets the view which should be used to \a view
 */
void LiveNodeEngine::recreateView()
{
    if (m_windowComponent)
        delete m_windowComponent;
    if (m_windowObject)
        delete m_windowObject;

    if (m_recreateView) {
        //m_recreateView->setSource(QUrl());
        m_recreateView->engine()->clearComponentCache();
        delete m_recreateView;
        if (m_reloadPlugins)
            qmlClearTypeRegistrations();
        QQuickPixmap::purgeCache();
    }

    m_recreateView = initView();

    setXOffset(m_xOffset);
    setYOffset(m_yOffset);

    m_recreateView->rootContext()->setContextProperty("livert", m_runtime);
    m_recreateView->engine()->setOutputWarningsToStandardError(false); // log
    if (!m_importPaths.isEmpty())
        m_recreateView->engine()->setImportPathList(m_importPaths);

    m_recreateView->engine()->clearComponentCache();
    QmlHelper::loadDummyData(m_recreateView, m_workspace.absolutePath());

    connect(m_recreateView->engine(), SIGNAL(warnings(QList<QQmlError>)),
            this, SIGNAL(logErrors(QList<QQmlError>)));
    connect(m_recreateView, SIGNAL(statusChanged(QQuickView::Status)),
            this, SLOT(onStatusChanged(QQuickView::Status)));
    connect(m_recreateView, SIGNAL(widthChanged(int)),
            this, SLOT(onSizeChanged()));
    connect(m_recreateView, SIGNAL(heightChanged(int)),
            this, SLOT(onSizeChanged()));
}

/*!
 * Checks if the QtQuick Controls module exists for the content adapters
 */
void LiveNodeEngine::checkQmlFeatures(QQuickView *view)
{
    foreach (QString importPath, view->engine()->importPathList()) {
        QDir dir(importPath);
        if (dir.exists("QtQuick/Controls") &&
            dir.exists("QtQuick/Layouts") &&
            dir.exists("QtQuick/Dialogs")) {
            m_quickFeatures |= ContentAdapterInterface::QtQuickControls;
        }
    }
}

/*!
 * Reloads the qml view source.
 */
void LiveNodeEngine::reloadDocument()
{
    QQuickView *view = 0;

    if (m_mode == RecreateView) {
        recreateView();

        view = m_recreateView;
    } else {
        view = m_view;
        view->setSource(QUrl());
        view->engine()->trimComponentCache();
        view->releaseResources();
    }

    checkQmlFeatures(view);

    emit logClear();
    //emit logIgnoreMessages(true);

    QUrl url = queryDocumentViewer(m_activeFile);
    QQmlEngine *engine = view->engine();
    m_windowComponent = new QQmlComponent(engine);

    m_windowComponent->loadUrl(url);

    m_windowObject = m_windowComponent->create();
    QQuickWindow *window = qobject_cast<QQuickWindow *>(m_windowObject);
    if (window) {
        engine->setIncubationController(window->incubationController());
        window->show();
    } else {
        view->setContent(url, m_windowComponent, m_windowObject);
    }

    //emit logIgnoreMessages(false);

    QList<QQmlError> errors = view->errors();
    if (!errors.isEmpty()) {
        emit logErrors(errors);
        if (m_quickFeatures.testFlag(ContentAdapterInterface::QtQuickControls))
            view->setSource(QUrl("qrc:/livert/error_qt5_controls.qml"));
        view->setSource(QUrl("qrc:/livert/error_qt5.qml"));
    }

    if (m_mode == RecreateView)
        view->setVisible(true);

    if (m_activePlugin || (view->rootObject() && QSize(view->rootObject()->width(), view->rootObject()->height()).isEmpty())) {
        view->setResizeMode(QQuickView::SizeRootObjectToView);
    } else {
        view->setResizeMode(QQuickView::SizeViewToRootObject);
    }
}


/*!
 * Allows to adapt a \a url to display not native qml documents (e.g. images).
 */
QUrl LiveNodeEngine::queryDocumentViewer(const QUrl& url)
{
    initPlugins();

    foreach (ContentAdapterInterface *adapter, m_plugins) {
        if (adapter->canAdapt(url)) {
            adapter->cleanUp();
            adapter->setAvailableFeatures(m_quickFeatures);

            m_activePlugin = adapter;

            if (m_mode == RecreateView)
                return adapter->adapt(url, m_recreateView->rootContext());
            else
                return adapter->adapt(url, m_view->rootContext());
        }
    }

    m_activePlugin = 0;

    return url;
}

/*!
 * Sets the document \a document to be shown
 *
 */
void LiveNodeEngine::setActiveDocument(const QString &document)
{
    QUrl url;
    if (!document.isEmpty()) {
        url = QUrl::fromLocalFile(m_workspace.absoluteFilePath(document));
    }

    loadDocument(url);
    emit activateDocument(document);
}

/*!
 * Sets the current workspace to \a path. Documents location will be adjusted based on
 * this workspace path.
 */
void LiveNodeEngine::setWorkspace(const QString &path)
{
    m_workspace = QDir(path);
}

/*!
 * Sets the import paths \a paths on the scene
 */
void LiveNodeEngine::setImportPaths(const QStringList &paths)
{
    m_importPaths = paths;

    if (m_view)
        m_view->engine()->setImportPathList(paths);
}

/*!
 * Returns the current import paths of the scene
 */
QStringList LiveNodeEngine::importPaths() const
{
    return m_importPaths;
}

/*!
 * Sets the pluginPath to \a path.
 *
 * The pluginPath will be used to load QmlLive plugins
 * \sa {ContentPlugin Example}
 */
void LiveNodeEngine::setPluginPath(const QString &path)
{
    m_pluginFactory->setPluginPath(path);
}

/*!
 * Returns the current pluginPath
 */
QString LiveNodeEngine::pluginPath() const
{
    return m_pluginFactory->pluginPath();
}

/*!
 * Returns the current active document url.
 */
QUrl LiveNodeEngine::activeDocument() const
{
    return m_activeFile;
}

/*!
 * Returns the active content adapter plugin
 */
ContentAdapterInterface *LiveNodeEngine::activePlugin() const
{
    return m_activePlugin;
}

/*!
 * Enables the reloading of native plugins when \a enabled set to true. Use
 * with care as native plugins tend to crash on reload.
 */
void LiveNodeEngine::setReloadPluginsEnabled(bool enabled)
{
    m_reloadPlugins = enabled;
}

/*!
 * Returns true when the active plugins reloading is enabled, otherwise false
 */
bool LiveNodeEngine::isReloadPluginsEnabled() const
{
    return m_reloadPlugins;
}

/*!
 * Loads all plugins found in the Pluginpath
 */
void LiveNodeEngine::initPlugins()
{
    if (m_plugins.isEmpty()) {
        m_pluginFactory->load();
        m_plugins.append(m_pluginFactory->plugins());
        m_plugins.append(new ImageAdapter(this));
        m_plugins.append(new FontAdapter(this));
    }
}

/*!
 * Handle \a status changes and emit the document loaded signal.
 */
void LiveNodeEngine::onStatusChanged(QQuickView::Status status)
{
    if (status == QQuickView::Ready ||
        status == QQuickView::Error) {
        emit documentLoaded();
    }
}

/*!
 * Initializes a new view
 */
QQuickView *LiveNodeEngine::initView()
{
    QQuickView *view = new QQuickView();
    emit viewChanged(view);
    return view;
}

/*!
 * Handles size changes and updates the view according
 */
void LiveNodeEngine::onSizeChanged()
{
    int width = -1, height = -1;

    if (m_mode == RecreateView && m_recreateView) {
        width = m_recreateView->width();
        height = m_recreateView->height();
    } else if (m_mode == ReloadDocument && m_view) {
        width = m_view->width();
        height = m_view->height();
    }

    if (width != -1 && height != -1) {
        m_runtime->setScreenWidth(width);
        m_runtime->setScreenHeight(height);
    }

    setRotation(m_rotation);
}

/*!
 * \fn void LiveNodeEngine::activateDocument(const QString& document)
 *
 * The document \a document was activated
 */

/*!
 * \fn void LiveNodeEngine::logClear()
 *
 * Requested to clear the log
 */

/*!
 * \fn void LiveNodeEngine::logIgnoreMessages(bool)
 *
 * Requsted to ignore the Messages when \a on is true
 */

/*!
 * \fn void LiveNodeEngine::documentLoaded()
 *
 * The signal is emitted when the document has finished loading.
 */

/*!
 * \fn void LiveNodeEngine::viewChanged(QQuickView *view)
 *
 * The signal is emitted when the view pointer \a view has changed (e.g by initializing a new view)
 */

/*!
 * \fn void LiveNodeEngine::logErrors(const QList<QQmlError> &errors)
 *
 * Log the Errors \a errors
 */
