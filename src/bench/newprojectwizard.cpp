/****************************************************************************
**
** Copyright (C) 2019 Luxoft Sweden AB
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

const QString QmlLiveProjectsLocation(QDir(QDir().homePath()).absoluteFilePath("QMLLive"));

NewProjectWizard::NewProjectWizard(QWidget *parent)
    : QWizard(parent)
    , m_importListWidget(nullptr)
    , m_projectPage(new ProjectPage())
    , m_workspacePage(new WorkspacePage())
    , m_mainDocumentPage(new MainDocumentPage())
    , m_projectFileDir(nullptr)
{
    setWizardStyle(QWizard::ClassicStyle);
    setOptions(QWizard::NoBackButtonOnStartPage);
    addPage(m_projectPage);
    addPage(m_workspacePage);
    addPage(createImportsPage());
    addPage(m_mainDocumentPage);

    connect(m_workspacePage, &WorkspacePage::updateWorkspace, m_mainDocumentPage, &MainDocumentPage::updateWorkspace);
    connect(m_projectPage, &ProjectPage::updateProjectDir, m_workspacePage, &WorkspacePage::onUpdateProjectDir);
    connect(m_projectPage, &ProjectPage::updateProjectDir, m_mainDocumentPage, &MainDocumentPage::onUpdateProjectDir);
    connect(m_projectPage, &ProjectPage::updateProjectDir, this, &NewProjectWizard::onUpdateProjectDir);
}

MainDocumentPage::MainDocumentPage(QWidget *parent)
    : QWizardPage (parent)
    , m_workspace(nullptr)
    , m_mainDocumentField(new QLineEdit)
    , m_projectFileDir(nullptr)
{
    setTitle("Main Document");

    QVBoxLayout *vbox = new QVBoxLayout;

    QLabel *hintlabel = new QLabel("Please select main QML document to load initially.");
    hintlabel->setWordWrap(true);
    hintlabel->setAlignment(Qt::AlignJustify);
    vbox->addWidget(hintlabel);

    QGridLayout *layout = new QGridLayout;

    QLabel *label = new QLabel("Main document: ");
    layout->addWidget(label, 1, 0);

    m_mainDocumentField = new QLineEdit;
    registerField("mainDocument*", m_mainDocumentField);
    layout->addWidget(m_mainDocumentField, 1, 1);

    QPushButton *button = new QPushButton("Select");
    layout->addWidget(button, 1, 2);
    connect(button, SIGNAL(clicked()), this, SLOT(selectDocument()));

    m_warningLabel = new QLabel;
    QPalette palette = m_warningLabel->palette();
    palette.setColor(m_warningLabel->backgroundRole(), Qt::yellow);
    palette.setColor(m_warningLabel->foregroundRole(), Qt::yellow);
    m_warningLabel->setPalette(palette);
    layout->addWidget(m_warningLabel, 2, 0, 1, 3, Qt::AlignTop);

    layout->setColumnStretch(1, 1);
    layout->setRowStretch(1, 1);
    vbox->addLayout(layout);
    setLayout(vbox);

}

QString MainDocumentPage::mainDocument() const
{
    if (m_mainDocumentField) {
        return m_mainDocumentField->text();
    }
    return "";
}

void MainDocumentPage::onUpdateProjectDir(const QString &path)
{
    if (m_projectFileDir == nullptr){
        m_projectFileDir = new QDir(path);
    }
    else {
        m_projectFileDir->setPath(path);
    }
}

void MainDocumentPage::selectDocument()
{
    m_warningLabel->setText("");
    QString filter = tr("QML (*.qml);; All files (*.*)");
    QString path = QFileDialog::getOpenFileName(this, "Select File", *m_workspace, filter);
    QString file = QDir(m_projectFileDir->absoluteFilePath(*m_workspace)).relativeFilePath(path);

    if (!file.isEmpty() && m_mainDocumentField) {
        m_mainDocumentField->setText(file);
    }
}

bool MainDocumentPage::validatePage()
{
    if (QFileInfo(QDir(m_projectFileDir->absoluteFilePath(*m_workspace)), mainDocument()).exists()) {
        m_warningLabel->setText("");
        return true;
    } else {
        m_warningLabel->setText("The file you entered does not exist or is located not in the workspace folder.");
        return false;
    }
}

void MainDocumentPage::updateWorkspace(const QString &workspace)
{
    if (m_workspace == nullptr){
        m_workspace = new QString(workspace);
    }
    else {
        *m_workspace = workspace;
    }
}


WorkspacePage::WorkspacePage(QWidget *parent)
    : QWizardPage (parent)
    , m_workspaceField(new QLineEdit)
    , m_warningLabel(new QLabel)
    , m_projectFileDir(nullptr)
{
    setTitle("Select Workspace");
    QVBoxLayout *vbox = new QVBoxLayout;

    QLabel *hintlabel = new QLabel("Please select workspace folder to watch for updates of edited QML files. "
                                   "The path should be relative to the project file location.");
    hintlabel->setWordWrap(true);
    hintlabel->setAlignment(Qt::AlignJustify);
    vbox->addWidget(hintlabel);

    QGridLayout *layout = new QGridLayout;

    QLabel *label = new QLabel("Workspace: ");
    layout->addWidget(label, 1, 0);

    m_workspaceField = new QLineEdit;
    registerField("workspace*", m_workspaceField);
    layout->addWidget(m_workspaceField, 1, 1);

    QPushButton *button = new QPushButton("Select");
    layout->addWidget(button, 1, 2);
    connect(button, SIGNAL(clicked()), this, SLOT(selectWorkspacePath()));

    m_warningLabel = new QLabel;
    QPalette palette = m_warningLabel->palette();
    palette.setColor(m_warningLabel->backgroundRole(), Qt::yellow);
    palette.setColor(m_warningLabel->foregroundRole(), Qt::yellow);
    m_warningLabel->setPalette(palette);
    layout->addWidget(m_warningLabel, 2, 0, 1, 3, Qt::AlignTop);

    layout->setColumnStretch(1, 1);
    layout->setRowStretch(1, 1);
    vbox->addLayout(layout);
    setLayout(vbox);
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
    QString path = QFileDialog::getExistingDirectory(this, "Select Workspace");
    QString workspace = m_projectFileDir->relativeFilePath(path);
    if (!workspace.isEmpty() && m_workspaceField) {
        m_workspaceField->setText(workspace);
    }
}

bool WorkspacePage::validatePage()
{
    if (m_projectFileDir->exists(workspace())) {
        m_warningLabel->setText("");
        emit updateWorkspace(workspace());
        return true;
    } else {
        m_warningLabel->setText("The path you entered does not exist.");
        return false;
    }
}

void WorkspacePage::onUpdateProjectDir(const QString &path)
{
    if (m_projectFileDir == nullptr){
        m_projectFileDir = new QDir(path);
    }
    else {
        m_projectFileDir->setPath(path);
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
    , m_projectField(new QLineEdit)
    , m_dirField(new QLineEdit)
{
    setTitle("Project Name");
    QVBoxLayout *vbox = new QVBoxLayout;

    QLabel *hintlabel = new QLabel("This wizard generates a QmlLive project. The QmlLive project file shall describe the "
                                   "common options for a QmlLiveBench by specifying the workspace folder, the main document "
                                   "and the import paths relative to the project document location.");
    hintlabel->setWordWrap(true);
    hintlabel->setAlignment(Qt::AlignJustify);
    vbox->addWidget(hintlabel);

    QGridLayout *layout = new QGridLayout;

    QLabel *label = new QLabel("Project name: ");
    layout->addWidget(label, 1, 0);

    m_projectField = new QLineEdit;
    registerField("projectName*", m_projectField);
    m_projectField->setPlaceholderText("MyQmlLiveProject");
    layout->addWidget(m_projectField, 1, 1);

    QLabel *dirlabel = new QLabel("Create in: ");
    layout->addWidget(dirlabel, 2, 0);

    layout->addWidget(m_dirField, 2, 1);
    m_dirField->setText(QmlLiveProjectsLocation);

    QPushButton *button = new QPushButton("Select");
    layout->addWidget(button, 2, 2);
    connect(button, &QPushButton::clicked, this, &ProjectPage::selectProjectPath);

    layout->setColumnStretch(1, 1);
    layout->setRowStretch(1, 1);
    vbox->addLayout(layout);
    setLayout(vbox);
}

QString ProjectPage::projectName() const
{
    if (m_projectField) {
        if (m_dirField->text().isEmpty())
            return m_projectField->text();
        else {
            return QDir(m_dirField->text()).absoluteFilePath(m_projectField->text());
        }
    }
    return "";
}

bool ProjectPage::validatePage()
{
    QDir dir(m_dirField->text());
    if (QDir().mkpath(m_dirField->text())) {
        emit updateProjectDir(m_dirField->text());
        return true;
    }
    else {
        qWarning()<< "Unable to create directory: "<< m_dirField->text();
        return false;
    }
}

void ProjectPage::selectProjectPath()
{
    QString projectPath = QFileDialog::getExistingDirectory(this, "Create in");
    if (!projectPath.isEmpty() && m_dirField) {
        m_dirField->setText(projectPath);
    }
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
    QString relativepath = m_projectFileDir->relativeFilePath(path);
    QListWidgetItem *item = new QListWidgetItem(relativepath);
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

void NewProjectWizard::onUpdateProjectDir(const QString &path)
{
    if (m_projectFileDir == nullptr){
        m_projectFileDir = new QDir(path);
    }
    else {
        m_projectFileDir->setPath(path);
    }
}
