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

using namespace c2n;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), mpRipper(nullptr),
      mpNetMD(nullptr), mpXEnc(nullptr), mpMDmodel(nullptr),
      mpWaitAni(nullptr), mbDAO(false)
{
    ui->setupUi(this);

    if ((mpRipper = new CJackTheRipper(this)) != nullptr)
    {
        connect(mpRipper, &CJackTheRipper::progress, ui->progressRip, &QProgressBar::setValue);
        connect(mpRipper->cddb(), &CCDDB::entries, this, &MainWindow::catchCDDBEntries);
        connect(mpRipper->cddb(), &CCDDB::match, this, &MainWindow::catchCDDBEntry);
        connect(mpRipper, &CJackTheRipper::match, this, &MainWindow::catchCDDBEntry);
        connect(mpRipper, &CJackTheRipper::finished, this, &MainWindow::ripFinished);
    }

    if ((mpNetMD = new CNetMD(this)) != nullptr)
    {
        connect(mpNetMD, &CNetMD::progress, ui->progressMDTransfer, &QProgressBar::setValue);
        connect(mpNetMD, &CNetMD::jsonOut, this, &MainWindow::catchJson);
        connect(mpNetMD, &CNetMD::finished, this, &MainWindow::transferFinished);
    }

    if ((mpXEnc = new CXEnc(this)) != nullptr)
    {
        connect(mpXEnc, &CXEnc::progress, ui->progressExtEnc, &QProgressBar::setValue);
        connect(mpXEnc, &CXEnc::fileDone, this, &MainWindow::encodeFinished);
    }
    mpWaitAni = new QMovie(":/main/wait", QByteArray(), this);

    connect(ui->treeView, &CMDTreeView::addGroup, this, &MainWindow::addMDGroup);
    connect(ui->treeView, &CMDTreeView::delGroup, this, &MainWindow::delMDGroup);
    connect(ui->treeView, &CMDTreeView::eraseDisc, this, &MainWindow::eraseDisc);
    connect(ui->treeView, &CMDTreeView::delTrack, this, &MainWindow::delTrack);

    mpMDDevice = new QLabel();
    mpCDDevice = new QLabel();

    ui->statusbar->addPermanentWidget(mpCDDevice);
    ui->statusbar->addPermanentWidget(mpMDDevice);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::transferConfig(CNetMD::NetMDCmd &netMdCmd, CXEnc::XEncCmd &xencCmd, QString &trackMode)
{
    netMdCmd  = CNetMD::NetMDCmd::WRITE_TRACK_SP;
    xencCmd   = CXEnc::XEncCmd::NONE;
    trackMode = "SP";

    if (mbDAO)
    {
        netMdCmd  = CNetMD::NetMDCmd::WRITE_TRACK_SP;
        xencCmd   = CXEnc::XEncCmd::DAO_LP2_ENCODE;
        trackMode = "LP2";
    }
    else if (ui->radioGroup->checkedButton()->objectName() == "radioLP2")
    {
        if (ui->checkOTFEnc->isChecked())
        {
            netMdCmd  = CNetMD::NetMDCmd::WRITE_TRACK_LP2;
        }
        else
        {
            xencCmd   = CXEnc::XEncCmd::LP2_ENCODE;
        }
        trackMode = "LP2";
    }
    else if (ui->radioGroup->checkedButton()->objectName() == "radioLP4")
    {
        if (ui->checkOTFEnc->isChecked())
        {
            netMdCmd  = CNetMD::NetMDCmd::WRITE_TRACK_LP4;
        }
        else
        {
            xencCmd   = CXEnc::XEncCmd::LP4_ENCODE;
        }
        trackMode = "LP4";
    }
}

void MainWindow::catchCDDBEntries(QStringList l)
{
    CCDDBEntriesDialog* pDlg = new CCDDBEntriesDialog(l, this);
    if (pDlg->exec() == QDialog::Accepted)
    {
        mpRipper->cddb()->getEntry(pDlg->request());
        delete pDlg;
        return;
    }
    catchCDDBEntry(QStringList{});
}

void MainWindow::catchCDDBEntry(QStringList l)
{
    CCDItemModel::TrackTimes v = mpRipper->trackTimes();
    time_t length = mpRipper->discLength();

    ui->labCDTime->clear();
    ui->labCDTime->setText(tr("Disc Time: %1:%2:%3").arg(length / 3600, 1, 10, QChar('0'))
                           .arg((length % 3600) / 60, 2, 10, QChar('0'))
                           .arg(length % 60, 2, 10, QChar('0')));

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

    mpCDDevice->clear();
    mpCDDevice->setText(mpRipper->deviceInfo());
    mpCDDevice->show();
    enableDialogItems(true);
}

void MainWindow::catchJson(QString j)
{
    recreateTreeView(j);
    if (mpMDmodel->discConf()->mOTFEnc == 0)
    {
        ui->checkOTFEnc->setChecked(false);
        ui->checkOTFEnc->setEnabled(false);
    }
    else
    {
        ui->checkOTFEnc->setEnabled(true);
        ui->checkOTFEnc->setChecked(true);
    }
    enableDialogItems(true);
}

void MainWindow::on_pushInitCD_clicked()
{
    enableDialogItems(false);
    mpRipper->init();
}

void MainWindow::on_pushLoadMD_clicked()
{
    enableDialogItems(false);
    mpNetMD->start({CNetMD::NetMDCmd::DISCINFO});
}

void MainWindow::mdTitling(CMDTreeModel::ItemRole role, QString title, int no)
{
    qDebug("Role: %d, Title: %s, Number: %d", static_cast<int>(role), static_cast<const char*>(title.toUtf8()), no);
    switch(role)
    {
    case CMDTreeModel::ItemRole::DISC:
        mpNetMD->start({CNetMD::NetMDCmd::RENAME_DISC, "", title});
        break;
    case CMDTreeModel::ItemRole::TRACK:
        mpNetMD->start({CNetMD::NetMDCmd::RENAME_TRACK, title, "", "", static_cast<int16_t>(no)});
        break;
    case CMDTreeModel::ItemRole::GROUP:
        mpNetMD->start({CNetMD::NetMDCmd::RENAME_GROUP, "", "", title, -1, -1, static_cast<int16_t>(no)});
        break;
    default:
        break;
    }
}

void MainWindow::on_pushTransfer_clicked()
{
    enableDialogItems(false);
    mbDAO = false;
    QModelIndexList selected = ui->tableViewCD->selectionModel()->selectedRows();

    // no selection means all!
    if (selected.isEmpty())
    {
        ui->tableViewCD->selectAll();
        selected = ui->tableViewCD->selectionModel()->selectedRows();
    }

    time_t selectionTime = 0;
    mWorkQueue.clear();

    // Multiple rows can be selected
    for(const auto& r : selected)
    {
        int16_t trackNo    = r.row() + 1;
        QString trackTitle = r.data().toString();
        time_t  trackTime  = r.sibling(r.row(), 1).data(Qt::UserRole).toLongLong();
        selectionTime += trackTime;
        mWorkQueue.append({trackNo, trackTitle, new QTemporaryFile(QDir::tempPath() + "/cd2netmd.XXXXXX.tmp"), trackTime, WorkStep::NONE});
    }

    if (ui->radioGroup->checkedButton()->objectName() == "radioLP2")
    {
        selectionTime /= 2;
    }
    else if (ui->radioGroup->checkedButton()->objectName() == "radioLP4")
    {
        selectionTime /= 4;
    }

    if (selectionTime > mpMDmodel->discConf()->mFreeTime)
    {
        // not enough space left on device
        time_t need = selectionTime - mpMDmodel->discConf()->mFreeTime;
        QString t = QString("%1:%2:%3").arg(need / 3600).arg((need % 3600) / 60, 2, 10, QChar('0')).arg(need % 60, 2, 10, QChar('0'));
        QMessageBox::warning(this, tr("Error"), tr("Not enough space left on MD to transfer CD title. You need %1 more.").arg(t));
        mWorkQueue.clear();
        enableDialogItems(true);
    }
    else if (!mWorkQueue.isEmpty())
    {
        for (auto& j : mWorkQueue)
        {
            j.mpFile->open();
            j.mpFile->close();
        }
        ripFinished();
    }
}

void MainWindow::ripFinished()
{
    CNetMD::NetMDCmd netMdCmd;
    CXEnc::XEncCmd   xencCmd;
    QString          trackMode;
    transferConfig(netMdCmd, xencCmd, trackMode);

    bool noEnc = xencCmd == CXEnc::XEncCmd::NONE;

    if (mbDAO)
    {
        if (mWorkQueue.at(0).mStep == WorkStep::RIP)
        {
            mWorkQueue[0].mStep = WorkStep::RIPPED;
        }

        if (mWorkQueue.at(0).mStep == WorkStep::NONE)
        {
            mWorkQueue[0].mStep = WorkStep::RIP;
            ui->progressRip->setValue(0);
            mpRipper->extractTrack(-1, mWorkQueue.at(0).mpFile->fileName(), ui->checkParanoia->isChecked());
        }
    }
    else
    {
        for (auto& j : mWorkQueue)
        {
            if (j.mStep == WorkStep::RIP)
            {
                j.mStep = noEnc ? WorkStep::ENCODED : WorkStep::RIPPED;
                break;
            }
        }

        for (auto& j : mWorkQueue)
        {
            if (j.mStep == WorkStep::NONE)
            {
                j.mStep = WorkStep::RIP;
                ui->progressRip->setValue(0);
                mpRipper->extractTrack(j.mCDTrackNo, j.mpFile->fileName(), ui->checkParanoia->isChecked());
                break;
            }
        }
    }

    countLabel(ui->labelCDRip, WorkStep::NONE, tr("CD-RIP"));

    // do we need encoding?
    if (noEnc)
    {
        // no encoding
        transferFinished(true);
    }
    else
    {
        // encoding
        encodeFinished(true);
    }
}

void MainWindow::encodeFinished(bool checkBusy)
{
    if (!checkBusy || !mpXEnc->busy())
    {
        CNetMD::NetMDCmd netMdCmd;
        CXEnc::XEncCmd   xencCmd;
        QString          trackMode;
        transferConfig(netMdCmd, xencCmd, trackMode);

        if (mbDAO)
        {
            if (mWorkQueue.at(0).mStep == WorkStep::ENCODE)
            {
                // atracdenc always misses 100% ;)
                ui->progressExtEnc->setValue(100);

                // mark all tracks as encoded
                for (auto& j : mWorkQueue)
                {
                    j.mStep = WorkStep::ENCODED;
                }
                transferFinished(true);
                return;
            }

            if (mWorkQueue.at(0).mStep == WorkStep::RIPPED)
            {
                mWorkQueue[0].mStep = WorkStep::ENCODE;
                ui->progressExtEnc->setValue(0);
                mpXEnc->start(xencCmd, mWorkQueue, mpRipper->discLength());
            }
        }
        else
        {
            for (auto& j : mWorkQueue)
            {
                if (j.mStep == WorkStep::ENCODE)
                {
                    j.mStep = WorkStep::ENCODED;

                    // atracdenc always misses 100% ;)
                    ui->progressExtEnc->setValue(100);
                    break;
                }
            }

            for (auto& j : mWorkQueue)
            {
                if (j.mStep == WorkStep::RIPPED)
                {
                    j.mStep = WorkStep::ENCODE;
                    ui->progressExtEnc->setValue(0);
                    mpXEnc->start(xencCmd, j.mpFile->fileName(), j.mLength);
                    break;
                }
            }
        }

        countLabel(ui->labelExtEnc, WorkStep::RIPPED, tr("External-Encoder"));
        transferFinished(true);
    }
}

void MainWindow::transferFinished(bool checkBusy)
{
    if ((!checkBusy || !mpNetMD->busy()) && !mWorkQueue.isEmpty())
    {
        CNetMD::NetMDCmd netMdCmd;
        CXEnc::XEncCmd   xencCmd;
        QString          trackMode;
        transferConfig(netMdCmd, xencCmd, trackMode);

        for (auto& j : mWorkQueue)
        {
            if (j.mStep == WorkStep::TRANSFER)
            {
                j.mStep = WorkStep::DONE;
                addMDTrack(mpMDmodel->discConf()->mTrkCount, j.mTitle, trackMode, j.mLength);
                break;
            }
        }

        for (auto& j : mWorkQueue)
        {
            if (j.mStep == WorkStep::ENCODED)
            {
                j.mStep = WorkStep::TRANSFER;
                ui->progressMDTransfer->setValue(0);
                mpNetMD->start({netMdCmd, j.mpFile->fileName(), j.mTitle});
                break;
            }
        }

        int dc = 0;
        for (auto& j : mWorkQueue)
        {
            if (j.mStep == WorkStep::DONE)
            {
                dc ++;
            }
        }

        countLabel(ui->labMDTransfer, WorkStep::ENCODED, tr("MD-Transfer"));

        if (dc == mWorkQueue.size())
        {
            if (trackMode != "SP")
            {
                addMDGroup(ui->lineCDTitle->text(),
                           static_cast<int16_t>(mpMDmodel->discConf()->mTrkCount - mWorkQueue.size() + 1),
                           static_cast<int16_t>(mpMDmodel->discConf()->mTrkCount));
            }
            else
            {
                // set disc title
                mpNetMD->start({CNetMD::NetMDCmd::RENAME_DISC, "", ui->lineCDTitle->text()});
                setMDTitle(ui->lineCDTitle->text());
            }
            for (auto& j : mWorkQueue)
            {
                delete j.mpFile;
            }
            mWorkQueue.clear();
            enableDialogItems(true);
            QMessageBox::information(this, tr("Success"), tr("All (selected) tracks are transfered to MiniDisc!"));
        }
    }
}

void MainWindow::addMDTrack(int number, const QString &title, const QString &mode, time_t length)
{
    QString timeString = QString("%1:%2:%3")
            .arg((length % 3600) / 60, 2, 10, QChar('0'))
            .arg(length % 60, 2, 10, QChar('0'))
            .arg("00"); // fake frames

    time_t tUsed = length;

    if (mode == "LP2")
    {
        tUsed /= 2;
    }
    else if (mode == "LP4")
    {
        tUsed /= 4;
    }

    nlohmann::json mdJson = mpMDmodel->exportJson();

    nlohmann::json trk;
    trk["no"]      = number;
    trk["name"]    = title.toStdString();
    trk["bitrate"] = mode.toStdString();
    trk["time"]    = timeString.toStdString();
    mdJson["tracks"].push_back(trk);

    mdJson["trk_count"] = mdJson["trk_count"].get<int>() + 1;
    mdJson["t_used"]    = mdJson["t_used"].get<int>() + length;
    mdJson["t_free"]    = mdJson["t_free"].get<int>() - tUsed;

    recreateTreeView(QString::fromStdString(mdJson.dump()));
}

void MainWindow::addMDGroup(const QString &title, int16_t first, int16_t last)
{
    mpNetMD->start({CNetMD::NetMDCmd::ADD_GROUP, "", "", title, first, last});

    nlohmann::json group;
    group["name"]  = title.toStdString();
    group["first"] = first;
    group["last"]  = first == last ? -1 : last;

    nlohmann::json mdJson = mpMDmodel->exportJson();
    mdJson["groups"].push_back(group);

    recreateTreeView(QString::fromStdString(mdJson.dump()));
}

void MainWindow::delMDGroup(int16_t number)
{
    CNetMD::NetMDStartup startUp(CNetMD::NetMDCmd::DEL_GROUP);
    startUp.miGroup = number + 1;
    mpNetMD->start(startUp);

    nlohmann::json mdJson = mpMDmodel->exportJson();

    nlohmann::json::iterator it;

    for (it = mdJson["groups"].begin(); it != mdJson["groups"].end(); it++)
    {
        if (it->at("no").get<int>() == number)
        {
            mdJson["groups"].erase(it);
            break;
        }
    }
    recreateTreeView(QString::fromStdString(mdJson.dump()));
}

void MainWindow::delTrack(int16_t track)
{
    if (QMessageBox::question(this, tr("Question"),
                              tr("Do you really want to delete MD track %1? This can't be undone!").arg(track + 1)) == QMessageBox::Yes)
    {
        enableDialogItems(false);
        CNetMD::NetMDStartup startUp(CNetMD::NetMDCmd::DEL_TRACK);
        startUp.miFirst = track;
        mpNetMD->start(startUp);
    }
}

void MainWindow::setMDTitle(const QString &title)
{
    nlohmann::json mdJson = mpMDmodel->exportJson();
    mdJson["title"] = title.toStdString();
    recreateTreeView(QString::fromStdString(mdJson.dump()));
}

void MainWindow::enableDialogItems(bool ena)
{
    if (!ena)
    {
        // reset progress bars and lables
        ui->progressRip->setValue(0);
        ui->progressExtEnc->setValue(0);
        ui->progressMDTransfer->setValue(0);

        ui->labelCDRip->clear();
        ui->labelExtEnc->clear();
        ui->labMDTransfer->clear();

        ui->labelCDRip->setText(tr("CD-RIP: "));
        ui->labelExtEnc->setText(tr("External-Encoder: "));
        ui->labMDTransfer->setText(tr("MD-Tranfer: "));

        ui->labAnimation->clear();
        ui->labAnimation->setMovie(mpWaitAni);
        mpWaitAni->start();
    }
    else
    {
        mpWaitAni->stop();
        ui->labAnimation->clear();
    }

    ui->tableViewCD->setEnabled(ena);
    ui->lineCDTitle->setEnabled(ena);
    ui->treeView->setEnabled(ena);
    ui->radioSP->setEnabled(ena);
    ui->radioLP2->setEnabled(ena);
    ui->radioLP4->setEnabled(ena);
    if (ena)
    {
        if ((mpMDmodel != nullptr) && mpMDmodel->discConf()->mOTFEnc)
        {
            ui->checkOTFEnc->setEnabled(ena);
        }
    }
    else
    {
        ui->checkOTFEnc->setEnabled(ena);
    }
    ui->pushLoadMD->setEnabled(ena);
    ui->pushInitCD->setEnabled(ena);
    ui->checkParanoia->setEnabled(ena);

    if (ena)
    {
        if ((ui->tableViewCD->model() != nullptr)
            && (ui->tableViewCD->model()->rowCount() > 0)
            && (ui->treeView->model() != nullptr)
            && (ui->treeView->model()->rowCount() > 0))
        {
            ui->pushTransfer->setEnabled(ena);
            ui->pushDAO->setEnabled(ena);
        }
    }
    else
    {
        ui->pushTransfer->setEnabled(ena);
        ui->pushDAO->setEnabled(ena);
    }
}

void MainWindow::recreateTreeView(const QString &json)
{
    mpMDmodel = static_cast<CMDTreeModel*>(ui->treeView->model());
    if (mpMDmodel != nullptr)
    {
        disconnect(mpMDmodel, &CMDTreeModel::editTitle, this, &MainWindow::mdTitling);
        delete mpMDmodel;
    }

    mpMDmodel = new CMDTreeModel(json, this);
    connect(mpMDmodel, &CMDTreeModel::editTitle, this, &MainWindow::mdTitling);

    ui->treeView->setModel(mpMDmodel);
    ui->treeView->expandAll();

    ui->treeView->setColumnWidth(0, (ui->treeView->width() / 100) * 80);
    ui->treeView->setColumnWidth(1, (ui->treeView->width() / 100) * 5);
    ui->treeView->setColumnWidth(2, (ui->treeView->width() / 100) * 10);

    ui->labFreeTime->clear();
    ui->labFreeTime->setText(tr("Free: %1:%2:%3")
                            .arg(mpMDmodel->discConf()->mFreeTime / 3600, 1, 10, QChar('0'))
                            .arg((mpMDmodel->discConf()->mFreeTime % 3600) / 60, 2, 10, QChar('0'))
                            .arg(mpMDmodel->discConf()->mFreeTime %  60, 2, 10, QChar('0')));
    ui->labFreeTime->show();

    mpMDDevice->clear();
    mpMDDevice->setText(mpMDmodel->discConf()->mDevice);
    mpMDDevice->show();
}

void MainWindow::countLabel(QLabel *pLabel, MainWindow::WorkStep step, const QString &text)
{
    int count = 0;
    for (auto& j : mWorkQueue)
    {
        if (static_cast<uint8_t>(j.mStep) > static_cast<uint8_t>(step))
        {
            count ++;
        }
    }
    int all = (mbDAO && ((step == MainWindow::WorkStep::NONE) || (step == MainWindow::WorkStep::RIPPED))) ? 1 : mWorkQueue.size();
    pLabel->clear();
    pLabel->setText(tr("%1: %2/%3").arg(text).arg(count).arg(all));
}

void MainWindow::eraseDisc()
{
    if (QMessageBox::question(this, tr("Question"), tr("Do you really want to erase the MD? This can't be undone!")) == QMessageBox::Yes)
    {
        enableDialogItems(false);
        mpNetMD->start({CNetMD::NetMDCmd::ERASE_DISC});
    }
}

void MainWindow::on_pushDAO_clicked()
{
    if (QMessageBox::question(this, tr("Question"), tr("Disc-at-Once mode only works with external encoder and LP2 mode. Do you want to start this process?")) != QMessageBox::Yes)
    {
        return;
    }
    enableDialogItems(false);
    mbDAO = true;

    ui->checkOTFEnc->setChecked(false);
    ui->radioLP2->setChecked(true);

    ui->tableViewCD->selectAll();
    QModelIndexList selected = ui->tableViewCD->selectionModel()->selectedRows();

    time_t selectionTime = 0;
    mWorkQueue.clear();

    // Multiple rows can be selected
    for(const auto& r : selected)
    {
        int16_t trackNo    = r.row() + 1;
        QString trackTitle = r.data().toString();
        time_t  trackTime  = r.sibling(r.row(), 1).data(Qt::UserRole).toLongLong();
        selectionTime += trackTime;
        mWorkQueue.append({trackNo, trackTitle, new QTemporaryFile(QDir::tempPath() + "/cd2netmd.XXXXXX.tmp"), trackTime, WorkStep::NONE});
    }

    // DAO supported only in LP2 mode with external encoder
    selectionTime /= 2;

    if (selectionTime > mpMDmodel->discConf()->mFreeTime)
    {
        // not enough space left on device
        time_t need = selectionTime - mpMDmodel->discConf()->mFreeTime;
        QString t = QString("%1:%2:%3").arg(need / 3600).arg((need % 3600) / 60, 2, 10, QChar('0')).arg(need % 60, 2, 10, QChar('0'));
        QMessageBox::warning(this, tr("Error"), tr("Not enough space left on MD to transfer CD title. You need %1 more.").arg(t));
        mWorkQueue.clear();
        enableDialogItems(true);
    }
    else if (!mWorkQueue.isEmpty())
    {
        for (auto& j : mWorkQueue)
        {
            j.mpFile->open();
            j.mpFile->close();
        }
        ripFinished();
    }
}

void MainWindow::on_pushAbout_clicked()
{
    CAboutDialog* pAbout = new CAboutDialog(this);
    pAbout->exec();
    delete pAbout;
    pAbout = nullptr;
}
