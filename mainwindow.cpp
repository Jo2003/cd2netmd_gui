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
#include <QSettings>
#include <QTimer>
#include <QFileDialog>
#include <QDesktopServices>
#include "cueparser.h"
#include "helpers.h"

using namespace c2n;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), mpRipper(nullptr),
      mpNetMD(nullptr), mpXEnc(nullptr), mpMDmodel(nullptr),
      mDAOMode(CDaoConfDlg::DAO_WTF), mpSettings(nullptr), mSpUpload(false),
      mTocManip(false), /* mpSpUpload(nullptr),*/ mpOtfEncode(nullptr),
      mpTocManip(nullptr)
{
    ui->setupUi(this);

    setWindowTitle(PROGRAM_NAME);

    if ((mpSettings = new SettingsDlg(this)) != nullptr)
    {
        connect(mpSettings, &SettingsDlg::loadingComplete, this, &MainWindow::loadSettings);
    }

    if ((mpRipper = new CJackTheRipper(this)) != nullptr)
    {
        connect(mpRipper, &CJackTheRipper::progress, ui->progressRip, &QProgressBar::setValue);
        connect(mpRipper->cddb(), &CCDDB::entries, this, &MainWindow::catchCDDBEntries);
        connect(mpRipper->cddb(), &CCDDB::match, this, &MainWindow::catchCDDBEntry);
        connect(mpRipper, &CJackTheRipper::match, this, &MainWindow::catchCDDBEntry);
        connect(mpRipper, &CJackTheRipper::finished, this, &MainWindow::ripFinished);
        connect(mpRipper, &CJackTheRipper::parseCue, this, &MainWindow::parseCueFile);
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

    connect(ui->treeView, &CMDTreeView::addGroup, this, &MainWindow::addMDGroup);
    connect(ui->treeView, &CMDTreeView::delGroup, this, &MainWindow::delMDGroup);
    connect(ui->treeView, &CMDTreeView::eraseDisc, this, &MainWindow::eraseDisc);
    connect(ui->treeView, &CMDTreeView::delTrack, this, &MainWindow::delTrack);
    connect(ui->tableViewCD, &CCDTableView::filesDropped, this, &MainWindow::catchDropped);
    connect(ui->tableViewCD, &CCDTableView::audioLength, this, &MainWindow::audioLength);

    mpMDDevice  = new StatusWidget(this, ":main/md", tr("Please re-load MD"));
    mpCDDevice  = new StatusWidget(this, ":buttons/cd", tr("Please re-load CD"));
    // mpSpUpload  = new StatusWidget(this, ":label/red", tr("SP"), tr("Marker for SP download"));
    mpOtfEncode = new StatusWidget(this, ":label/red", tr("OTF"), tr("Marker for on-the-fly encoding"));
    mpTocManip  = new StatusWidget(this, ":label/red", tr("TOC"), tr("Marker for TOC manipulation"));

    ui->statusbar->addPermanentWidget(mpTocManip);
    // ui->statusbar->addPermanentWidget(mpSpUpload);
    ui->statusbar->addPermanentWidget(mpOtfEncode);
    ui->statusbar->addPermanentWidget(mpCDDevice);
    ui->statusbar->addPermanentWidget(mpMDDevice);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    QSettings set;
    set.setValue("mainwindow", geometry());
    QMainWindow::closeEvent(e);
}

//--------------------------------------------------------------------------
//! @brief      something changed
//!
//! @param[in]  e pointer to event
//--------------------------------------------------------------------------
void MainWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::StyleChange)
    {
        QString style;
        if (mpSettings->theme() == SettingsDlg::Theme::STANDARD)
        {
            style = styles::CD_TAB_STD;
        }
        else
        {
            style = styles::CD_TAB_STYLED;
        }
        ui->tableViewCD->setStyleSheet(style);
    }
    QMainWindow::changeEvent(e);
}

