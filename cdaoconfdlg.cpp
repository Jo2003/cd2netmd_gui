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
    ui(new Ui::CDaoConfDlg), mSPUpload(false)
{
    ui->setupUi(this);
    QString md = R"(
### (D)isc (A)t (O)nce or Gapless Mode ###

DAO / gapless is supported in 2 modes. Both have there pros and cons.

+ __DAO LP2 Mode:__ Disc / input will be extracted and compressed at once. After that audio will be split in tracks and transfered to NetMD.
  + __Pro:__ Track information will be transfered as well.
  + __Contra:__ Quality loss due to LP2 mode and external encoder. Playback only on MDLP capable devices.
+ __DAO SP Mode:__ Disc / input will be extracted and compressed at once. After that audio will be split in tracks and transfered to NetMD.
  + __Pro:__ Best quality. Playback on all MD devices. Track information will be transfered as well.
  + __Contra:__ This is only supported on __Sony Type S devices with firmware revision 1.2 or 1.6__.

> Please note: Any change on CD track list will be reverted before starting!
)";
    ui->textBrowser->setMarkdown(md);
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

    if (!mSPUpload)
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
//! @brief      tell if SP uload is supported
//!
//! @param      spu  support flag
//--------------------------------------------------------------------------
void CDaoConfDlg::spUpload(bool spu)
{
    mSPUpload = spu;
    if (!mSPUpload)
    {
        ui->radioDaoSP->setEnabled(false);
    }
    else
    {
        ui->radioDaoSP->setChecked(true);
    }
}

