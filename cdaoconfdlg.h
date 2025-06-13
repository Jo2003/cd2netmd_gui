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
#include "settingsdlg.h"

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
    //! @brief      Constructs a new instance.
    //!
    //! @param      parent  The parent widget
    //! @param      set     pointer to settingsdialog
    //--------------------------------------------------------------------------
    explicit CDaoConfDlg(QWidget *parent, SettingsDlg* set);
    
    //--------------------------------------------------------------------------
    //! @brief      Destroys the object.
    //--------------------------------------------------------------------------
    ~CDaoConfDlg();

private slots:
    void on_checkDontShow_toggled(bool checked);

private:
    Ui::CDaoConfDlg *ui;
    SettingsDlg *pSet;
};

