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

#include "logger.h"
#include "stdio.h"

Logger *Logger::s_instance = 0;
bool Logger::s_ignoreMesssages = false;
QMutex Logger::m_mutex;

/*!
 * \class Logger
 * \brief Installs a qt messageHandler and receives all log messages
 * \inmodule qmllive
 *
 * The intention is to use this class if you want to display the log into widget
 * or you want to preproccess the log messages itself before displaying it
 *
 * \sa RemoteLogger
 */

/*!
 * Standard constructor using \a parent as parent
 */
Logger::Logger(QObject *parent) :
    QObject(parent)
{
    if (s_instance) {
        qFatal("Cannot create more than one Logger");
    }
    s_instance = this;

    qInstallMessageHandler(messageHandler);
}

/*!
 * Standard destructor
 */
Logger::~Logger()
{
    qInstallMessageHandler(0);
}

/*!
 * When \a ignoreMessages set to true, the Logger ignores all incoming log messages
 */
void Logger::setIgnoreMessages(bool ignoreMessages)
{
    QMutexLocker l(&m_mutex);
    s_ignoreMesssages = ignoreMessages;
}

void Logger::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    if (!s_instance) {
        printf("No Logger instance for log message:\n  %s\n", msg.toLatin1().constData());
        abort();
    }

    {
        QMutexLocker l(&m_mutex);
        if (s_ignoreMesssages)
            return;
    }

    emit s_instance->message(type, msg);

    // and output to stdout
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
#endif
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
    }
}

/*!
 * \fn Logger::message(int type, const QString &msg, const QUrl &url = QUrl(), int line, int column);
 *
 * Emitted for every incoming log message \a type describes the type of the log
 * message (QtMsgType), \a msg is the debug message, \a url is the document
 * location, \a line and \a column is the position in the document. This signal
 * will not be emitted when ignoreMessages set to true \sa setIgnoreMessages()
 */
