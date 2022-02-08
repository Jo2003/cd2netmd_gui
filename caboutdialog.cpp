/**
 * Copyright (C) 2021 Jo2003 (olenka.joerg@gmail.com)
 * This file is part of cd2netmd_gui
 *
 * cd2netmd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cd2netmd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 */
#include <QFont>
#include "caboutdialog.h"
#include "defines.h"
#include "ui_caboutdialog.h"

CAboutDialog::CAboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CAboutDialog)
{
    ui->setupUi(this);
    ui->labelName->setText(QString("%1 %2 %3").arg(c2n::PROGRAM_NAME).arg(c2n::PROGRAM_VERSION).arg(__DATE__));
    QString text =
R"(<b>CD to NetMD GUI - a tool like Sonys NetMD Simply Burner</b><br>
Project site: <a style="color: #3499b3; font-weight: bold;" href="https://github.com/Jo2003/cd2netmd_gui">github.com</a><br>
Thanks for following projects:<br>
<ul>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://www.gnu.org/software/libcdio/">libcdio</a> with CDDA paranoia extension.</li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://github.com/nlohmann/json">json.hpp - JSON for modern C++</a></li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://github.com/dcherednik/atracdenc">atracdenc - ATRAC Decoder Encoder</a></li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://github.com/Jo2003/netmd">linux-minidisc</a></li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://github.com/cybercase/webminidisc">webminidisc - the idea behind all of that</a></li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://www.qt.io/">Qt - One framework to rule them all</a></li>
</ul>
Special thanks to my personal tester <b>Lars W.</b> - the first guy who paid me a beer (or two)!
<br /> <br />
Please support me through <a style="color: #3499b3; font-weight: bold;" href="https://paypal.me/Jo2003">Paypal</a>!)";
    ui->textAbout->setHtml(text);
#ifdef Q_OS_MAC
    QFont f = ui->textAbout->font();
    f.setPointSize(f.pointSize() + 4);
    ui->textAbout->setFont(f);
#endif
}

CAboutDialog::~CAboutDialog()
{
    delete ui;
}
