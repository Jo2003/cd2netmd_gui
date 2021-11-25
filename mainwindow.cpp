#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), mpRipper(nullptr)
{
    ui->setupUi(this);

    if ((mpRipper = new CJackTheRipper(this)) != nullptr)
    {
        connect(mpRipper, &CJackTheRipper::progress, ui->progressBar, &QProgressBar::setValue);
        connect(mpRipper->cddb(), &CCDDB::entries, this, &MainWindow::catchCDDBEntries);
        connect(mpRipper->cddb(), &CCDDB::match, this, &MainWindow::catchCDDBEntry);
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


    CCDItemModel *pModel = new CCDItemModel(l, v, this);

    ui->tableViewCD->setModel(pModel);
    int width = ui->tableViewCD->width();

    ui->tableViewCD->setColumnWidth(0, (width / 100) * 80);
    ui->tableViewCD->setColumnWidth(1, (width / 100) * 15);
}

void MainWindow::on_pushInitCD_clicked()
{
    mpRipper->init();
}
