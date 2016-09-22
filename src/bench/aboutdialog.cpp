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

#include "aboutdialog.h"

#include <QApplication>
#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

#include "qmllive_version.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("About Qt QML Live"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QString versionExtra;
#ifdef QMLLIVE_VERSION_EXTRA
    versionExtra = QLatin1String(" (") + QLatin1String(QMLLIVE_VERSION_EXTRA_STR) + QLatin1String(")");
#endif

    QString buildInfo;
#ifdef QMLLIVE_REVISION
    buildInfo = tr("<p>Built on %1 %2 from revision %3.</p>")
        .arg(QLatin1String(__DATE__),
             QLatin1String(__TIME__),
             QString::fromLatin1(QMLLIVE_REVISION_STR).left(7));
#endif

    QString about = tr(
        "<h3>Qt QML Live %1%2</h3>"
        "%3"
        "<p>%4</p>"
        "<p>The program is provided AS IS with NO WARRANTY OF ANY KIND, "
        "INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A "
        "PARTICULAR PURPOSE.</p>")
        .arg(QLatin1String(QMLLIVE_VERSION_STR),
             versionExtra,
             buildInfo,
             QLatin1String(QMLLIVE_COPYRIGHT_NOTICE));

    QLabel *label = new QLabel(about);
    label->setWordWrap(true);
    label->setTextInteractionFlags(Qt::TextBrowserInteraction);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox , &QDialogButtonBox::rejected, this, &QDialog::reject);

    layout->addWidget(label);
    layout->addWidget(buttonBox);
}

void AboutDialog::exec(QWidget *parent)
{
    AboutDialog *dialog = new AboutDialog(parent);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->exec();
}
