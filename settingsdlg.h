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
#pragma once

#include <QDialog>
#include <QMovie>

namespace Ui {
class SettingsDlg;
}


//------------------------------------------------------------------------------
//! @brief      This class describes a settings dialog.
//------------------------------------------------------------------------------
class SettingsDlg : public QDialog
{
    Q_OBJECT

public:

    /// current theme
    enum Theme {
        LIGHT,
        DARK,
        STANDARD
    };

    /// supported read speeds
    static const int READ_SPEEDS[];

    /// config structure for cdparanoia
    struct SParanoia
    {
        int mReadSpeed;     ///< read speed
        bool mEnaParanoia;  ///< enable paranoia mode
    };

    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param      parent  The parent pointer
    //--------------------------------------------------------------------------
    explicit SettingsDlg(QWidget *parent = nullptr);
    
    //--------------------------------------------------------------------------
    //! @brief      Destroys the object.
    //--------------------------------------------------------------------------
    ~SettingsDlg();

    //--------------------------------------------------------------------------
    //! @brief      is CD paranoia mode enabled
    //!
    //! @return     pointer to paranoia config
    //--------------------------------------------------------------------------
    const SParanoia* paranoia();
    
    //--------------------------------------------------------------------------
    //! @brief      is on-the-fly-transfer enabled
    //!
    //! @param[in]  statusOnly get checked status only (optional)
    //!
    //! @return     true if enabled
    //--------------------------------------------------------------------------
    bool onthefly(bool statusOnly = false) const;
    
    //--------------------------------------------------------------------------
    //! @brief      set / enable / diable on-the-fly checkbox
    //!
    //! @param[in]  check  The checked value
    //! @param[in]  ena    The enaable value
    //--------------------------------------------------------------------------
    void enaDisaOtf(bool check, bool ena);

    //--------------------------------------------------------------------------
    //! @brief      set / enable / diable device reset checkbox
    //!
    //! @param[in]  check  The checked value
    //! @param[in]  ena    The enaable value
    //--------------------------------------------------------------------------
    void enaDisaDevReset(bool check, bool ena);

    //--------------------------------------------------------------------------
    //! @brief      reset device after TOC edit
    //!
    //! @param[in]  statusOnly get checked status only (optional)
    //!
    //! @return     true if enabled
    //--------------------------------------------------------------------------
    bool devReset(bool statusOnly = false) const;
    
    //--------------------------------------------------------------------------
    //! @brief      gets wait animation (theme dependent)
    //!
    //! @return     pointer to wait animation movie
    //--------------------------------------------------------------------------
    QMovie* waitAni();

    //--------------------------------------------------------------------------
    //! @brief      write MD title after SP transfer
    //!
    //! @return     true if enabled
    //--------------------------------------------------------------------------
    bool spMdTitle() const;

    //--------------------------------------------------------------------------
    //! @brief      create track group after LP transfer
    //!
    //! @return     true if enabled
    //--------------------------------------------------------------------------
    bool lpTrackGroup() const;

    //--------------------------------------------------------------------------
    //! @brief      do CDDB request
    //!
    //! @return     true if enabled
    //--------------------------------------------------------------------------
    bool cddb() const;

    //--------------------------------------------------------------------------
    //! @brief      get active theme
    //!
    //! @return     active theme
    //--------------------------------------------------------------------------
    Theme theme() const;

    //--------------------------------------------------------------------------
    //! @brief      get location of at3tool (if there)
    //!
    //! @return     file path to at3tool
    //--------------------------------------------------------------------------
    QString at3tool() const;

    //--------------------------------------------------------------------------
    //! @brief      do size check
    //!
    //! @return     true if enabled
    //--------------------------------------------------------------------------
    bool sizeCheck() const;

    //--------------------------------------------------------------------------
    //! @brief      don't add artist names to track titles
    //!
    //! @return     true if enabled
    //--------------------------------------------------------------------------
    bool noArtistInTitle() const;

private slots:
    //--------------------------------------------------------------------------
    //! @brief      get path to at3tool
    //--------------------------------------------------------------------------
    void on_pushAtTool_clicked();
    
    //--------------------------------------------------------------------------
    //! @brief      Called when combo box current index changed.
    //!
    //! @param[in]  index  The index
    //--------------------------------------------------------------------------
    void on_comboBox_currentIndexChanged(int index);
    
    //--------------------------------------------------------------------------
    //! @brief      Loads settings.
    //--------------------------------------------------------------------------
    void loadSettings();

    //--------------------------------------------------------------------------
    //! @brief      Called when cbx log level current index changed.
    //!
    //! @param[in]  index  The index
    //--------------------------------------------------------------------------
    void on_cbxLogLevel_currentIndexChanged(int index);

    //--------------------------------------------------------------------------
    //! @brief      Called when push cleanup clicked.
    //--------------------------------------------------------------------------
    void on_pushCleanup_clicked();

    //--------------------------------------------------------------------------
    //! @brief      Called when push OK clicked.
    //--------------------------------------------------------------------------
    void on_pushOK_clicked();

signals:
    //--------------------------------------------------------------------------
    //! @brief      will be sent when settings loaded
    //--------------------------------------------------------------------------
    void loadingComplete();

private:
    /// pointer to UI
    Ui::SettingsDlg *ui;

    /// busy animation
    QMovie          *mpWaitAni;

    /// cdparanoia settings
    SParanoia mParanoia{8, false};
};