void MainWindow::transferConfig(CNetMD::NetMDCmd &netMdCmd, CXEnc::XEncCmd &xencCmd, QString &trackMode)
{
    netMdCmd  = CNetMD::NetMDCmd::WRITE_TRACK_SP;
    xencCmd   = CXEnc::XEncCmd::NONE;
    trackMode = "SP";

    if (mDAOMode == CDaoConfDlg::DAO_LP2)
    {
        netMdCmd  = CNetMD::NetMDCmd::WRITE_TRACK_SP;
        xencCmd   = CXEnc::XEncCmd::DAO_LP2_ENCODE;
        trackMode = "LP2";
    }
    // else if (mDAOMode == CDaoConfDlg::DAO_SP)
    // {
    //     xencCmd   = CXEnc::XEncCmd::DAO_SP_ENCODE;
    // }
    else if (ui->radioGroup->checkedButton()->objectName() == "radioLP2")
    {
        if (mpSettings->onthefly())
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
        if (mpSettings->onthefly())
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
        qInfo() << "Request CDDB match: " << pDlg->request();
        mpRipper->cddb()->getEntry(pDlg->request());
        delete pDlg;
        return;
    }

    c2n::AudioTracks trks = {{"Request Aborted!"}};
    catchCDDBEntry(trks);
}

//--------------------------------------------------------------------------
//! @brief      revert track order changes for DAO
//--------------------------------------------------------------------------
void MainWindow::revertCDEntries()
{
    c2n::AudioTracks trks = ui->tableViewCD->myModel()->audioTracks();

    // we grab the changed titel, but keep the original order
    for (const auto& t : trks)
    {
        for (auto& b : mTracksBackup)
        {
            if (t.mCDTrackNo == b.mCDTrackNo)
            {
                b.mTitle = t.mTitle;
                break;
            }
        }
    }

    // don't forget the disc title
    mTracksBackup[0].mTitle = ui->lineCDTitle->text();

    catchCDDBEntry(mTracksBackup);
}

//--------------------------------------------------------------------------
//! @brief      get the one matching CDDB entry
//!
//! @param[in]  tracks audio tracks vector
//--------------------------------------------------------------------------
void MainWindow::catchCDDBEntry(c2n::AudioTracks tracks)
{
    // remove all data tracks from model
    for (auto it = tracks.begin(); it != tracks.end();)
    {
        if (it->mTType == c2n::TrackType::DATA)
        {
            it = tracks.erase(it);
        }
        else
        {
            it ++;
        }
    }

    // backup tracks
    mTracksBackup = tracks;

    time_t length = tracks.isEmpty() ? 0 : (tracks.at(0).mLbCount / CDIO_CD_FRAMES_PER_SEC);

    ui->labCDTime->clear();
    ui->labCDTime->setText(tr("Disc Time: %1:%2:%3").arg(length / 3600, 1, 10, QChar('0'))
                           .arg((length % 3600) / 60, 2, 10, QChar('0'))
                           .arg(length % 60, 2, 10, QChar('0')));

    ui->lineCDTitle->setText(tracks.isEmpty() ? "<No Disc>" : tracks.at(0).mTitle);

    // remove disc entry
    if (!tracks.isEmpty())
    {
        tracks.removeFirst();
    }

    CCDItemModel *pModel = ui->tableViewCD->myModel();

    if (pModel != nullptr)
    {
        delete pModel;
    }

    pModel = new CCDItemModel(tracks, this);

    ui->tableViewCD->setModel(pModel);
    int width = ui->tableViewCD->width();

    ui->tableViewCD->setColumnWidth(0, (width / 100) * 80);
    ui->tableViewCD->setColumnWidth(1, (width / 100) * 18);

    mpCDDevice->setText(mpRipper->deviceInfo().isEmpty() ? tr("Please re-load CD") : mpRipper->deviceInfo());
    enableDialogItems(true);
}

