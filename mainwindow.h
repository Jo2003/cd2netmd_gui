#pragma once
#include <QMainWindow>
#include "cjacktheripper.h"
#include "ccddb.h"
#include "ccddbentriesdialog.h"
#include "ccditemmodel.h"

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

    void on_pushInitCD_clicked();

private:
    Ui::MainWindow *ui;
    CJackTheRipper *mpRipper;
};
