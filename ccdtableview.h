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
#include <QObject>
#include <QTableView>
#include <QResizeEvent>
#include "defines.h"

//------------------------------------------------------------------------------
//! @brief      cd table visualization of the data that model contains.
//------------------------------------------------------------------------------
class CCDTableView : public QTableView
{
    Q_OBJECT
public:
    //--------------------------------------------------------------------------
    //! @brief      create / init instance
    //--------------------------------------------------------------------------
    CCDTableView(QWidget *parent = nullptr);

protected:
    //--------------------------------------------------------------------------
    //! @brief      resize columns when size was changed
    //!
    //! @param      e     pointer to resize event
    //--------------------------------------------------------------------------
    void resizeEvent(QResizeEvent *e) override;

    //--------------------------------------------------------------------------
    //! @brief      drag event enters the widget
    //!
    //! @param      event pointer to QDragEnterEvent
    //--------------------------------------------------------------------------
    void dragEnterEvent(QDragEnterEvent *event) override;

    //--------------------------------------------------------------------------
    //! @brief      drop event
    //!
    //! @param      event pointer to QDropEvent
    //--------------------------------------------------------------------------
    void dropEvent(QDropEvent *event) override;

    //--------------------------------------------------------------------------
    //! @brief      drag move event
    //!
    //! @param      event pointer to QDragMoveEvent
    //--------------------------------------------------------------------------
    void dragMoveEvent(QDragMoveEvent *event) override;

    //--------------------------------------------------------------------------
    //! @brief      key press event
    //!
    //! @param      event pointer to QKeyEvent
    //--------------------------------------------------------------------------
    void keyPressEvent(QKeyEvent *event) override;

signals:
    //--------------------------------------------------------------------------
    //! @brief      signal that files where dropped into the widget
    //!
    //! @param      files string list with file names
    //--------------------------------------------------------------------------
    void filesDropped(QStringList files);

    //--------------------------------------------------------------------------
    //! @brief      signal that entries should be deleted from list
    //!
    //! @param      selection vector with selected rows
    //--------------------------------------------------------------------------
    void deleteSelected(QVector<int> selection);

};
