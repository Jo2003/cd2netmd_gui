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
R"(<b>NetMD Wizard - a tool like Sonys NetMD Simply Burner</b><br>
Project site: <a style="color: #3499b3; font-weight: bold;" href="https://github.com/Jo2003/cd2netmd_gui">github.com</a>
<br> <br>
If you like this program, please support me through <a style="color: #3499b3; font-weight: bold;" href="https://paypal.me/Jo2003">Paypal</a>!
<br> <br>
Thanks for following projects:<br>
<ul>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://www.gnu.org/software/libcdio/">libcdio</a> with CDDA paranoia extension.</li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://github.com/nlohmann/json">json.hpp</a> - JSON for modern C++</li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://github.com/dcherednik/atracdenc">atracdenc</a> - ATRAC Decoder Encoder</li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://github.com/Jo2003/netmd">linux-minidisc</a></li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://github.com/cybercase/webminidisc">webminidisc</a> - the idea behind all of that</li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://github.com/Jo2003/libcue/">libcue</a> - for making Cue Sheet parsing a bit easier</li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://taglib.org/">taglib</a> - for make reading tags much easier</li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://www.ffmpeg.org/">ffmpeg</a> - for encoding <b>xxx</b> to compatible wav files</li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://www.qt.io/">Qt</a> - One framework to rule them all</li>
</ul>
Special thanks to my personal tester <b>Lars W.</b> - the first guy who paid me a beer (or two)!)";
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
