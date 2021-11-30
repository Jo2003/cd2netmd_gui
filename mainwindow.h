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
#include <QMainWindow>
#include "cjacktheripper.h"
#include "ccddb.h"
#include "ccddbentriesdialog.h"
#include "ccditemmodel.h"
#include "cnetmd.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void catchCDDBEntries(QStringList l);
    void catchCDDBEntry(QStringList l);
    void catchJson(QString);

    void on_pushInitCD_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    CJackTheRipper *mpRipper;
    CNetMD         *mpNetMD;
};
