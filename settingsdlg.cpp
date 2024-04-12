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
#include "settingsdlg.h"
#include "ui_settingsdlg.h"
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QTimer>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include "defines.h"

const int SettingsDlg::READ_SPEEDS[] = {1, 2, 4, 8, 12, 16};

SettingsDlg::SettingsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDlg), mpWaitAni(nullptr)
{
    ui->setupUi(this);
    mpWaitAni = new QMovie(":/light/wait", QByteArray(), this);
    QTimer::singleShot(10, this, &SettingsDlg::loadSettings);
}

SettingsDlg::~SettingsDlg()
{
    QSettings set;
    set.setValue("theme", ui->comboBox->currentIndex());
    set.setValue("loglevel", ui->cbxLogLevel->currentIndex());
    set.setValue("paranoia", ui->checkParanoia->isChecked());
    set.setValue("otf", ui->checkOTFEnc->isChecked());
    set.setValue("sp_title", ui->checkSPTitle->isChecked());
    set.setValue("lp_group", ui->checkLPGroup->isChecked());
    set.setValue("cddb", ui->checkCDDB->isChecked());
    set.setValue("at3tool", ui->lineAtTool->text());
    set.setValue("dev_reset", ui->checkDevReset->isChecked());
    set.setValue("read_speed", ui->comboReadSpeed->currentIndex());
    delete ui;
}

const SettingsDlg::SParanoia* SettingsDlg::paranoia()
{
    mParanoia = {
        READ_SPEEDS[ui->comboReadSpeed->currentIndex()],
        ui->checkParanoia->isChecked()
    };
    return &mParanoia;
}

//--------------------------------------------------------------------------
//! @brief      is on-the-fly-transfer enabled
//!
//! @param[in]  statusOnly get checked status only (optional)
//!
//! @return     true if enabled
//--------------------------------------------------------------------------
bool SettingsDlg::onthefly(bool statusOnly) const
{
    if (statusOnly)
    {
        return ui->checkOTFEnc->isChecked();
    }
    else
    {
        return ui->checkOTFEnc->isChecked() && ui->checkOTFEnc->isEnabled();
    }
}

void SettingsDlg::enaDisaOtf(bool check, bool ena)
{
    ui->checkOTFEnc->setChecked(check);
    ui->checkOTFEnc->setEnabled(ena);
}

//--------------------------------------------------------------------------
//! @brief      set / enable / diable device reset checkbox
//!
//! @param[in]  check  The checked value
//! @param[in]  ena    The enaable value
//--------------------------------------------------------------------------
void SettingsDlg::enaDisaDevReset(bool check, bool ena)
{
    ui->checkDevReset->setChecked(check);
    ui->checkDevReset->setEnabled(ena);
}

//--------------------------------------------------------------------------
//! @brief      reset device after TOC edit
//!
//! @param[in]  statusOnly get checked status only (optional)
//!
//! @return     true if enabled
//--------------------------------------------------------------------------
bool SettingsDlg::devReset(bool statusOnly) const
{
    if (statusOnly)
    {
        return ui->checkDevReset->isChecked();
    }
    else
    {
        return ui->checkDevReset->isChecked() && ui->checkDevReset->isEnabled();
    }
}

QMovie *SettingsDlg::waitAni()
{
    return mpWaitAni;
}

bool SettingsDlg::spMdTitle() const
{
    return ui->checkSPTitle->isChecked();
}

bool SettingsDlg::lpTrackGroup() const
{
    return ui->checkLPGroup->isChecked();
}

bool SettingsDlg::cddb() const
{
    return ui->checkCDDB->isChecked();
}

//--------------------------------------------------------------------------
//! @brief      get active theme
//!
//! @return     active theme
//--------------------------------------------------------------------------
SettingsDlg::Theme SettingsDlg::theme() const
{
    switch(ui->comboBox->currentIndex())
    {
    case 1:
        return Theme::DARK;
    case 2:
        return Theme::LIGHT;
    default:
        return Theme::STANDARD;
    }
}

//--------------------------------------------------------------------------
//! @brief      get location of at3tool (if there)
//!
//! @return     file path to at3tool
//--------------------------------------------------------------------------
QString SettingsDlg::at3tool() const
{
    if (QFile::exists(ui->lineAtTool->text()))
    {
        return ui->lineAtTool->text();
    }
    return "";
}

