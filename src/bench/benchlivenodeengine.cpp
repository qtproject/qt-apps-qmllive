/****************************************************************************
**
** Copyright (C) 2016 Pelagicore AG
** Contact: http://www.qt.io/ or http://www.pelagicore.com/
**
** This file is part of the QmlLive tool.
**
** $QT_BEGIN_LICENSE:GPL3-PELAGICORE$
** Commercial License Usage
** Licensees holding valid commercial Pelagicore QmlLive
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

#include "benchlivenodeengine.h"
#include "directorypreviewadapter.h"
#include "previewimageprovider.h"
#include "qmlpreviewadapter.h"
#include "widgets/windowwidget.h"
#include "liveruntime.h"
#include "widgets/workspaceview.h"
#include "benchquickview.h"

#include <QFileIconProvider>
#include <QQmlImageProviderBase>

BenchLiveNodeEngine::BenchLiveNodeEngine(QObject *parent)
    : LiveNodeEngine(parent),
      m_view(0),
      m_ww(0),
      m_workspaceView(0),
      m_ignoreCache(false),
      m_clipToRootObject(false)
{
    setUpdateMode(LiveNodeEngine::RecreateView);
}

void BenchLiveNodeEngine::setWindowWidget(WindowWidget* widget)
{
    m_ww = widget;
}

QQuickView* BenchLiveNodeEngine::initView()
{
    m_view = new BenchQuickView();
    connect(m_view, SIGNAL(sizeChanged(QSize)), this, SLOT(onSizeChanged(QSize)));

    if (!m_imageProvider) {
        m_imageProvider = new PreviewImageProvider(this);
    }

    m_imageProvider->setIgnoreCache(m_ignoreCache);

    m_view->engine()->addImageProvider("qmlLiveDirectoryPreview", m_imageProvider);

    emit viewChanged(m_view);

    return m_view;
}

void BenchLiveNodeEngine::setWorkspaceView(WorkspaceView *view)
{
    m_workspaceView = view;
}

//If a refresh is done we want to reload everything
void BenchLiveNodeEngine::refresh()
{
    m_ignoreCache = true;
    reloadHelper();
}

//A normal reload should use the cache
void BenchLiveNodeEngine::reloadDocument()
{
    m_ignoreCache = false;
    reloadHelper();
}

/*!
 *  Loads the fileIcon for the file provided by \a info and returns a QImage.
 *
 *  It tries to resize the Icon to the \a requestedSize, but the content may be smaller.
 *
 *  This function is used by the previewimageprovide and has to run in the mainthread.
 *  That's why it lives in the LiveNodeEngine
 */
QImage BenchLiveNodeEngine::convertIconToImage(const QFileInfo &info, const QSize &requestedSize)
{
    QFileIconProvider iconProvider;
    QIcon icon = iconProvider.icon(info);
    QImage img(requestedSize, QImage::Format_ARGB32);
    img.fill(Qt::transparent);
    QPainter painter(&img);
    icon.paint(&painter, QRect(QPoint(0,0), requestedSize));
    return img;
}

void BenchLiveNodeEngine::onSizeChanged(const QSize &size)
{
    m_runtime->setScreenWidth(size.width());
    m_runtime->setScreenHeight(size.height());
}

void BenchLiveNodeEngine::onWidthChanged(int width)
{
    m_runtime->setScreenWidth(width);
}

void BenchLiveNodeEngine::onHeightChanged(int height)
{
    m_runtime->setScreenHeight(height);
}

void BenchLiveNodeEngine::initPlugins()
{
    LiveNodeEngine::initPlugins();

    DirectoryPreviewAdapter* adapter = new DirectoryPreviewAdapter(this);
    if (m_workspaceView) {
        //This needs to be QueuedConnection because Qt5 doesn't like it to destruct it's object while it is in a signalHandler
        connect(adapter, SIGNAL(loadDocument(QString)), m_workspaceView, SLOT(activateDocument(QString)), Qt::QueuedConnection);
    }

    QmlPreviewAdapter* previewAdapter = new QmlPreviewAdapter(this);

    previewAdapter->setImportPaths(importPaths());

    m_plugins.append(adapter);
    m_plugins.append(previewAdapter);

    if (m_imageProvider)
        m_imageProvider->setPlugins(m_plugins);
}

void BenchLiveNodeEngine::reloadHelper()
{
    LiveNodeEngine::reloadDocument();

    QAbstractScrollArea* scroller = m_ww;

    ContentAdapterInterface *adapter = activePlugin();
    if (adapter && adapter->isFullScreen()) {
        m_ww->setCenteringEnabled(false);
        scroller->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    } else {
        m_ww->setCenteringEnabled(true);
        scroller->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
    m_ww->forceInitialResize();
}
