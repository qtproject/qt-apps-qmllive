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

#include "windowwidget.h"
#include <QQuickView>
#include <QQuickItem>
#include <QGuiApplication>
#include <QDebug>
#include <QScrollBar>
#include <QResizeEvent>

WindowWidget::WindowWidget(QWidget *parent) :
    QAbstractScrollArea(parent), m_hostedWindow(0), m_resizing(false), m_centering(false)
{
    setFrameShape(QFrame::NoFrame);
}

void WindowWidget::setHostedWindow(QQuickWindow *hostedWindow)
{
    if (m_hostedWindow == hostedWindow)
        return;

    if (m_hostedWindow) {
        m_hostedWindow->removeEventFilter(this);
        m_hostedWindow->hide();
        m_hostedWindow->setParent(0);
    }

    m_hostedWindow = hostedWindow;

    if (m_hostedWindow) {
        m_hostedWindow->installEventFilter(this);

        // force creation of the TL window in order to get its handle
        if (!testAttribute(Qt::WA_WState_Created))
            create(0, true, true);

        if (!windowHandle())
            qWarning("Could not get a valid windowhandle for our widget based window");

        // Ensure the native platform resources are not allocated before
        // embedding the window. Otherwise it cannot be embedded reliably.
        //
        // For some reason this only happens with QQuickWindow (or the subclass
        // actually instantiated by QML Window element) and not with QQuickView.
        //
        // At least one unfortunate side effect exists: geometry is not
        // preserved due to the way it is stored (see QWindow::resize(QSize))
        // while this is needed for the QQuickView's auto resizing to work
        // correctly.
        //
        // This is the rationale why it is done and why it is done conditionally.
        //
        // Another options would be:
        //
        // a) Remember and reset the geometry here
        // b) Place the QWindow::destroy() call directly after loading a
        //    QQuickWindow from QML in LiveNodeEngine::reloadDocument()
        // c) Eliminate what makes this only happen with QQuickWindow loaded
        //    from QML
        if (!qobject_cast<QQuickView *>(m_hostedWindow)) {
            m_hostedWindow->destroy();
        }

        m_hostedWindow->setFlags(Qt::Tool | Qt::FramelessWindowHint);
        m_hostedWindow->setParent(windowHandle());
        m_hostedWindow->setVisible(isVisible());

        viewport()->setBackgroundRole(backgroundRole());
        viewport()->setPalette(palette());
        viewport()->setBackgroundRole(QPalette::Background);
        setFocusPolicy(Qt::StrongFocus);
        m_hostedWindow->show();
    }
}

QQuickWindow *WindowWidget::hostedWindow() const
{
    return m_hostedWindow;
}

void WindowWidget::setVisible(bool visible)
{
    QAbstractScrollArea::setVisible(visible);
    if (m_hostedWindow)
        m_hostedWindow->setVisible(visible);
}

void WindowWidget::setCenteringEnabled(bool enabled)
{
    m_centering = enabled;
    updateScrollBars();
}

QSize WindowWidget::sizeHint() const
{
    QSize s = qmlSize();

    return s + QSize(frameWidth() * 2, frameWidth() * 2);
}

void WindowWidget::forceInitialResize()
{
    if (QQuickView *view = qobject_cast<QQuickView *>(m_hostedWindow)) {
        if (view->resizeMode() == QQuickView::SizeRootObjectToView)
            view->resize(size());
    }
    updateGeometry();
    updateScrollBars();
    emit widthChanged(width());
    emit heightChanged(height());

}

