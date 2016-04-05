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
#include <QtQuick>

#include "contentadapterinterface.h"

class IpcPeer;
class LiveRuntime;
class ContentPluginFactory;

class LiveNodeEngine : public QObject
{
    Q_OBJECT
public:

    enum UpdateMode {
        ReloadDocument,
        RecreateView,
        RecreateProcess
    };
    Q_ENUMS(UpdateMode)

    explicit LiveNodeEngine(QObject *parent = 0);
    virtual void setView(QQuickView* view);

    int xOffset() const;
    int yOffset() const;
    int rotation() const;

    void setUpdateMode(LiveNodeEngine::UpdateMode mode);
    LiveNodeEngine::UpdateMode updateMode() const;

    void setWorkspace(const QString& path);

    void setImportPaths(const QStringList& paths);
    QStringList importPaths() const;

    void setPluginPath(const QString& path);
    QString pluginPath() const;

    QUrl activeDocument() const;
    ContentAdapterInterface *activePlugin() const;
    bool isReloadPluginsEnabled() const;

public Q_SLOTS:

    void setXOffset(int offset);
    void setYOffset(int offset);
    void setRotation(int rotation);
    void setReloadPluginsEnabled(bool enabled);
    void setActiveDocument(const QString& document);
    void loadDocument(const QUrl& url);
    void delayReload();
    virtual void reloadDocument();
Q_SIGNALS:
    void activateDocument(const QString& document);
    void logClear();
    void logIgnoreMessages(bool on);
    void documentLoaded();
    void viewChanged(QQuickView *view);
    void logErrors(const QList<QQmlError> &errors);


protected:
    virtual QQuickView* initView();
    virtual void initPlugins();
    QList<ContentAdapterInterface*> m_plugins;
    QUrl m_activeFile;
    LiveRuntime *m_runtime;
private Q_SLOTS:
    void onStatusChanged(QQuickView::Status status);
    void onSizeChanged();

    void recreateView();

    void checkQmlFeatures(QQuickView *view);

private:
    QUrl queryDocumentViewer(const QUrl& url);
private:
    int m_xOffset;
    int m_yOffset;
    int m_rotation;

    IpcPeer *m_ipc;
    QQuickView *m_view;
    QQuickView *m_recreateView;
    QPointer<QQmlComponent> m_windowComponent;
    QPointer<QObject> m_windowObject;
    QDir m_workspace;
    QTimer *m_delayReload;
    QStringList m_importPaths;

    UpdateMode m_mode;

    ContentPluginFactory* m_pluginFactory;
    ContentAdapterInterface* m_activePlugin;

    ContentAdapterInterface::Features m_quickFeatures;

    bool m_reloadPlugins;
};

