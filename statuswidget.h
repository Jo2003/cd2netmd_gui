/**
 * Copyright (C) 2023 Jo2003 (olenka.joerg@gmail.com)
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

#include <QWidget>
#include <QString>

namespace Ui {
class StatusWidget;
}

//------------------------------------------------------------------------------
//! @brief      This class describes a status widget.
//------------------------------------------------------------------------------
class StatusWidget : public QWidget
{
    Q_OBJECT

public:

    ///
    /// \brief construct StatusWidget
    /// \param parent widget
    /// \param icon string
    /// \param text string
    /// \param stip string status tip
    /// \param iconSz size in pixel
    ///
    explicit StatusWidget(QWidget *parent = nullptr,
                          const QString& icon = "",
                          const QString& text = "",
                          const QString& stip = "",
                          int iconSz = 16);

    ///
    /// \brief destroy object
    ///
    ~StatusWidget();

    ///
    /// \brief setText
    /// \param text string
    ///
    void setText(const QString& text);

    ///
    /// \brief setStatusTip
    /// \param tip string
    ///
    void setStatusTip(const QString& tip);

    ///
    /// \brief setIcon
    /// \param icon string
    ///
    void setIcon(const QString& icon);

private:
    Ui::StatusWidget *ui;
    int mScale;
    QString mStatusTip;
};
