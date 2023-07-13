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
#include <QTreeView>
#include <QResizeEvent>
#include <QContextMenuEvent>
#include <QAction>

//------------------------------------------------------------------------------
//! @brief      md tree visualization of the data that model contains.
//------------------------------------------------------------------------------
class CMDTreeView : public QTreeView
{
    Q_OBJECT
public:
    /// data type for disc content
    using DiscContent = QVector<QStringList>;

    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param      parent  The parent
    //--------------------------------------------------------------------------
    explicit CMDTreeView(QWidget* parent);

protected:
    //--------------------------------------------------------------------------
    //! @brief      scale columns on size change
    //!
    //! @param      e     resize event
    //--------------------------------------------------------------------------
    void resizeEvent(QResizeEvent *e) override;
    
    //--------------------------------------------------------------------------
    //! @brief      context menu was activated
    //!
    //! @param      event  The event
    //--------------------------------------------------------------------------
    void contextMenuEvent(QContextMenuEvent *event) override;
    
    //--------------------------------------------------------------------------
    //! @brief      Creates actions belonging to context menu
    //--------------------------------------------------------------------------
    void createActions();

    //--------------------------------------------------------------------------
    //! @brief      grab disc conent
    //!
    //! @param[in,out]  content stores title content
    //! @param[in]       pModel pointer to QAbstractItemModel
    //! @param[in]       parent parent index (optional)
    //--------------------------------------------------------------------------
    void treeWalk(DiscContent& content, QAbstractItemModel *pModel, QModelIndex parent = QModelIndex());

signals:
    //--------------------------------------------------------------------------
    //! @brief      signal about track deletion
    //!
    //! @param[in]  <unnamed>  track number
    //--------------------------------------------------------------------------
    void delTrack(int16_t);
    
    //--------------------------------------------------------------------------
    //! @brief      signal about group deletion
    //!
    //! @param[in]  <unnamed>  group number
    //--------------------------------------------------------------------------
    void delGroup(int16_t);
    
    //--------------------------------------------------------------------------
    //! @brief      signal about disc erase
    //--------------------------------------------------------------------------
    void eraseDisc();
    
    //--------------------------------------------------------------------------
    //! @brief      Adds a group.
    //!
    //! @param[in]  <unnamed>  group name
    //! @param[in]  <unnamed>  first group title
    //! @param[in]  <unnamed>  last group title
    //--------------------------------------------------------------------------
    void addGroup(QString, int16_t, int16_t);

private slots:
    //--------------------------------------------------------------------------
    //! @brief      delete track action activated
    //--------------------------------------------------------------------------
    void slotDelTrack();
    
    //--------------------------------------------------------------------------
    //! @brief      delete group action activated
    //--------------------------------------------------------------------------
    void slotDelGroup();

    //--------------------------------------------------------------------------
    //! @brief      erase disc action activated
    //--------------------------------------------------------------------------
    void slotEraseDisc();

    //--------------------------------------------------------------------------
    //! @brief      add group action activated
    //--------------------------------------------------------------------------
    void slotAddGroup();

    //--------------------------------------------------------------------------
    //! @brief      export title list action activated
    //--------------------------------------------------------------------------
    void slotExportTitles();

private:
    /// add group action
    QAction* mpaAddGroup;
    
    /// del group action
    QAction* mpaDelGroup;
    
    /// del track action
    QAction* mpaDelTrack;

    /// erase disc action
    QAction* mpaEraseDisc;

    /// export title list
    QAction* mpaExportTilteList;
};
