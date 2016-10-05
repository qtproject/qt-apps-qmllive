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

#include <QtWidgets>
#include <remotepublisher.h>

class Host;

class HostWidget : public QWidget
{
    Q_OBJECT
public:

    explicit HostWidget(QWidget *parent = 0);

    void setHost(Host* host);
    void setLiveHubEngine(LiveHubEngine* engine);
    void setCurrentFile(const QString currentFile);
    bool followTreeSelection() const;

signals:
    void connected();
    void openHostConfig(Host*);
    void remoteLog(int type, const QString &msg, const QUrl &url = QUrl(), int line = -1, int column = -1);
    void clearLog();

public slots:
    void probe();
    void publishWorkspace();
    void refresh();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    bool eventFilter(QObject *, QEvent *);

private slots:
    void updateName(const QString& name);
    void updateIp(const QString& ip);
    void updatePort(int port);
    void updateFile(const QString& file);
    void setUpdateFile(const QString& file);
    void updateOnlineState(bool online);

    void connectToServer();
    void connectAndSendFile();

    void onConnected();
    void onDisconnected();
    void onConnectionError(QAbstractSocket::SocketError error);

    void sendDocument(const QString &document);

    void sendXOffset(int offset);
    void sendYOffset(int offset);
    void sendRotation(int rotation);

    void onSentSuccessfully(const QUuid &uuid);
    void onSendingError(const QUuid &uuid, QAbstractSocket::SocketError socketError);
    void resetProgressBar();

    void showPinDialog();
    void onPinOk(bool ok);

    void publishAll();
    void onEditHost();

    void resizeEvent( QResizeEvent * event );
private:
    QStackedLayout *m_stackedLayout;

    QGroupBox* m_groupBox;
//    QLabel* m_ipLabel;
    QLabel* m_documentLabel;
//    QLabel* m_statusLabel;
    QProgressBar* m_sendProgress;
    QToolButton* m_menuButton;
    QMenu* m_menu;
    QAction* m_publishAction;
    QAction* m_followTreeSelectionAction;
    QAction* m_refreshAction;
    QAction* m_connectDisconnectAction;
    QAction* m_editHostAction;

    QPointer<Host> m_host;

    RemotePublisher m_publisher;
    QPointer<LiveHubEngine> m_engine;

    QUuid m_activateId;
    QList<QUuid> m_changeIds;
    QUuid m_xOffsetId;
    QUuid m_yOffsetId;
    QUuid m_rotationId;
};

