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

#pragma once

#include "livenodeengine.h"

class BenchQuickView;
class WindowWidget;
class PreviewImageProvider;
class WorkspaceView;
class BenchLiveNodeEngine : public LiveNodeEngine
{
    Q_OBJECT

public:
    BenchLiveNodeEngine(QObject* parent = 0);

    void setWorkspaceView(WorkspaceView* view);
    void setWindowWidget(WindowWidget *widget);

public slots:
    void refresh();
    void reloadDocument();

signals:
    void viewChanged(BenchQuickView *view);

protected:
    void initPlugins();
    void reloadHelper();

    virtual QQuickView* initView();

private Q_SLOTS:
    QImage convertIconToImage(const QFileInfo& info, const QSize& requestedSize);
    void onHeightChanged(int height);
    void onWidthChanged(int width);
    void onSizeChanged(const QSize& size);

private:
    BenchQuickView *m_view;
    WindowWidget* m_ww;
    QPointer<PreviewImageProvider> m_imageProvider;
    WorkspaceView* m_workspaceView;
    bool m_ignoreCache;
    bool m_clipToRootObject;
};
