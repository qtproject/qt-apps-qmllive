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

#include "newprojectwizard.h"

#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>
#include <QFileDialog>
#include "livedocument.h"

NewProjectWizard::NewProjectWizard(QWidget *parent)
    : QWizard(parent)
    , m_importListWidget(nullptr)
    , m_projectPage(new ProjectPage())
    , m_workspacePage(new WorkspacePage())
    , m_mainDocumentPage(new MainDocumentPage())
{
    setWizardStyle(QWizard::ClassicStyle);
    setOptions(QWizard::NoBackButtonOnStartPage);
    addPage(m_projectPage);
    addPage(m_workspacePage);
    addPage(createImportsPage());
    addPage(m_mainDocumentPage);
}

MainDocumentPage::MainDocumentPage(QWidget *parent)
    : QWizardPage (parent)
{
    setTitle("Main Document");
    QGridLayout *layout = new QGridLayout;

    QLabel *label = new QLabel("Main document: ");
    layout->addWidget(label, 0, 0);

    m_mainDocumentField = new QLineEdit;
    registerField("mainDocument*", m_mainDocumentField);
    layout->addWidget(m_mainDocumentField, 0, 1);

    layout->setColumnStretch(1, 1);
    layout->setRowStretch(1, 1);
    setLayout(layout);

}

QString MainDocumentPage::mainDocument() const
{
    if (m_mainDocumentField) {
        return m_mainDocumentField->text();
    }
    return "";
}

WorkspacePage::WorkspacePage(QWidget *parent)
    : QWizardPage (parent)
{
    setTitle("Select Workspace");
    QGridLayout *layout = new QGridLayout;

    QLabel *label = new QLabel("Workspace: ");
    layout->addWidget(label, 0, 0);

    m_workspaceField = new QLineEdit;
    registerField("workspace*", m_workspaceField);
    layout->addWidget(m_workspaceField, 0, 1);

    QPushButton *button = new QPushButton("Select");
    layout->addWidget(button, 0, 2);
    connect(button, SIGNAL(clicked()), this, SLOT(selectWorkspacePath()));

    m_warningLabel = new QLabel;
    layout->addWidget(m_warningLabel, 1, 0, 1, 3, Qt::AlignTop);

    layout->setColumnStretch(1, 1);
    layout->setRowStretch(1, 1);
    setLayout(layout);
}

QString WorkspacePage::workspace() const
{
    if (m_workspaceField) {
        return m_workspaceField->text();
    }
    return "";
}

void WorkspacePage::selectWorkspacePath()
{
    m_warningLabel->setText("");
    QString workspace = QFileDialog::getExistingDirectory(this, "Select Workspace");
    if (!workspace.isEmpty() && m_workspaceField) {
        m_workspaceField->setText(workspace);
    }
}

bool WorkspacePage::validatePage()
{
    if (QDir(workspace()).exists()) {
        m_warningLabel->setText("");
        return true;
    } else {
        m_warningLabel->setText("The path you entered does not exist.");
        return false;
    }
}

QWizardPage *NewProjectWizard::createImportsPage()
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("Imports");
    QGridLayout *layout = new QGridLayout;

    m_importListWidget = new QListWidget;
    layout->addWidget(m_importListWidget, 0, 0, 4, 1);

    QPushButton *add = new QPushButton("Add");
    connect(add, SIGNAL(clicked()), this, SLOT(addImportPath()));
    layout->addWidget(add, 0, 1);

    QPushButton *edit = new QPushButton("Edit");
    connect(edit, SIGNAL(clicked()), this, SLOT(editImportPath()));
    layout->addWidget(edit, 1, 1);

    QPushButton *remove = new QPushButton("Remove");
    connect(remove, SIGNAL(clicked()), this, SLOT(removeImportPath()));
    layout->addWidget(remove, 2, 1);

    layout->setRowStretch(4, 1);
    page->setLayout(layout);
    return page;
}

ProjectPage::ProjectPage(QWidget *parent)
    : QWizardPage (parent)
{
    setTitle("Project Name");
    setSubTitle("This wizard generates a Qt QmlLive project. The QmlLive project file shall describe the"
                "common options for a QmlLive project by specifying the workspace folder, the main document"
                "and the import paths relative to the project document location.");

    QGridLayout *layout = new QGridLayout;

    QLabel *label = new QLabel("Project name: ");
    layout->addWidget(label, 0, 0);

    m_projectField = new QLineEdit;
    registerField("projectName*", m_projectField);
    m_projectField->setPlaceholderText("MyQmlLiveProject");
    layout->addWidget(m_projectField, 0, 1);

    layout->setColumnStretch(1, 1);
    layout->setRowStretch(1, 1);
    setLayout(layout);
}

QString ProjectPage::projectName() const
{
    if (m_projectField) {
        return m_projectField->text();
    }
    return "";
}

QString NewProjectWizard::mainDocument() const
{
    return m_mainDocumentPage->mainDocument();
}

QString NewProjectWizard::workspace() const
{
    return m_workspacePage->workspace();
}

QStringList NewProjectWizard::imports() const
{
    QStringList list;
    if (m_importListWidget) {
        for (int i = 0; i < m_importListWidget->count(); i++) {
            list.append(m_importListWidget->takeItem(i)->text());
        }
    }

    return list;
}

QString NewProjectWizard::projectName() const
{
    return m_projectPage->projectName();
}

void NewProjectWizard::addImportPath()
{
    QString path = QFileDialog::getExistingDirectory(this, "Add Import Path");
    if (path.isEmpty()) {
        return;
    }
    QListWidgetItem *item = new QListWidgetItem(path);
    item->setFlags(item->flags () | Qt::ItemIsEditable);
    m_importListWidget->addItem(item);

}

void NewProjectWizard::editImportPath()
{
    QListWidgetItem *item = m_importListWidget->currentItem();
    if (item) {
        m_importListWidget->editItem(item);
    }

}

void NewProjectWizard::removeImportPath()
{
    QListWidgetItem *item = m_importListWidget->currentItem();
    if (item) {
        delete item;
    }
}