void MainWindow::catchJson(QString j)
{
    recreateTreeView(j);
    bool otf = !!mpMDmodel->discConf()->mOTFEnc;

    mpSettings->enaDisaOtf(mpSettings->onthefly(true), otf);

    mTocManip = !!mpMDmodel->discConf()->mTocManip;
    mpSettings->enaDisaDevReset(mpSettings->devReset(true), mTocManip);

    mSpUpload = !!mpMDmodel->discConf()->mSPUpload;

    // support label ...
    // mpSpUpload->setStatusTip(mSpUpload ? tr("SP download supported by device") : tr("SP download not supported by device"));
    // mpSpUpload->setIcon(mSpUpload ? ":label/green" : ":label/red");

    mpOtfEncode->setStatusTip(otf ? tr("on-the-fly encoding supported by device") : tr("on-the-fly encoding not supported by device"));
    mpOtfEncode->setIcon(otf ? ":label/green" : ":label/red");

    mpTocManip->setStatusTip(mTocManip ? tr("TOC manipulation supported by device") : tr("TOC manipulation not supported by device"));
    mpTocManip->setIcon(mTocManip ? ":label/green" : ":label/red");

    if (!(mpMDmodel->discConf()->mDiscFlags & eDiscFlags::WRITEABLE))
    {
        // read only disc
        delayedPopUp(ePopUp::INFORMATION, tr("Information"), tr("The MD in your drive isn't writeable.\n"
                                                                "Replace it with a writeable disc and reload the MD!"));
    }
    else if (mpMDmodel->discConf()->mDiscFlags & eDiscFlags::WRITE_LOCK)
    {
        // write lock
        delayedPopUp(ePopUp::INFORMATION, tr("Information"), tr("The MD in your drive is write locked.\n"
                                                                "Remove the write lock and reload the MD!"));
    }

    enableDialogItems(true);
}

void MainWindow::on_pushInitCD_clicked()
{
    enableDialogItems(false);
    mpRipper->init(mpSettings->cddb());
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
    c2n::AudioTracks trks = ui->tableViewCD->myModel()->audioTracks();
    trks.prepend({ui->lineCDTitle->text(), "", "", 0, 0, ui->tableViewCD->myModel()->audioLength()});
    mpRipper->setAudioTracks(trks);
    bool isCD = (trks.listType() == c2n::AudioTracks::CD);
    mDAOMode = CDaoConfDlg::DAO_WTF;

    QModelIndexList selected = ui->tableViewCD->selectionModel()->selectedRows();

    // no selection or drag'n'drop mode means all!
    if (selected.isEmpty() || (trks.listType() == c2n::AudioTracks::FILES))
    {
        ui->tableViewCD->selectAll();
        selected = ui->tableViewCD->selectionModel()->selectedRows();
    }

    double selectionTime = 0;
    mWorkQueue.clear();

    // Multiple rows can be selected
    for(const auto& r : selected)
    {
        int16_t trackNo    = isCD ? trks.at(r.row() + 1).mCDTrackNo : (r.row() + 1);
        QString trackTitle = r.data().toString();
        double  trackTime  = r.sibling(r.row(), 1).data(Qt::UserRole).toDouble();
        selectionTime += trackTime;
        mWorkQueue.append({trackNo,
                           trackTitle,
                           QDir::tempPath() + tempFileName("/cd2netmd.XXXXXX.tmp"),
                           trackTime,
                           WorkStep::NONE,
                           isCD});
    }

    if (ui->radioGroup->checkedButton()->objectName() == "radioLP2")
    {
        selectionTime /= 2.0;
    }
    else if (ui->radioGroup->checkedButton()->objectName() == "radioLP4")
    {
        selectionTime /= 4.0;
    }

    if (mpSettings->sizeCheck() && (selectionTime > mpMDmodel->discConf()->mFreeTime))
    {
        // not enough space left on device
        time_t need = selectionTime - mpMDmodel->discConf()->mFreeTime;
        QString t = QString("%1:%2:%3").arg(need / 3600).arg((need % 3600) / 60, 2, 10, QChar('0')).arg(need % 60, 2, 10, QChar('0'));
        mWorkQueue.clear();
        enableDialogItems(true);
        delayedPopUp(ePopUp::WARNING, tr("Error"), tr("No space left on MD to transfer your selected titles. You need %1 more.").arg(t), 100);
    }
    else if (!mWorkQueue.isEmpty())
    {
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

    if ((mDAOMode == CDaoConfDlg::DAO_LP2)
        || (mDAOMode == CDaoConfDlg::DAO_SP))
    {
        if (mWorkQueue.at(0).mStep == WorkStep::RIP)
        {
            mWorkQueue[0].mStep = (mDAOMode == CDaoConfDlg::DAO_LP2) ? WorkStep::RIPPED : WorkStep::ENCODED;
        }

        if (mWorkQueue.at(0).mStep == WorkStep::NONE)
        {
            mWorkQueue[0].mStep = WorkStep::RIP;
            ui->progressRip->setValue(0);
            mpRipper->extractTrack(-1, mWorkQueue.at(0).mFileName, mpSettings->paranoia());
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
                mpRipper->extractTrack(j.mCDTrackNo, j.mFileName, mpSettings->paranoia());
                break;
            }
        }
    }

    countLabel(ui->labelCDRip, WorkStep::NONE, tr("CD-RIP"));

    // do we need encoding?
    if (noEnc)
    {
        // no encoding
        transferFinished(true, 0);
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

        if ((mDAOMode == CDaoConfDlg::DAO_LP2)
            || (mDAOMode == CDaoConfDlg::DAO_SP))
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
                transferFinished(true, 0);
                return;
            }

            if (mWorkQueue.at(0).mStep == WorkStep::RIPPED)
            {
                mWorkQueue[0].mStep = WorkStep::ENCODE;
                ui->progressExtEnc->setValue(0);
                mpXEnc->start(xencCmd, mWorkQueue,
                              static_cast<double>(ui->tableViewCD->myModel()->audioLength())
                              / static_cast<double>(CDIO_CD_FRAMES_PER_SEC),
                              mpSettings->at3tool());
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
                    mpXEnc->start(xencCmd, j.mFileName, j.mLength,
                                  mpSettings->at3tool());
                    break;
                }
            }
        }

        countLabel(ui->labelExtEnc, WorkStep::RIPPED, tr("External-Encoder"));
        transferFinished(true, 0);
    }
}