//--------------------------------------------------------------------------
//! @brief      do size check
//!
//! @return     true if enabled
//--------------------------------------------------------------------------
bool SettingsDlg::sizeCheck() const
{
    return !ui->disSzCheck->isChecked();
}

void SettingsDlg::on_comboBox_currentIndexChanged(int index)
{
    QFile styleFile;

    if (mpWaitAni)
    {
        delete mpWaitAni;
    }

    if (index == 1) // dark
    {
        styleFile.setFileName(":qdarkstyle/dark/darkstyle.qss");
        mpWaitAni = new QMovie(":/dark/wait", QByteArray(), this);
    }
    else if (index == 2) // light
    {
        styleFile.setFileName(":qdarkstyle/light/lightstyle.qss");
        mpWaitAni = new QMovie(":/light/wait", QByteArray(), this);
    }
    else
    {
        qApp->setStyleSheet("");
        mpWaitAni = new QMovie(":/light/wait", QByteArray(), this);
        return;
    }

    if (!styleFile.exists())
    {
        qWarning() << "Unable to set stylesheet, file not found!";
    }
    else
    {
        styleFile.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&styleFile);
        qApp->setStyleSheet(ts.readAll());
    }
}

void SettingsDlg::loadSettings()
{
    QSettings set;
    if (set.contains("theme"))
    {
        ui->comboBox->setCurrentIndex(set.value("theme").toUInt());
    }

    if (set.contains("paranoia"))
    {
        ui->checkParanoia->setChecked(set.value("paranoia").toBool());
    }

    if (set.contains("otf"))
    {
        ui->checkOTFEnc->setChecked(set.value("otf").toBool());
    }

    if (set.contains("dev_reset"))
    {
        ui->checkDevReset->setChecked(set.value("dev_reset").toBool());
    }

    if (set.contains("loglevel"))
    {
        ui->cbxLogLevel->setCurrentIndex(set.value("loglevel").toUInt());
        g_LogFilter = static_cast<c2n::LogLevel>(set.value("loglevel").toUInt());
    }
    else
    {
        ui->cbxLogLevel->setCurrentIndex(static_cast<int>(c2n::LogLevel::INFO));
    }

    if (set.contains("sp_title"))
    {
        ui->checkSPTitle->setChecked(set.value("sp_title").toBool());
    }
    else
    {
        ui->checkSPTitle->setChecked(true);
    }

    if (set.contains("lp_group"))
    {
        ui->checkLPGroup->setChecked(set.value("lp_group").toBool());
    }
    else
    {
        ui->checkLPGroup->setChecked(true);
    }

    if (set.contains("cddb"))
    {
        ui->checkCDDB->setChecked(set.value("cddb").toBool());
    }
    else
    {
        ui->checkCDDB->setChecked(true);
    }

    if (set.contains("at3tool"))
    {
        ui->lineAtTool->setText(set.value("at3tool").toString());
    }

    if (set.contains("read_speed"))
    {
        ui->comboReadSpeed->setCurrentIndex(set.value("read_speed").toUInt());
    }
    else
    {
        // 8x
        ui->comboReadSpeed->setCurrentIndex(3);
    }

    emit loadingComplete();
}

void SettingsDlg::on_cbxLogLevel_currentIndexChanged(int index)
{
    g_LogFilter = static_cast<c2n::LogLevel>(index);
}

void SettingsDlg::on_pushCleanup_clicked()
{
    QDir tempDir(QDir::temp());
    QStringList tmpFiles = tempDir.entryList(QStringList() << "cd2netmd.*" << "cd2netmd_flac_decode*.*", QDir::Files);

    for (const auto& fName : tmpFiles)
    {
        QString compName = QString("%1/%2").arg(tempDir.path()).arg(fName);
        qInfo() << "Removing lost temp file" << compName;
        QFile::remove(compName);
    }
    qInfo() << "Temp file cleanup dome!";
}

void SettingsDlg::on_pushOK_clicked()
{
    accept();
}

void SettingsDlg::on_pushAtTool_clicked()
{
#ifdef Q_OS_WIN
    QString fileName = QFileDialog::getOpenFileName(this, tr("Get alternate ATRAC3 encoder"), "", tr("Program File (*.exe)"));
#else
    QString fileName = QFileDialog::getOpenFileName(this, tr("Get alternate ATRAC3 encoder"), "", tr("Program File (*)"));
#endif
    if (!fileName.isEmpty())
    {
        ui->lineAtTool->setText(fileName);
    }
}

