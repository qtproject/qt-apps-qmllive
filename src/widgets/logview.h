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

#include <QtCore>
#include <QtGui>
#include <QtQuick>
#include <QtWidgets>

#include "logger.h"


class LogView : public QWidget
{
    Q_OBJECT
public:
    enum {
        InternalInfo = QtDebugMsg - 1,
        InternalError = QtDebugMsg - 2
    };

    explicit LogView(bool createLogger = true, QWidget *parent = 0);
public slots:
    void setIgnoreMessages(bool ignoreMessages);
    void clear();
    void appendToLog(int type, const QString &msg, const QUrl &url = QUrl(), int line = -1, int column = -1);
    void appendToLog(const QList<QQmlError> &errors);

private:
    QPlainTextEdit *m_log;

    QMutex m_mutex;
    Logger* m_logger;
};