//--------------------------------------------------------------------------
//! @brief      one transfer finished
//!
//! @param[in]  checkBusy  The check busy
//! @param[in]  ret        if given return value from last transfer
//--------------------------------------------------------------------------
void MainWindow::transferFinished(bool checkBusy, int ret)
{
    qInfo() << "checkBusy:" << checkBusy << "ret:" << ret;
    if ((!checkBusy || !mpNetMD->busy()) && !mWorkQueue.isEmpty())
    {
        QString labText = tr("MD-Transfer");
        int dc = 0;
        CNetMD::NetMDCmd netMdCmd;
        CXEnc::XEncCmd   xencCmd;
        QString          trackMode;
        transferConfig(netMdCmd, xencCmd, trackMode);

        if (ret < 0)
        {
            for (auto& t : mWorkQueue)
            {
                // mark all tracks as failed
                t.mStep = WorkStep::FAILED;

                if (QFile::exists(t.mFileName))
                {
                    QFile::remove(t.mFileName);
                }
            }

            mpRipper->removeTemp();
            enableDialogItems(true);
            delayedPopUp(ePopUp::CRITICAL, tr("Transfer Error!"), tr("Error while track transfer. Sorry!"));
            return;
        }

        if (mDAOMode == CDaoConfDlg::DAO_SP)
        {
            if (mWorkQueue.at(0).mStep == WorkStep::DONE)
            {
                ui->progressMDTransfer->setValue(100);

                for (auto& t : mWorkQueue)
                {
                    // mark all tracks as done
                    t.mStep = WorkStep::DONE;
                }

                // add to MD list
                if (ret != CNetMD::TOCMANIP_DEV_RESET) // TOC Manipulation done, device reset done
                {
                    addMDTrack(1, tr("Re-insert MD for content!"), trackMode, 60.00);
                }
            }
            else if (mWorkQueue.at(0).mStep == WorkStep::TRANSFER)
            {
                mWorkQueue[0].mStep = WorkStep::DONE;

                CNetMD::TocData tocData;

                // add disc name / -length to TOC data
                tocData.append({static_cast<const char*>(utf8ToMd(ui->lineCDTitle->text())),
                                blocksToMs(ui->tableViewCD->myModel()->audioLength())});

                for (auto& t : mWorkQueue)
                {
                    // add track name / -length to TOC data
                    tocData.append({static_cast<const char*>(utf8ToMd(t.mTitle)),
                                    static_cast<uint32_t>(std::round(t.mLength * 1000.0))});
                }

                labText = tr("TOC edit");

                // start TOC manipulation
                mpNetMD->start(tocData, mpSettings->devReset());
            }
            else if (mWorkQueue.at(0).mStep == WorkStep::ENCODED)
            {
                auto& disc = mWorkQueue[0];
                disc.mStep = WorkStep::TRANSFER;

                ui->progressMDTransfer->setValue(0);
                mpNetMD->start({netMdCmd, disc.mFileName, "DAO All in One!"});
            }
        }
        else
        {
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
                    mpNetMD->start({netMdCmd, j.mFileName, j.mTitle});
                    break;
                }
            }
        }

        for (auto& j : mWorkQueue)
        {
            if (j.mStep == WorkStep::DONE)
            {
                dc ++;
            }
        }

        countLabel(ui->labMDTransfer, WorkStep::ENCODED, labText);

        if (dc == mWorkQueue.size())
        {
            if (trackMode != "SP")
            {
                if (mpSettings->lpTrackGroup())
                {
                    addMDGroup(ui->lineCDTitle->text(),
                               static_cast<int16_t>(mpMDmodel->discConf()->mTrkCount - mWorkQueue.size() + 1),
                               static_cast<int16_t>(mpMDmodel->discConf()->mTrkCount));
                }
            }
            else
            {
                if (mpSettings->spMdTitle())
                {
                    // title is already set on DAO SP mode
                    if (mDAOMode != CDaoConfDlg::DAO_SP)
                    {
                        // set disc title
                        mpNetMD->start({CNetMD::NetMDCmd::RENAME_DISC, "", ui->lineCDTitle->text()});
                        setMDTitle(ui->lineCDTitle->text());
                    }
                    else if (ret != CNetMD::TOCMANIP_DEV_RESET)
                    {
                        setMDTitle("DAO TOC Edit");
                    }
                }
            }

            for (auto& j : mWorkQueue)
            {
                if (QFile::exists(j.mFileName))
                {
                    qInfo() << "Delete temp. file" << j.mFileName;
                    QFile::remove(j.mFileName);
                }
            }
            mWorkQueue.clear();
            mpRipper->removeTemp();
            enableDialogItems(true);
            QString info = tr("All (selected) tracks were transferred to MiniDisc!");
            if ((mDAOMode == CDaoConfDlg::DAO_SP) && (ret != CNetMD::TOCMANIP_DEV_RESET))
            {
                info += QString("<br><b>%1</b> %2").arg(tr("TOC edit done!")).arg("Please re-insert the minidisc in your device as soon as possible!");
            }
            delayedPopUp(ePopUp::INFORMATION, tr("Success"), info);
        }
    }
}

