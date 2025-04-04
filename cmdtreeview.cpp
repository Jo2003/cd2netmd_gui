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
#include "cmdtreeview.h"
#include "cmdtreemodel.h"
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include "cnamingdialog.h"
#include "defines.h"

CMDTreeView::CMDTreeView(QWidget *parent)
    :QTreeView(parent)
{
    createActions();
}

void CMDTreeView::resizeEvent(QResizeEvent *e)
{
    QTreeView::resizeEvent(e);
    setColumnWidth(0, (width() / 100) * 80);
    setColumnWidth(1, (width() / 100) * 5);
    setColumnWidth(2, (width() / 100) * 10);
}

void CMDTreeView::contextMenuEvent(QContextMenuEvent *event)
{
    if (model() != nullptr)
    {
        QMenu menu(this);
        menu.addAction(mpaAddGroup);
        menu.addAction(mpaDelGroup);
        menu.addAction(mpaDelTrack);
        menu.addAction(mpaEraseDisc);
        menu.addSeparator();
        menu.addAction(mpaExportTilteList);
        menu.exec(event->globalPos());
    }
}

void CMDTreeView::createActions()
{
    mpaAddGroup = new QAction(tr("&Add Group"), this);
    mpaAddGroup->setShortcut({"Alt+Ctrl+A"});
    mpaAddGroup->setStatusTip(tr("Add Tracks to a new Group."));
    connect(mpaAddGroup, &QAction::triggered, this, &CMDTreeView::slotAddGroup);

    mpaDelGroup = new QAction(tr("&Delete Group"), this);
    mpaDelGroup->setShortcut({"Alt+Ctrl+D"});
    mpaDelGroup->setStatusTip(tr("Delete the group, but the tracks."));
    connect(mpaDelGroup, &QAction::triggered, this, &CMDTreeView::slotDelGroup);

    mpaDelTrack = new QAction(tr("Delete &Track"), this);
    mpaDelTrack->setShortcut({"Alt+Ctrl+T"});
    mpaDelTrack->setStatusTip(tr("Delete a tracks. This can't be undone."));
    connect(mpaDelTrack, &QAction::triggered, this, &CMDTreeView::slotDelTrack);

    mpaEraseDisc = new QAction(tr("Erase D&isc"), this);
    mpaEraseDisc->setShortcut({"Alt+Ctrl+I"});
    mpaEraseDisc->setStatusTip(tr("Erase the Disc. This can't be undone."));
    connect(mpaEraseDisc, &QAction::triggered, this, &CMDTreeView::slotEraseDisc);

    mpaExportTilteList = new QAction(tr("&Export Title List"), this);
    mpaEraseDisc->setShortcut({"Alt+Ctrl+E"});
    mpaEraseDisc->setStatusTip(tr("Export Title List"));
    connect(mpaExportTilteList, &QAction::triggered, this, &CMDTreeView::slotExportTitles);
}

void CMDTreeView::slotDelTrack()
{
    qDebug("%s", __FUNCTION__);
    QModelIndexList selection = selectedIndexes();
    try
    {
        if (selection.size() == model()->columnCount())
        {
            CTreeItem *item = static_cast<CTreeItem*>(selection.at(0).internalPointer());
            if (item->itemRole() != CMDTreeModel::ItemRole::TRACK)
            {
                throw tr("Please select a track for deletion!");
            }
            qDebug("Delete track No. %d", item->trackNumber() - 1);
            emit delTrack(static_cast<int16_t>(item->trackNumber() - 1));
        }
        else
        {
            throw tr("Please select one track only for deletion!");
        }
    }
    catch (const QString& err)
    {
        QMessageBox::information(this, tr("Note!"), err);
    }
}

void CMDTreeView::slotDelGroup()
{
    qDebug("%s", __FUNCTION__);
    QModelIndexList selection = selectedIndexes();
    try
    {
        if (selection.size() == model()->columnCount())
        {
            CTreeItem *item = static_cast<CTreeItem*>(selection.at(0).internalPointer());
            if (item->itemRole() != CMDTreeModel::ItemRole::GROUP)
            {
                throw tr("Please select a group for deletion!");
            }
            qDebug("Delete group No. %d", item->trackNumber() - 1);
            emit delGroup(static_cast<int16_t>(item->trackNumber() - 1));
        }
        else
        {
            throw tr("Please select one group only for deletion!");
        }
    }
    catch (const QString& err)
    {
        QMessageBox::information(this, tr("Note!"), err);
    }
}

