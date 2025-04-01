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
#include "cdaoconfdlg.h"
#include "statuswidget.h"
#include "transfermode.h"

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

    enum ePopUp
    {
        INFORMATION,
        WARNING,
        CRITICAL
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

    //--------------------------------------------------------------------------
    //! @brief      get ms value for CDDA block count
    //!
    //! @param      blonks  CDDA block count
    //!
    //! @return     time in milliseconds
    //--------------------------------------------------------------------------
    static inline uint32_t blocksToMs(long blocks)
    {
        return std::round((static_cast<double>(blocks) * 1000.0) / static_cast<double>(CDIO_CD_FRAMES_PER_SEC));
    }

protected:

    //--------------------------------------------------------------------------
    //! @brief      app is about to close
    //!
    //! @param[in]  e pointer to close event
    //--------------------------------------------------------------------------
    void closeEvent(QCloseEvent* e) override;

    //--------------------------------------------------------------------------
    //! @brief      something changed
    //!
    //! @param[in]  e pointer to event
    //--------------------------------------------------------------------------
    void changeEvent(QEvent* e) override;

    //--------------------------------------------------------------------------
    //! @brief      Adds a md track.
    //!
    //! @param[in]  number  The number
    //! @param[in]  title   The title
    //! @param[in]  length  The length
    //--------------------------------------------------------------------------
    void addMDTrack(int number, const QString &title, double length);
    
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

    //--------------------------------------------------------------------------
    //! @brief      revert track order changes for DAO
    //--------------------------------------------------------------------------
    void revertCDEntries();

    //--------------------------------------------------------------------------
    //! @brief      show a popup message delayed (asynchronous)
    //!
    //! @param      tp      Popup type
    //! @param[in]  caption window title
    //! @param[in]  msg     message text
    //! @param[in]  wait    delay time in ms
    //--------------------------------------------------------------------------
    void delayedPopUp(ePopUp tp, const QString& caption, const QString& msg, int wait = 500);

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
    //! @param[in]  tracks audio tracks vector
    //--------------------------------------------------------------------------
    void catchCDDBEntry(c2n::AudioTracks tracks);
    
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
    //! @param[in]  ret        if given return value from last transfer
    //--------------------------------------------------------------------------
    void transferFinished(bool checkBusy, int ret);
    
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
    //! @brief      Called when push about clicked.
    //--------------------------------------------------------------------------
    void on_pushAbout_clicked();

    //--------------------------------------------------------------------------
    //! @brief      Called when push settings clicked.
    //--------------------------------------------------------------------------
    void on_pushSettings_clicked();

    //--------------------------------------------------------------------------
    //! @brief      Called when load image button was clicked.
    //--------------------------------------------------------------------------
    void on_pushLoadImg_clicked();

    //--------------------------------------------------------------------------
    //! @brief      catch dropped files from cd table view
    //!
    //! @param[in]  sl  string list with file pathes
    //--------------------------------------------------------------------------
    void catchDropped(QStringList sl);

    //--------------------------------------------------------------------------
    //! @brief      catch new audio length in list
    //!
    //! @param      length in blocks
    //--------------------------------------------------------------------------
    void audioLength(long blocks);

    //--------------------------------------------------------------------------
    //! @brief      parse cue file
    //!
    //! @param[in]  fileName cue sheet file name
    //--------------------------------------------------------------------------
    int parseCueFile(QString fileName);

    void on_pushHelp_clicked();

    void on_pushLog_clicked();

    //--------------------------------------------------------------------------
    //! @brief      transfer mode changed
    //!
    //! @param[in]  index new activated index
    //--------------------------------------------------------------------------
    void on_cbxTranferMode_currentIndexChanged(int index);

    //--------------------------------------------------------------------------
    //! @brief      update free time label
    //--------------------------------------------------------------------------
    void updateFreeTimeLabel();

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
    StatusWidget   *mpMDDevice;
    
    /// CD device info
    StatusWidget   *mpCDDevice;
    
    /// settings dialog class
    SettingsDlg    *mpSettings;

    /// keep a clean backup of the tracks
    c2n::AudioTracks mTracksBackup;

    /// can the device SP upload
    bool mSpUpload;

    /// does the device support forced TOC edit
    bool mTocManip;

    /// does the device support pcm2Mono
    bool mPcm2Mono;

    /// show SP download support
    StatusWidget   *mpSpUpload;

    /// show on-the-fly support
    StatusWidget   *mpOtfEncode;

    /// show TOC manipulation support
    StatusWidget   *mpTocManip;

    /// show SP Mono support
    StatusWidget   *mpPcm2Mono;

    /// chosen transfer mode
    TransferMode mTransferMode;
};