void MainWindow::addMDTrack(int number, const QString &title, const QString &mode, time_t length)
{
    QString t = title;
    deUmlaut(t);

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
    trk["name"]    = t.toStdString();
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
    QString t = title;
    deUmlaut(t);
    mpNetMD->start({CNetMD::NetMDCmd::ADD_GROUP, "", "", t, first, last});

    nlohmann::json group;
    group["name"]  = t.toStdString();
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
    QString t = title;
    deUmlaut(t);
    nlohmann::json mdJson = mpMDmodel->exportJson();
    mdJson["title"] = t.toStdString();
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
        ui->labMDTransfer->setText(tr("MD-Transfer: "));

        ui->labAnimation->clear();
        ui->labAnimation->setMovie(mpSettings->waitAni());
        mpSettings->waitAni()->start();
    }
    else
    {
        mpSettings->waitAni()->stop();
        ui->labAnimation->clear();
    }

    ui->tableViewCD->setEnabled(ena);
    ui->lineCDTitle->setEnabled(ena);
    ui->treeView->setEnabled(ena);
    ui->radioSP->setEnabled(ena);
    ui->radioLP2->setEnabled(ena);
    ui->radioLP4->setEnabled(ena);
    ui->pushSettings->setEnabled(ena);
    if (ena)
    {
        if ((mpMDmodel != nullptr) && mpMDmodel->discConf()->mOTFEnc)
        {
            mpSettings->enaDisaOtf(mpSettings->onthefly(), ena);
        }
    }
    else
    {
        mpSettings->enaDisaOtf(mpSettings->onthefly(), ena);
    }
    ui->pushLoadMD->setEnabled(ena);
    ui->pushInitCD->setEnabled(ena);
    ui->pushLoadImg->setEnabled(ena);

    if (ena)
    {
        if ((ui->tableViewCD->model() != nullptr)
            && (ui->tableViewCD->model()->rowCount() > 0)
            && (ui->treeView->model() != nullptr)
            && (ui->treeView->model()->rowCount() > 0)
            && (mpMDmodel && (mpMDmodel->discConf()->mDiscFlags & eDiscFlags::WRITEABLE))
            && (mpMDmodel && !(mpMDmodel->discConf()->mDiscFlags & eDiscFlags::WRITE_LOCK)))
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

    mpMDDevice->setText(mpMDmodel->discConf()->mDevice.isEmpty() ? tr("Please re-load MD") : mpMDmodel->discConf()->mDevice);
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
    int all =  mWorkQueue.size();

    if (((mDAOMode == CDaoConfDlg::DAO_LP2) || (mDAOMode == CDaoConfDlg::DAO_SP))
            && ((step == MainWindow::WorkStep::NONE) || (step == MainWindow::WorkStep::RIPPED)))
    {
        all = 1;
    }
    else if ((mDAOMode == CDaoConfDlg::DAO_SP) && (step == MainWindow::WorkStep::ENCODED))
    {
        all = 1;
    }

    pLabel->clear();
    pLabel->setText(tr("%1: %2/%3").arg(text).arg(count).arg(all));
}