bool WindowWidget::event(QEvent *e)
{
    bool handled = false;

    if (m_hostedWindow) {
        switch (e->type()) {
        case QEvent::Wheel: {
            QWheelEvent *oe = static_cast<QWheelEvent *>(e);
            if (!viewport()->geometry().contains(oe->pos()))
                break;

            QWheelEvent ne(m_hostedWindow->mapFromGlobal(oe->globalPos()),
                           oe->globalPos(),
                           oe->pixelDelta(),
                           oe->angleDelta(),
                           oe->delta(),
                           oe->orientation(),
                           oe->buttons(),
                           oe->modifiers());

            qGuiApp->sendEvent(m_hostedWindow, &ne);
            handled = true;
            break;
        }
        case QEvent::KeyPress:
        case QEvent::KeyRelease: {
            qGuiApp->sendEvent(m_hostedWindow, e);
            handled = true;
            break;
        }
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseMove: {
            QMouseEvent *oe = static_cast<QMouseEvent *>(e);

            if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseButtonDblClick) {
                if (!viewport()->geometry().contains(oe->pos()))
                    break;
            }

            QMouseEvent ne(oe->type(),
                           m_hostedWindow->mapFromGlobal(oe->globalPos()),
                           m_hostedWindow->mapFromGlobal(oe->globalPos()),
                           oe->globalPos(),
                           oe->button(),
                           oe->buttons(),
                           oe->modifiers());

            qGuiApp->sendEvent(m_hostedWindow, &ne);
            handled = true;
            break;
        }
        case QEvent::Resize: {
            QResizeEvent *re = static_cast<QResizeEvent*>(e);
            if (QQuickView *view = qobject_cast<QQuickView *>(m_hostedWindow)) {
                if (view->resizeMode() == QQuickView::SizeRootObjectToView)
                    view->resize(re->size());
            }
            updateGeometry();
            updateScrollBars();
            if (re->oldSize().width() != re->size().width())
                emit widthChanged(re->size().width());
            if (re->oldSize().height() != re->size().height())
                emit heightChanged(re->size().height());
            break;
        }
        case QEvent::Hide:
            m_hostedWindow->close();
            break;
        default:
            break;
        }
    }
    return handled ? true : QAbstractScrollArea::event(e);
}

bool WindowWidget::eventFilter(QObject *o, QEvent *e)
{
    if (o && o == m_hostedWindow) {
        switch (e->type()) {
        case QEvent::Resize:
            updateScrollBars();
            break;
        default:
            break;
        }
    }
    return QAbstractScrollArea::eventFilter(o, e);
}

void WindowWidget::scrollContentsBy(int dx, int dy)
{
    Q_UNUSED(dx)
    Q_UNUSED(dy)

    updateWindowPosition();
}

#include <QStyle>

void WindowWidget::updateScrollBars()
{
    if (!m_hostedWindow)
        return;

    if (window() && !m_resizing) {
        m_resizing = true;

        QSize vpSize = viewport()->size();
        QSize vpMaxSize = maximumViewportSize();
        QSize wSize = qmlSize();

        // does the window fit without scrollbars?
        if (vpMaxSize.expandedTo(wSize) == vpMaxSize)
            vpSize = vpMaxSize;

        // fix scrollbars
        horizontalScrollBar()->setRange(0, (wSize - vpSize).width());
        horizontalScrollBar()->setPageStep(vpSize.width());
        verticalScrollBar()->setRange(0, (wSize - vpSize).height());
        verticalScrollBar()->setPageStep(vpSize.height());

        //qWarning() << "wSize:" << wSize << "vpSize:" << vpSize << "vpRect" << viewport()->rect();
        //qWarning() << "Hrange:" << (wSize - vpSize).width() << "Hstep:" << vpSize.width();

        updateWindowPosition();
        m_resizing = false;
    }
}
void WindowWidget::updateWindowPosition()
{
    if (m_hostedWindow && !m_hostedWindow->contentItem()->childItems().isEmpty()) {
        QSize wSize = qmlSize();
        Qt::LayoutDirection dir = layoutDirection();
        QRect scrolled = QStyle::visualRect(dir, viewport()->rect(), QRect(QPoint(-horizontalScrollBar()->value(), -verticalScrollBar()->value()), wSize));
        QRect aligned = QStyle::alignedRect(dir, m_centering ? Qt::AlignCenter : Qt::AlignTop | Qt::AlignLeft, wSize, viewport()->rect());

        //qWarning() << "xAlign:" << aligned.x() << "xScrolled:" << scrolled.x();
        m_hostedWindow->setPosition(wSize.width() < viewport()->width() ? aligned.x() : scrolled.x(),
                                    wSize.height() < viewport()->height() ? aligned.y() : scrolled.y());
    }
}

QSize WindowWidget::qmlSize() const
{
    QSize s;
    if (m_hostedWindow && !m_hostedWindow->contentItem()->childItems().isEmpty()) {
        QQuickItem *const rootItem = m_hostedWindow->contentItem()->childItems().first();
        s = QSize(rootItem->width(), rootItem->height());
    }

    if (s.width() <= 20)
        s.setWidth(800);
    if (s.height() <= 20)
        s.setHeight(600);

    return s;
}

