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
