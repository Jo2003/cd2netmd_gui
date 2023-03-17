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

namespace Ui {
class CDaoConfDlg;
}


//------------------------------------------------------------------------------
//! @brief      This class describes the dao config dialog.
//------------------------------------------------------------------------------
class CDaoConfDlg : public QDialog
{
    Q_OBJECT

public:
    //--------------------------------------------------------------------------
    //! @brief      DAO modes
    //--------------------------------------------------------------------------
    enum DAO_Mode
    {
        DAO_SP,     ///< SP mode
        DAO_LP2,    ///< LP2 mode
        DAO_WTF     ///< wtf ...
    };

    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param      parent  The parent widget
    //--------------------------------------------------------------------------
    explicit CDaoConfDlg(QWidget *parent = nullptr);
    
    //--------------------------------------------------------------------------
    //! @brief      Destroys the object.
    //--------------------------------------------------------------------------
    ~CDaoConfDlg();

    //--------------------------------------------------------------------------
    //! @brief      give dao mode
    //!
    //! @return     The dao mode.
    //--------------------------------------------------------------------------
    DAO_Mode daoMode() const;

    //--------------------------------------------------------------------------
    //! @brief      tell if SP uload is supported
    //!
    //! @param      spu  support flag
    //--------------------------------------------------------------------------
    void spUpload(bool spu);

private:
    Ui::CDaoConfDlg *ui;
    bool mSPUpload;
};

