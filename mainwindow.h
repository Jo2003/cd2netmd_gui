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
#pragma once
#include <QMainWindow>
#include <QTemporaryFile>
#include <QDir>
#include <QLabel>
#include <QMovie>
#include "cjacktheripper.h"
#include "ccddb.h"
#include "ccddbentriesdialog.h"
#include "ccditemmodel.h"
#include "cnetmd.h"
#include "cxenc.h"
#include "cmdtreemodel.h"
#include "defines.h"
#include "caboutdialog.h"
#include "settingsdlg.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//------------------------------------------------------------------------------
//! @brief      This class describes a main window.
//------------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
    Q_OBJECT
    using TransferQueue = c2n::TransferQueue;
    using WorkStep      = c2n::WorkStep;

    enum eDiscFlags
    {
        WRITEABLE  = (1 << 4),
        WRITE_LOCK = (1 << 6)
    };

public:
    //--------------------------------------------------------------------------
    //! @brief      creates the object
    //!
    //! @param      parent  The parent
    //--------------------------------------------------------------------------
    MainWindow(QWidget *parent = nullptr);

    //--------------------------------------------------------------------------
    //! @brief      Destroys the object.
    //--------------------------------------------------------------------------
    ~MainWindow();

protected:

    //--------------------------------------------------------------------------
    //! @brief      app is about to close
    //!
    //! @param[in]  e pointer to clode event
    //--------------------------------------------------------------------------
    void closeEvent(QCloseEvent* e) override;

    //--------------------------------------------------------------------------
    //! @brief      get the transfer configuration
    //!
    //! @param[out] netMdCmd   The net md command
    //! @param[out] xencCmd    The xenc command
    //! @param[out] trackMode  The track mode
    //--------------------------------------------------------------------------
    void transferConfig(CNetMD::NetMDCmd& netMdCmd, CXEnc::XEncCmd& xencCmd, QString& trackMode);
    
    //--------------------------------------------------------------------------
    //! @brief      Adds a md track.
    //!
    //! @param[in]  number  The number
    //! @param[in]  title   The title
    //! @param[in]  mode    The mode
    //! @param[in]  length  The length
    //--------------------------------------------------------------------------
    void addMDTrack(int number, const QString &title, const QString &mode, time_t length);
    
    //--------------------------------------------------------------------------
    //! @brief      Sets the md title.
    //!
    //! @param[in]  title  The title
    //--------------------------------------------------------------------------
    void setMDTitle(const QString& title);
    
    //--------------------------------------------------------------------------
    //! @brief      Enables the dialog items.
    //!
    //! @param[in]  ena   enable or disable
    //--------------------------------------------------------------------------
    void enableDialogItems(bool ena);
    
    //--------------------------------------------------------------------------
    //! @brief      recreate MD content tree view
    //!
    //! @param[in]  json  The json
    //--------------------------------------------------------------------------
    void recreateTreeView(const QString& json);
    
    //--------------------------------------------------------------------------
    //! @brief      update count label
    //!
    //! @param      pLabel  The label
    //! @param[in]  step    The step
    //! @param[in]  text    The text
    //--------------------------------------------------------------------------
    void countLabel(QLabel *pLabel, WorkStep step, const QString& text);

private slots:
    //--------------------------------------------------------------------------
    //! @brief      load settings
    //--------------------------------------------------------------------------
    void loadSettings();

    //--------------------------------------------------------------------------
    //! @brief      get the list of matching CDDB entries
    //!
    //! @param[in]  l     entries
    //--------------------------------------------------------------------------
    void catchCDDBEntries(QStringList l);
    
    //--------------------------------------------------------------------------
    //! @brief      get the one matching CDDB entry
    //!
    //! @param[in]  l     track titles
    //--------------------------------------------------------------------------
    void catchCDDBEntry(QStringList l);
    
    //--------------------------------------------------------------------------
    //! @brief      get json data from MD
    //!
    //! @param[in]  <unnamed>  json data as string
    //--------------------------------------------------------------------------
    void catchJson(QString);

    //--------------------------------------------------------------------------
    //! @brief      Called when push initialize cd clicked.
    //--------------------------------------------------------------------------
    void on_pushInitCD_clicked();
    
    //--------------------------------------------------------------------------
    //! @brief      Called when push load md clicked.
    //--------------------------------------------------------------------------
    void on_pushLoadMD_clicked();

    //--------------------------------------------------------------------------
    //! @brief      MD titling
    //!
    //! @param[in]  role   The role
    //! @param[in]  title  The title
    //! @param[in]  no     number
    //--------------------------------------------------------------------------
    void mdTitling(CMDTreeModel::ItemRole role, QString title, int no);

    //--------------------------------------------------------------------------
    //! @brief      Called when push transfer clicked.
    //--------------------------------------------------------------------------
    void on_pushTransfer_clicked();

    //--------------------------------------------------------------------------
    //! @brief      rip of one track finsihed
    //--------------------------------------------------------------------------
    void ripFinished();
    
    //--------------------------------------------------------------------------
    //! @brief      one encode finished.
    //!
    //! @param[in]  checkBusy  The check busy
    //--------------------------------------------------------------------------
    void encodeFinished(bool checkBusy = false);
    
    //--------------------------------------------------------------------------
    //! @brief      one transfer finished
    //!
    //! @param[in]  checkBusy  The check busy
    //--------------------------------------------------------------------------
    void transferFinished(bool checkBusy = false);
    
    //--------------------------------------------------------------------------
    //! @brief      Adds a md group.
    //!
    //! @param[in]  title  The title
    //! @param[in]  first  The first
    //! @param[in]  last   The last
    //--------------------------------------------------------------------------
    void addMDGroup(const QString& title, int16_t first, int16_t last);
    
    //--------------------------------------------------------------------------
    //! @brief      delete one MD group
    //!
    //! @param[in]  number  The group number
    //--------------------------------------------------------------------------
    void delMDGroup(int16_t number);
    
    //--------------------------------------------------------------------------
    //! @brief      delete track
    //!
    //! @param[in]  <unnamed>  track number
    //--------------------------------------------------------------------------
    void delTrack(int16_t);
    
    //--------------------------------------------------------------------------
    //! @brief      erase MD
    //--------------------------------------------------------------------------
    void eraseDisc();

    //--------------------------------------------------------------------------
    //! @brief      Called when push dao clicked.
    //--------------------------------------------------------------------------
    void on_pushDAO_clicked();

    //--------------------------------------------------------------------------
    //! @brief      Called when push about clicked.
    //--------------------------------------------------------------------------
    void on_pushAbout_clicked();

    //--------------------------------------------------------------------------
    //! @brief      Called when push settings clicked.
    //--------------------------------------------------------------------------
    void on_pushSettings_clicked();

private:
    /// GUI pointer
    Ui::MainWindow *ui;

    /// CD Ripper pointer
    CJackTheRipper *mpRipper;
    
    /// NetMD handling pointer
    CNetMD         *mpNetMD;
    
    /// external encoder pointer
    CXEnc          *mpXEnc;
    
    /// tree model for MD
    CMDTreeModel   *mpMDmodel;
    
    /// job list
    TransferQueue   mWorkQueue;
    
    /// MD device info
    QLabel         *mpMDDevice;
    
    /// CD device info
    QLabel         *mpCDDevice;
    
    /// is DAO handling active?
    bool            mbDAO;

    /// settings dialog class
    SettingsDlg    *mpSettings;
};
