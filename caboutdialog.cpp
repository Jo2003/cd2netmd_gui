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
#include "git_version.h"

CAboutDialog::CAboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CAboutDialog)
{
    ui->setupUi(this);
    ui->labelName->setText(QString("%1 %2 (c) Jo2003, %3").arg(c2n::PROGRAM_NAME).arg(GIT_VERSION).arg(__DATE__));
    QString text =
R"(<img src=':main/wizard' alt='wizard' width='128' height='128' boarder='0' /><br><b>NetMD Wizard - a tool like Sonys NetMD Simple Burner</b><br>
Project site: <a style="color: #3499b3; font-weight: bold;" href="https://github.com/Jo2003/cd2netmd_gui">github.com</a>
<br> <br>
If you like this program, please support me through <a style="color: #3499b3; font-weight: bold;" href="https://paypal.me/Jo2003">Paypal</a>!
<br> <br>
<b>Thanks for following projects:</b>
<ul>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://www.gnu.org/software/libcdio/">libcdio</a> with CDDA paranoia extension.</li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://github.com/nlohmann/json">json.hpp</a> - JSON for modern C++</li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://github.com/dcherednik/atracdenc">atracdenc</a> - ATRAC Decoder Encoder</li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://github.com/Jo2003/netmd">linux-minidisc</a></li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://github.com/cybercase/webminidisc">webminidisc</a> - the idea behind all of that</li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://taglib.org/">taglib</a> - for make reading tags much easier</li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://www.ffmpeg.org/">ffmpeg</a> - for encoding <b>xxx</b> to compatible wav files</li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://www.qt.io/">Qt</a> - One framework to rule them all</li>
</ul>
<b>Further thanks to:</b>
<ul>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://www.reddit.com/user/asivery">asivery</a> for his great support to add the Sony firmware patching.</li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://www.reddit.com/user/Sir68k">Sir68k</a> for discovering the Sony firmware exploit.</li>
    <li><b style="color: #3499b3;">donlaszlo</b> for his tests and the first ever beer I earned through this project</li>
    <li><b style="color: #3499b3;">Ozzey</b> for his code reviews</li>
    <li><a style="color: #3499b3; font-weight: bold;" href="https://www.reddit.com/user/DaveFlash">DaveFlash</a> for the nice program icon and some nice ideas.</li>
</ul>
<br> <br>)";
    ui->textAbout->setHtml(text);
    QFont f = ui->textAbout->font();
#ifdef Q_OS_MAC
    f.setPointSize(f.pointSize() + 4);
#else
    f.setPointSize(f.pointSize() + 1);
#endif
    ui->textAbout->setFont(f);

}

CAboutDialog::~CAboutDialog()
{
    delete ui;
}
