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

#include "logger.h"
#include "stdio.h"

Logger *Logger::s_instance = 0;
bool Logger::s_ignoreMesssages = false;
QMutex Logger::m_mutex;

/*!
 * \class Logger
 * \brief Installs a qt messageHandler and receives all log messages
 * \group qmllive
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
 * \fn void Logger::message(int type, const QString &msg)
 * Emitted for every incoming log message
 * \a type describes the type of the log message (QtMsgType).
 * \a msg is the debug message
 * This signal will not be emitted when ignoreMessages set to true
 * \sa setIgnoreMessages()
 */