void MainWindow::loadSettings()
{
    QSettings set;
    if (set.contains("mainwindow"))
    {
        setGeometry(set.value("mainwindow").toRect());
    }

    if (mpSettings->theme() == SettingsDlg::Theme::STANDARD)
    {
        ui->tableViewCD->setStyleSheet(styles::CD_TAB_STD);
    }
    else
    {
        ui->tableViewCD->setStyleSheet(styles::CD_TAB_STYLED);
    }
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
    mDAOMode = CDaoConfDlg::DAO_WTF;

    CDaoConfDlg* pDaoConf = new CDaoConfDlg(this);

    if (pDaoConf)
    {
        pDaoConf->tocManip(mTocManip);
        if (pDaoConf->exec() == QDialog::Accepted)
        {
            mDAOMode = pDaoConf->daoMode();
        }
        else
        {
            return;
        }
        delete pDaoConf;
    }

    if (mDAOMode == CDaoConfDlg::DAO_WTF)
    {
        return;
    }

    if (mTracksBackup.listType() == c2n::AudioTracks::CD)
    {
        // DAO can only be done from original CD
        // revert any change in track order or count
        revertCDEntries();
    }

    enableDialogItems(false);
    c2n::AudioTracks trks = ui->tableViewCD->myModel()->audioTracks();
    trks.prepend({ui->lineCDTitle->text(), "", "", 0, 0, ui->tableViewCD->myModel()->audioLength()});
    mpRipper->setAudioTracks(trks);
    bool isCD = (trks.listType() == c2n::AudioTracks::CD);

    mpSettings->enaDisaOtf(false, true);
    if (mDAOMode == CDaoConfDlg::DAO_LP2)
    {
        ui->radioLP2->setChecked(true);
    }
    else if (mDAOMode == CDaoConfDlg::DAO_SP)
    {
        ui->radioSP->setChecked(true);
    }

    ui->tableViewCD->selectAll();
    QModelIndexList selected = ui->tableViewCD->selectionModel()->selectedRows();

    double selectionTime = 0;
    mWorkQueue.clear();

    // Multiple rows can be selected
    for(const auto& r : selected)
    {
        int16_t trackNo    = isCD ? trks.at(r.row() + 1).mCDTrackNo : (r.row() + 1);
        QString trackTitle = r.data().toString();
        double  trackTime  = r.sibling(r.row(), 1).data(Qt::UserRole).toDouble();
        selectionTime += trackTime;
        mWorkQueue.append({trackNo,
                           trackTitle,
                           QDir::tempPath() + tempFileName("/cd2netmd.XXXXXX.tmp"),
                           trackTime,
                           WorkStep::NONE,
                           isCD});
    }

    if (mDAOMode == CDaoConfDlg::DAO_LP2)
    {
        // LP2 needs half time only
        selectionTime /= 2;
    }

    if (mpSettings->sizeCheck() && (selectionTime > mpMDmodel->discConf()->mFreeTime))
    {
        // not enough space left on device
        time_t need = selectionTime - mpMDmodel->discConf()->mFreeTime;
        QString t = QString("%1:%2:%3").arg(need / 3600).arg((need % 3600) / 60, 2, 10, QChar('0')).arg(need % 60, 2, 10, QChar('0'));
        mWorkQueue.clear();
        enableDialogItems(true);
        delayedPopUp(ePopUp::WARNING, tr("Error"), tr("No space left on MD to transfer your selected titles. You need %1 more.").arg(t), 100);
    }
    else if (!mWorkQueue.isEmpty())
    {
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

void MainWindow::on_pushSettings_clicked()
{
    mpSettings->show();
}

void MainWindow::on_pushLoadImg_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Cue Sheet File"), "", tr("Cue Sheet (*.cue)"));
    if (!fileName.isEmpty())
    {
        parseCueFile(fileName);
    }
}

//--------------------------------------------------------------------------
//! @brief      catch dropped files from cd table view
//!
//! @param[in]  sl  string list with file pathes
//--------------------------------------------------------------------------
void MainWindow::catchDropped(QStringList sl)
{
    int length;
    int wholeLength = 0;
    audio::STag tag;
    c2n::STrackInfo trackInfo;
    c2n::AudioTracks tracks;

    if (ui->tableViewCD->myModel() != nullptr)
    {
        tracks      = ui->tableViewCD->myModel()->audioTracks();
        wholeLength = ui->tableViewCD->myModel()->audioLength();
    }

    if (tracks.listType() != c2n::AudioTracks::FILES)
    {
        tracks.clear();
        tracks.setListType(c2n::AudioTracks::FILES);
        ui->lineCDTitle->clear();
        wholeLength = 0;
    }

    for (const auto& url : sl)
    {
        if (audio::checkAudioFile(url, trackInfo.mConversion, length, &tag) == 0)
        {
            trackInfo.mFileName = url;
            trackInfo.mStartLba = 0;
            trackInfo.mLbCount  = qRound((static_cast<double>(length) / 1000.0) * static_cast<double>(CDIO_CD_FRAMES_PER_SEC));
            wholeLength        += trackInfo.mLbCount;

            if (!tag.mTitle.isEmpty())
            {
                if (!tag.mArtist.isEmpty())
                {
                    trackInfo.mTitle = QString("%1 - ").arg(tag.mArtist);
                }

                trackInfo.mTitle += tag.mTitle;
            }
            else
            {
                trackInfo.mTitle = titleFromFileName(url);
            }

            tracks.append(trackInfo);
        }
    }

    if (wholeLength > 0)
    {
        // disc "title"
        trackInfo.mFileName = "";
        trackInfo.mStartLba = 0;
        trackInfo.mLbCount  = wholeLength;
        if (ui->lineCDTitle->text().isEmpty())
        {
            trackInfo.mTitle = tr("(Maybe) Various Artists - Dropped Hits (%1)").arg(QDateTime::currentDateTime().toString());
        }
        else
        {
            trackInfo.mTitle = ui->lineCDTitle->text();
        }
        tracks.prepend(trackInfo);
    }

    if (!tracks.isEmpty())
    {
        mpRipper->setDeviceInfo("Drag'n'Drop");
        catchCDDBEntry(tracks);
    }
}

//--------------------------------------------------------------------------
//! @brief      catch new audio length in list
//!
//! @param      length in blocks
//--------------------------------------------------------------------------
void MainWindow::audioLength(long blocks)
{
    time_t length = blocks / CDIO_CD_FRAMES_PER_SEC;

    ui->labCDTime->clear();
    ui->labCDTime->setText(tr("Disc Time: %1:%2:%3").arg(length / 3600, 1, 10, QChar('0'))
                           .arg((length % 3600) / 60, 2, 10, QChar('0'))
                           .arg(length % 60, 2, 10, QChar('0')));
}

//--------------------------------------------------------------------------
//! @brief      parse cue file
//!
//! @param[in]  fileName cue sheet file name
//--------------------------------------------------------------------------
int MainWindow::parseCueFile(QString fileName)
{
    int ret = -1;
    c2n::STrackInfo cueInfo;
    c2n::AudioTracks tracks;
    QFileInfo fi(fileName);
    tracks.setListType(c2n::AudioTracks::CUE_SHEET);

    CueParser* pParser = new CueParser(fileName);
    if (pParser)
    {
        if (pParser->isValid())
        {
            // all well ...
            ret = 0;

            // disc info
            QString discArtist = pParser->discPerfromer();
            if (!discArtist.isEmpty())
            {
                cueInfo.mTitle = discArtist + " - ";
            }
            cueInfo.mTitle += pParser->discTitle();

            // in case no title is stored, use file name
            if (cueInfo.mTitle.isEmpty())
            {
                cueInfo.mTitle = titleFromFileName(fileName);
            }

            if (cueInfo.mTitle.isEmpty())
            {
                cueInfo.mTitle = tr("<untitled>");
            }
            cueInfo.mLbCount = qRound((static_cast<double>(pParser->discLength()) / 1000.0) * 75.0);
            tracks.append(cueInfo);

            // tracks ...
            for (int i = 1; i <= pParser->trackCount(); i++)
            {
                const CueParser::Track& t = pParser->track(i);

                cueInfo.mFileName   = fi.canonicalPath() + "/" + t.mAudioFile;
                cueInfo.mStartLba   = t.mStartLba;
                cueInfo.mLbCount    = t.mLbaCount;
                cueInfo.mCDTrackNo  = t.mNo;
                cueInfo.mConversion = t.mConversion;
                cueInfo.mTType      = t.mType;
                cueInfo.mTitle      = "";
                if (!t.mPerformer.isEmpty() && (t.mPerformer != discArtist))
                {
                    cueInfo.mTitle  = t.mPerformer + " - ";
                }
                cueInfo.mTitle     += t.mTitle;

                // in case no title is stored, use file name
                if (cueInfo.mTitle.isEmpty())
                {
                    cueInfo.mTitle = titleFromFileName(cueInfo.mFileName);
                }

                if (cueInfo.mTitle.isEmpty())
                {
                    cueInfo.mTitle = tr("<untitled>");
                }
                tracks.append(cueInfo);
            }
        }
        delete pParser;
    }

    mpRipper->setDeviceInfo("Cue Sheet");
    catchCDDBEntry(tracks);
    return ret;
}

void MainWindow::on_pushHelp_clicked()
{
    QString sAppDir = QApplication::applicationDirPath();
#ifdef Q_OS_WIN
    QDesktopServices::openUrl(QUrl(QString("file:///%1/help/NetMD Wizard.html").arg(sAppDir)));
#elif defined Q_OS_MAC
    QDir applFolder(sAppDir);
    if (applFolder.cdUp())
    {
        QDesktopServices::openUrl(QUrl(QString("file:///%1/Resources/help/NetMD Wizard.html").arg(applFolder.canonicalPath())));
    }
#elif defined Q_OS_LINUX
    QString sBinName = QFileInfo(QApplication::applicationFilePath()).baseName();
    QDir applFolder(sAppDir);
    if (applFolder.cdUp())
    {
        QDesktopServices::openUrl(QUrl(QString("file:///%1/share/%2/resources/help/NetMD Wizard.html").arg(applFolder.canonicalPath()).arg(sBinName)));
    }
#endif
}

void MainWindow::on_pushLog_clicked()
{
    QDesktopServices::openUrl(QUrl(QString("file:///%1").arg(g_logFileName)));
}

//--------------------------------------------------------------------------
//! @brief      show a popup message delayed (asynchronous)
//!
//! @param      tp      Popup type
//! @param[in]  caption window title
//! @param[in]  msg     message text
//! @param[in]  wait    delay time in ms
//--------------------------------------------------------------------------
void MainWindow::delayedPopUp(ePopUp tp, const QString& caption, const QString& msg, int wait)
{
    switch(tp)
    {
    case ePopUp::CRITICAL:
        QTimer::singleShot(wait, [=]()->void{QMessageBox::critical(this, caption, msg);});
        break;
    case ePopUp::INFORMATION:
        QTimer::singleShot(wait, [=]()->void{QMessageBox::information(this, caption, msg);});
        break;
    case ePopUp::WARNING:
        QTimer::singleShot(wait, [=]()->void{QMessageBox::warning(this, caption, msg);});
        break;
    }
}

