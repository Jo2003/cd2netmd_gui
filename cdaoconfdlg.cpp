/**
 * Copyright (C) 2022 Jo2003 (olenka.joerg@gmail.com)
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
#include "cdaoconfdlg.h"
#include "ui_cdaoconfdlg.h"
#include <QSettings>

//------------------------------------------------------------------------------
//! @brief      Constructs a new instance.
//!
//! @param      parent  The parent widet
//------------------------------------------------------------------------------
CDaoConfDlg::CDaoConfDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDaoConfDlg)
{
    ui->setupUi(this);
    QString s = R"(
<b style="font-size: x-large;">(D)isc (A)t (O)nce or Gapless Mode</b> - Please read careful!
<p>DAO / gapless is supported in 4 modes. All have there pros and cons.</p>
<b style="font-size: large;">DAO LP2 / LP4 Mode</b>
<p>The audio content will be extracted and compressed at once. After that, the audio will be split into tracks
and transferred to your NetMD device. You have to expect quality loss due to LP2/LP4 mode and the usage of an
external encoder. Playback is only supported on MDLP capable devices.</p>
<b style="font-size: large;">DAO SP Mode</b>
<p>The audio content will be extracted and transferred to the NetMD device at once.
After that the audio data will be split directly on the NetMD device through TOC edit. This gives you the best possible quality.
Playback is supported on all MD devices. Unfortunately, this is only supported on Sony / Aiwa portable type R, and type S devices.</p>
<b style="font-size: large;">DAO SP Pre-Enc Mode</b>
<p>The whole audio content will be extracted and converted into Atrac 1, and transferred to the NetMD device at once.
After that the audio data will be split directly on the NetMD device through TOC edit. Playback is supported on all MD devices.
Unfortunately, this is only supported on Sony portable type S devices. This mode is much faster than the other SP modi, but quality
depends on atracdenc and might be slightly worse.</p>
<b style="font-size: large;">DAO SP Mono Mode</b>
<p>The whole audio content will be extracted and transferred to the NetMD device at once. The NetMd device converts the audio data
into SP mono (needing half the data size). After that the audio data will be split directly on the NetMD device through TOC edit.
Playback is supported on all MD devices. Unfortunately, this is only supported on Sony portable type R - and S devices.</p>
<table style="margin: 4px"><tr><td style="color:red; background-color: #fff6d1; padding: 3px; border: 3px solid red;">
For all DAO SP modi I'd recommend the usage of a blank MD. While we take care for existing content, you might end up with issues on very fragmented discs.
Furthermore, take care that there is no pending TOC edit on your NetMD device before starting the DAO upload. Simply press 'stop' on your device
<b>&rarr; right now &larr;</b> before going on!
</td></tr></table>
<blockquote><p>Please note: Any change on CD track list will be reverted before starting!</p>
</blockquote>)";
    ui->textBrowser->setHtml(s);
    QFont f = ui->textBrowser->font();
#ifdef Q_OS_MAC
    f.setPointSize(f.pointSize() + 4);
#else
    f.setPointSize(f.pointSize() + 1);
#endif
    ui->textBrowser->setFont(f);
}

//------------------------------------------------------------------------------
//! @brief      Destroys the object.
//------------------------------------------------------------------------------
CDaoConfDlg::~CDaoConfDlg()
{
    delete ui;
}

//--------------------------------------------------------------------------
//! @brief      don't show again toggled
//!
//! @param      checked  true or false
//--------------------------------------------------------------------------
void CDaoConfDlg::on_checkDontShow_toggled(bool checked)
{
    QSettings set;
    set.setValue("dont_show_dao_info", checked);
}
