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

//------------------------------------------------------------------------------
//! @brief      Constructs a new instance.
//!
//! @param      parent  The parent widet
//------------------------------------------------------------------------------
CDaoConfDlg::CDaoConfDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDaoConfDlg), mTocManip(false)
{
    ui->setupUi(this);
    QString s = R"(
<h3 id='disc-at-once-or-gapless-mode'>(D)isc (A)t (O)nce or Gapless Mode</h3>
<p>DAO / gapless is supported in 2 modes. Both have there pros and cons.</p>
<ul>
<li><p><strong>DAO LP2 Mode:</strong> Disc / input will be extracted and compressed at once. After that audio will be split in tracks and transferred to NetMD.</p>
<ul>
<li><strong>Pro:</strong> Track information will be transferred as well.</li>
<li><strong>Contra:</strong> Quality loss due to LP2 mode and external encoder. Playback only on MDLP capable devices.</li>
</ul>
</li>
<li><p><strong>DAO SP Mode:</strong> Disc / input will be extracted and transferred to the NetMD device at once. After that the audio will be split on the NetMD device itself through TOC edit.</p>
<ul>
<li><strong>Pro:</strong> Best quality. Playback on all MD devices. Track information will be transferred as well.</li>
<li><strong>Contra:</strong> This is only supported on Sony / Aiwa type R, and type S  devices.</li>
</ul>
<p style='color:red; background-color: #fff6d1'><strong>Note:</strong> For DAO SP I'd highly recommend the usage of a blank MD.
While editing the TOC, we'll take care for existing tracks. Nevertheless, there is a chance to corrupt
the TOC on very fragmented discs, which ends up in an unplayable disc.</p>
</li>
</ul>
<blockquote><p>Please note: Any change on CD track list will be reverted before starting!</p>
</blockquote>
)";
    ui->textBrowser->setHtml(s);
#ifdef Q_OS_MAC
    QFont f = ui->textBrowser->font();
    f.setPointSize(f.pointSize() + 4);
    ui->textBrowser->setFont(f);
#endif
}

//------------------------------------------------------------------------------
//! @brief      Destroys the object.
//------------------------------------------------------------------------------
CDaoConfDlg::~CDaoConfDlg()
{
    delete ui;
}

//------------------------------------------------------------------------------
//! @brief      give DAO mode
//!
//! @return     DAO mode
//------------------------------------------------------------------------------
CDaoConfDlg::DAO_Mode CDaoConfDlg::daoMode() const
{
    DAO_Mode mode = DAO_Mode::DAO_WTF;

    if (!mTocManip)
    {
        mode = DAO_Mode::DAO_LP2;
    }
    else if (ui->buttonDAOMode->checkedButton()->objectName() == "radioDaoLP2")
    {
        mode = DAO_Mode::DAO_LP2;
    }
    else if (ui->buttonDAOMode->checkedButton()->objectName() == "radioDaoSP")
    {
        mode = DAO_Mode::DAO_SP;
    }

    return mode;
}

//--------------------------------------------------------------------------
//! @brief      tell if TOC manipulation is supported
//!
//! @param      tm  support flag
//--------------------------------------------------------------------------
void CDaoConfDlg::tocManip(bool tm)
{
    mTocManip = tm;
    if (!mTocManip)
    {
        ui->radioDaoSP->setEnabled(false);
    }
    else
    {
        ui->radioDaoSP->setChecked(true);
    }
}

