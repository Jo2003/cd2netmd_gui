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
#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class CCDDBEntriesDialog;
}

//------------------------------------------------------------------------------
//! @brief      This class describes a ccddb entries dialog.
//------------------------------------------------------------------------------
class CCDDBEntriesDialog : public QDialog
{
    Q_OBJECT

public:
    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param[in]  entries  The entries
    //! @param      parent   The parent
    //--------------------------------------------------------------------------
    explicit CCDDBEntriesDialog(const QStringList& entries, QWidget *parent = nullptr);
    
    //--------------------------------------------------------------------------
    //! @brief      Destroys the object.
    //--------------------------------------------------------------------------
    ~CCDDBEntriesDialog();
    
    //--------------------------------------------------------------------------
    //! @brief      get request
    //!
    //! @return     The request string.
    //--------------------------------------------------------------------------
    QString request();

protected:

    //--------------------------------------------------------------------------
    //! @brief      to do when dialog is shown
    //!
    //! @param      e pointer to show event
    //--------------------------------------------------------------------------
    void showEvent(QShowEvent *e) override;
    
    QStringList mEntries; ///< buffer entries
    QString mRequest;     ///< buffer request

private slots:
    //--------------------------------------------------------------------------
    //! @brief      Called when button box clicked.
    //!
    //! @param      button  The clicked button
    //--------------------------------------------------------------------------
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::CCDDBEntriesDialog *ui;
};
