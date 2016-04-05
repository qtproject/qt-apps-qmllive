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

#include "logview.h"

LogView::LogView(bool createLogger, QWidget *parent)
    : QWidget(parent)
    , m_log(new QPlainTextEdit(this))
    , m_logger(0)
{
    m_log->setReadOnly(true);
    m_log->setMaximumBlockCount(1000);
    m_log->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_log);
    setLayout(layout);

    if (createLogger) {
        m_logger = new Logger(this);
        connect(m_logger, SIGNAL(message(int,QString)), this, SLOT(appendToLog(int,QString)));
    }
}

void LogView::setIgnoreMessages(bool ignoreMessages)
{
    if (m_logger)
        m_logger->setIgnoreMessages(ignoreMessages);
}

void LogView::appendToLog(int type, const QString &msg, const QUrl &url, int line, int column)
{
    QMutexLocker l(&m_mutex);

    qreal baseValue = m_log->palette().color(QPalette::Base).valueF();
    QColor color = m_log->palette().color(QPalette::Text);

    switch (type) {
    case QtWarningMsg: // yellow
        color = baseValue < 0.5f ? QColor(255, 255, 128) : QColor(140, 140, 0);
        break;
    case QtCriticalMsg: // red
        color = baseValue < 0.5f ? QColor(255, 64, 64) : QColor(165, 0, 0);
        break;
    case QtFatalMsg: // red
        color = baseValue < 0.5f ? QColor(255, 64, 64) : QColor(165, 0, 0);
        break;
    case InternalInfo: // green
        color = baseValue < 0.5f ? QColor(96, 255, 96) : QColor(128, 0, 0);
        break;
    case InternalError: // purple
        color = baseValue < 0.5f ? QColor(196, 128, 196) : QColor(96, 0, 96);
        break;
    default:
        break;
    }

    QString s;
    if (url.isValid()) {
        s.append(url.isLocalFile() ? url.toLocalFile() : url.toString());
        s.append(QLatin1Char(':'));
    }
    if (line > 0) {
        s.append(QString::number(line));
        s.append(QLatin1Char(':'));
    }
    Q_UNUSED(column);
    if (!s.isEmpty())
        s.append(QLatin1Char(' '));

    s.append(QString::fromLatin1("<b><font color=\"%2\">%1</font></b>")
             .arg(msg)
             .arg(color.name()));

    m_log->appendHtml(s);
}

void LogView::appendToLog(const QList<QQmlError> &errors)
{
    foreach (const QQmlError &err, errors) {
        if (!err.isValid())
            continue;

        QtMsgType type = QtDebugMsg;

        if (err.description().contains(QString::fromLatin1("error"), Qt::CaseInsensitive) ||
            err.description().contains(QString::fromLatin1("is not installed"), Qt::CaseInsensitive) ||
            err.description().contains(QString::fromLatin1("is not a type"), Qt::CaseInsensitive))
            type = QtCriticalMsg;
        else if (err.description().contains(QString::fromLatin1("warning"), Qt::CaseInsensitive))
            type = QtWarningMsg;

        appendToLog(type, err.description(), err.url(), err.line(), err.column());
    }
}

void LogView::clear()
{
    QMutexLocker l(&m_mutex);
    m_log->clear();
}

