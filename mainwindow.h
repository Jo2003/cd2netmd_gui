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
#include <QTemporaryFile>
#include <QDir>
#include <QLabel>
#include <QMovie>
#include "cjacktheripper.h"
#include "ccddb.h"
#include "ccddbentriesdialog.h"
#include "ccditemmodel.h"
#include "cnetmd.h"
#include "cxenc.h"
#include "cmdtreemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum class WorkStep : uint8_t
    {
        NONE,       // steady
        RIP,        // transient
        RIPPED,     // steady
        ENCODE,     // transient
        ENCODED,    // steady
        TRANSFER,   // transient
        DONE        // steady
    };

    struct SRipTrack
    {
        int16_t         mCDTrackNo;
        QString         mTitle;
        QTemporaryFile* mpFile;
        time_t          mLength;
        WorkStep        mStep;
    };

    using TransferQueue = QVector<SRipTrack>;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void transferConfig(CNetMD::NetMDCmd& netMdCmd, CXEnc::XEncCmd& xencCmd, QString& trackMode);
    void addMDTrack(int number, const QString &title, const QString &mode, time_t length);
    void addMDGroup(const QString& title, int16_t first, int16_t last);
    void setMDTitle(const QString& title);
    void enableDialogItems(bool ena);
    void recreateTreeView(const QString& json);
    void countLabel(QLabel *pLabel, WorkStep step, const QString& text);

private slots:
    void catchCDDBEntries(QStringList l);
    void catchCDDBEntry(QStringList l);
    void catchJson(QString);

    void on_pushInitCD_clicked();
    void on_pushLoadMD_clicked();

    void mdTitling(CMDTreeModel::ItemRole role, QString title, int no);

    void on_pushTransfer_clicked();

    void ripFinished();
    void encodeFinished(bool checkBusy = false);
    void transferFinished(bool checkBusy = false);

    void on_pushEraseMD_clicked();

private:
    Ui::MainWindow *ui;
    CJackTheRipper *mpRipper;
    CNetMD         *mpNetMD;
    CXEnc          *mpXEnc;
    CMDTreeModel   *mpMDmodel;
    TransferQueue   mWorkQueue;
    QMovie         *mpWaitAni;
};
