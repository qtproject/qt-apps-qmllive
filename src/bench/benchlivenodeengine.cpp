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

#include "benchlivenodeengine.h"
#include "directorypreviewadapter.h"
#include "previewimageprovider.h"
#include "qmlpreviewadapter.h"
#include "widgets/windowwidget.h"
#include "liveruntime.h"
#include "widgets/workspaceview.h"

#include <QFileIconProvider>
#include <QQmlImageProviderBase>

BenchLiveNodeEngine::BenchLiveNodeEngine(QObject *parent)
    : LiveNodeEngine(parent),
      m_ww(0),
      m_imageProvider(new PreviewImageProvider(this)),
      m_workspaceView(0),
      m_clipToRootObject(false)
{
    setQmlEngine(new QQmlEngine(this));
    setFallbackView(new QQuickView(qmlEngine(), 0));

    qmlEngine()->addImageProvider("qmlLiveDirectoryPreview", m_imageProvider);
}

BenchLiveNodeEngine::~BenchLiveNodeEngine()
{
    delete fallbackView();
}

void BenchLiveNodeEngine::setWindowWidget(WindowWidget *widget)
{
    m_ww = widget;
}

void BenchLiveNodeEngine::setWorkspaceView(WorkspaceView *view)
{
    m_workspaceView = view;
}

//If a refresh is done we want to reload everything
void BenchLiveNodeEngine::refresh()
{
    m_imageProvider->setIgnoreCache(true);
    reloadHelper();
}

//A normal reload should use the cache
void BenchLiveNodeEngine::reloadDocument()
{
    m_imageProvider->setIgnoreCache(false);
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

void BenchLiveNodeEngine::initPlugins()
{
    LiveNodeEngine::initPlugins();

    DirectoryPreviewAdapter *adapter = new DirectoryPreviewAdapter(this);
    if (m_workspaceView) {
        //This needs to be QueuedConnection because Qt5 doesn't like it to destruct it's object while it is in a signalHandler
        connect(adapter, SIGNAL(loadDocument(QString)), m_workspaceView, SLOT(activateDocument(QString)), Qt::QueuedConnection);
    }

    QmlPreviewAdapter *previewAdapter = new QmlPreviewAdapter(this);

    previewAdapter->setImportPaths(qmlEngine()->importPathList());

    m_plugins.append(adapter);
    m_plugins.append(previewAdapter);

    if (m_imageProvider)
        m_imageProvider->setPlugins(m_plugins);
}

void BenchLiveNodeEngine::reloadHelper()
{
    // avoid flickering
    fallbackView()->resize(m_ww->size());

    LiveNodeEngine::reloadDocument();

    QAbstractScrollArea *scroller = m_ww;

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