void CMDTreeView::slotEraseDisc()
{
    qDebug("%s", __FUNCTION__);
    emit eraseDisc();
}

void CMDTreeView::slotAddGroup()
{
    qDebug("%s", __FUNCTION__);
    QModelIndexList selection = selectedIndexes();
    try
    {
        if (!(selection.size() % model()->columnCount()))
        {
            CTreeItem *item = nullptr;

            // make sure we only have tracks selected
            for (const auto& i : selection)
            {
                if (i.column() == 0)
                {
                    item = static_cast<CTreeItem*>(i.internalPointer());
                    if (item->itemRole() != CMDTreeModel::ItemRole::TRACK)
                    {
                        throw tr("Please select tracks only!");
                    }
                }
            }

            // check that track isn't in a group already
            CMDTreeModel *pModel = static_cast<CMDTreeModel*>(model());
            for (const auto& i : selection)
            {
                if (i.column() == 0)
                {
                    item = static_cast<CTreeItem*>(i.internalPointer());
                    if (!pModel->group(item->trackNumber()).empty())
                    {
                        throw tr("Please select ungrouped tracks only!");
                    }
                }
            }

            int16_t first = -1;
            int16_t last  = -1;

            for (const auto& i : selection)
            {
                if (i.column() == 0)
                {
                    item = static_cast<CTreeItem*>(i.internalPointer());
                    if (first == -1)
                    {
                        first = item->trackNumber();
                    }
                    else if (item->trackNumber() < first)
                    {
                        first = item->trackNumber();
                    }

                    if (last == -1)
                    {
                        last = item->trackNumber();
                    }
                    else if (item->trackNumber() > last)
                    {
                        last = item->trackNumber();
                    }
                }
            }

            CNamingDialog *pName = new CNamingDialog(this, tr("Group Naming"), tr("Group Name to add: "));
            QString grpName;

            if (pName)
            {
                if (pName->exec() == QDialog::Accepted)
                {
                    grpName = pName->name();
                }
                delete pName;
            }
            emit addGroup(grpName, first, last);
        }
    }
    catch (const QString& err)
    {
        QMessageBox::information(this, tr("Note!"), err);
    }
}

//--------------------------------------------------------------------------
//! @brief      grab disc conent
//!
//! @param[in,out]  content stores title content
//! @param[in]       pModel pointer to QAbstractItemModel
//! @param[in]       parent parent index (optional)
//--------------------------------------------------------------------------
void CMDTreeView::treeWalk(DiscContent& content, QAbstractItemModel *pModel, QModelIndex parent)
{
    for(int r = 0; r < pModel->rowCount(parent); ++r)
    {
        QStringList row;
        for (int c = 0; c < pModel->columnCount(parent); c++)
        {
            QModelIndex index = pModel->index(r, c, parent);
            QString text = pModel->data(index).toString();

            if (!text.isEmpty())
            {
                row << text;
            }
        }
        content.append(row);

        if(pModel->hasChildren(pModel->index(r, 0, parent)))
        {
            treeWalk(content, pModel, pModel->index(r, 0, parent));
        }
    }
}

//--------------------------------------------------------------------------
//! @brief      export title list action activated
//--------------------------------------------------------------------------
void CMDTreeView::slotExportTitles()
{
    DiscContent titles;
    treeWalk(titles, model());
    int len = 0;

    // format
    for (const auto& l : titles)
    {
        if (l.count() > 1)
        {
            if (l.at(0).size() > len)
            {
                len = l.at(0).size();
            }
        }
    }

    QString s;
    QTextStream ts(&s);

    for (const auto& l : titles)
    {
        if (l.count() == 1)
        {
            ts << l.at(0) << Qt::endl;
            ts << QString("%1").arg("~", -l.at(0).size(), QChar('~')) << Qt::endl;
        }
        else if (l.count() > 1)
        {
            for (int i = 0; i < l.count(); i++)
            {
                if (i == 0)
                {
                    ts << QString("%1").arg(l.at(i), -len, QChar(' '));
                }
                else
                {
                    ts << QString(" %1").arg(l.at(i));
                }
            }
            ts << Qt::endl;
        }
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Title File"),
        QStandardPaths::locate(QStandardPaths::DocumentsLocation, ".", QStandardPaths::LocateDirectory),
        tr("Plain Text (*.txt)"));

    if (!fileName.isEmpty())
    {
        QFile f(fileName);
        if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            f.write(s.toUtf8());
        }
    }
}
