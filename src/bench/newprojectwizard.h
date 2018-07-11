/****************************************************************************
**
** Copyright (C) 2018 Pelagicore AG
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

#include <QWizard>
#include <QWizardPage>
#include <QLineEdit>
#include <QListWidget>
#include <QLabel>

class ProjectPage : public QWizardPage
{
    Q_OBJECT

public:
    ProjectPage(QWidget *parent = nullptr);
    QString projectName() const;

private:
    QLineEdit *m_projectField;
};

class WorkspacePage : public QWizardPage
{
    Q_OBJECT

public:
    WorkspacePage(QWidget *parent = nullptr);
    QString workspace() const;
    bool validatePage() override;

private slots:
    void selectWorkspacePath();

private:
    QLineEdit *m_workspaceField;
    QLabel *m_warningLabel;
};

class MainDocumentPage : public QWizardPage
{
    Q_OBJECT

public:
    MainDocumentPage(QWidget *parent = nullptr);
    QString mainDocument() const;

private:
    QLineEdit *m_mainDocumentField;
};

class NewProjectWizard : public QWizard
{
    Q_OBJECT
public:
    explicit NewProjectWizard(QWidget *parent = 0);
    QWizardPage* createMainDocumentPage();
    QWizardPage* createWorkspacePage();
    QWizardPage* createImportsPage();
    QWizardPage* createProjectPage();

    QString mainDocument() const;
    QString workspace() const;
    QStringList imports() const;
    QString projectName() const;

private slots:
    void addImportPath();
    void editImportPath();
    void removeImportPath();

private:
    QListWidget *m_importListWidget;
    ProjectPage *m_projectPage;
    WorkspacePage *m_workspacePage;
    MainDocumentPage *m_mainDocumentPage;
};
