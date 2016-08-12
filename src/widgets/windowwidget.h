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

#include <QAbstractScrollArea>
#include <QPointer>

QT_FORWARD_DECLARE_CLASS(QQuickWindow);

class WindowWidget : public QAbstractScrollArea
{
    Q_OBJECT
public:
    explicit WindowWidget(QWidget *parent = 0);

    QQuickWindow *hostedWindow() const;
    void setHostedWindow(QQuickWindow *hostedWindow);
    void setVisible(bool visible);
    void setCenteringEnabled(bool enabled);
    QSize sizeHint() const;
    void forceInitialResize();

Q_SIGNALS:
    void widthChanged(int w);
    void heightChanged(int w);

protected:
    bool event(QEvent *e);
    bool eventFilter(QObject *o, QEvent *e);
    void scrollContentsBy(int dx, int dy);

private:
    void updateScrollBars();
    void updateWindowPosition();
    QSize qmlSize() const;

    QPointer<QQuickWindow> m_hostedWindow;
    bool m_resizing;
    bool m_centering;
};

