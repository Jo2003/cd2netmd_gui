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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QStringListModel>
#include <QJsonDocument>
#include <QJsonObject>

#include "cmdtreemodel.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), mpRipper(nullptr), mpNetMD(nullptr)
{
    ui->setupUi(this);

    if ((mpRipper = new CJackTheRipper(this)) != nullptr)
    {
        connect(mpRipper, &CJackTheRipper::progress, ui->progressBar, &QProgressBar::setValue);
        connect(mpRipper->cddb(), &CCDDB::entries, this, &MainWindow::catchCDDBEntries);
        connect(mpRipper->cddb(), &CCDDB::match, this, &MainWindow::catchCDDBEntry);
        connect(mpRipper, &CJackTheRipper::match, this, &MainWindow::catchCDDBEntry);
    }

    if ((mpNetMD = new CNetMD(this)) != nullptr)
    {
        connect(mpNetMD, &CNetMD::jsonOut, this, &MainWindow::catchJson);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::catchCDDBEntries(QStringList l)
{
    CCDDBEntriesDialog* pDlg = new CCDDBEntriesDialog(l, this);
    if (pDlg->exec() == QDialog::Accepted)
    {
        mpRipper->cddb()->getEntry(pDlg->request());
    }
    delete pDlg;
}

void MainWindow::catchCDDBEntry(QStringList l)
{
    CCDItemModel::TrackTimes v = mpRipper->trackTimes();

    // no CDDB result
    if (l.isEmpty())
    {
        l.append("<untitled disc>");
        for (const auto& t : v)
        {
            Q_UNUSED(t)
            l.append("<untitled track>");
        }
    }

    if (l.size() > 0)
    {
        ui->lineCDTitle->setText(l.at(0));
    }

    l.removeFirst();

    CCDItemModel *pModel = static_cast<CCDItemModel *>(ui->tableViewCD->model());

    if (pModel != nullptr)
    {
        delete pModel;
    }

    pModel = new CCDItemModel(l, v, this);

    ui->tableViewCD->setModel(pModel);
    int width = ui->tableViewCD->width();

    ui->tableViewCD->setColumnWidth(0, (width / 100) * 80);
    ui->tableViewCD->setColumnWidth(1, (width / 100) * 15);
}

void MainWindow::catchJson(QString j)
{
    CMDTreeModel* model = static_cast<CMDTreeModel*>(ui->treeView->model());
    if (model != nullptr)
    {
        delete model;
    }

    model = new CMDTreeModel(j, this);
    ui->treeView->setModel(model);

    ui->treeView->expandAll();

    ui->treeView->setColumnWidth(0, (ui->treeView->width() / 100) * 80);
    ui->treeView->setColumnWidth(1, (ui->treeView->width() / 100) * 5);
    ui->treeView->setColumnWidth(2, (ui->treeView->width() / 100) * 10);
}

void MainWindow::on_pushInitCD_clicked()
{
    mpRipper->init();
}

void MainWindow::on_pushButton_clicked()
{
    mpNetMD->start({CNetMD::NetMDCmd::DISCINFO});
}
