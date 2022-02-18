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
#include <QMimeData>
#include <QUrl>
#include <QStringList>
#include <QFileInfo>
#include "ccdtableview.h"

//--------------------------------------------------------------------------
//! @brief      create / init instance
//--------------------------------------------------------------------------
CCDTableView::CCDTableView(QWidget *parent)
    :QTableView(parent)
{
     setAcceptDrops(true);
     setDragEnabled(true);
     setDropIndicatorShown(true);
     viewport()->setAcceptDrops(true);
}

//--------------------------------------------------------------------------
//! @brief      resize columns when size was changed
//!
//! @param      e     pointer to resize event
//--------------------------------------------------------------------------
void CCDTableView::resizeEvent(QResizeEvent *e)
{
    QTableView::resizeEvent(e);
    setColumnWidth(0, (width() / 100) * 80);
    setColumnWidth(1, (width() / 100) * 15);
}

//--------------------------------------------------------------------------
//! @brief      drag event enters the widget
//!
//! @param      event pointer to QDragEnterEvent
//--------------------------------------------------------------------------
void CCDTableView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else
    {
        QTableView::dragEnterEvent(event);
    }
}

//--------------------------------------------------------------------------
//! @brief      drag move event
//!
//! @param      event pointer to QDragMoveEvent
//--------------------------------------------------------------------------
void CCDTableView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else
    {
        QTableView::dragMoveEvent(event);
    }
}

//--------------------------------------------------------------------------
//! @brief      key press event
//!
//! @param      event pointer to QKeyEvent
//--------------------------------------------------------------------------
void CCDTableView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
        {
            QVector<int> rows;
            for (const auto& sel : selectionModel()->selectedRows())
            {
                rows << sel.row();
            }

            std::sort(rows.begin(), rows.end(), std::greater<int>());

            for (auto r : rows)
            {
                model()->removeRow(r);
            }

            emit audioLength(myModel()->audioLength());
        }
        break;
    default:
        QTableView::keyPressEvent(event);
        break;
    }
}

//--------------------------------------------------------------------------
//! @brief      get correctly casted model
//--------------------------------------------------------------------------
CCDItemModel *CCDTableView::myModel()
{
    return reinterpret_cast<CCDItemModel*>(model());
}

//--------------------------------------------------------------------------
//! @brief      drop event
//!
//! @param      event pointer to QDropEvent
//--------------------------------------------------------------------------
void CCDTableView::dropEvent(QDropEvent *event)
{
    QStringList files;
    QFileInfo fi;
    for (const auto& u : event->mimeData()->urls())
    {
        fi.setFile(QUrl::fromPercentEncoding(u.toString(QUrl::PreferLocalFile).toUtf8()));
        files << fi.absoluteFilePath();
    }
    emit filesDropped(files);

    QTableView::dropEvent(event);
}
