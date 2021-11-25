#include "ccddbentriesdialog.h"
#include "ui_ccddbentriesdialog.h"
#include <QStringListModel>
#include <QAbstractButton>

CCDDBEntriesDialog::CCDDBEntriesDialog(const QStringList &entries, QWidget *parent) :
    QDialog(parent), mEntries(entries),
    ui(new Ui::CCDDBEntriesDialog)
{
    ui->setupUi(this);
}

CCDDBEntriesDialog::~CCDDBEntriesDialog()
{
    delete ui;
}

QString CCDDBEntriesDialog::request()
{
    return mRequest;
}

void CCDDBEntriesDialog::showEvent(QShowEvent *e)
{
    ui->listView->setModel(new QStringListModel(mEntries));
    QDialog::showEvent(e);
}


void CCDDBEntriesDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    switch(ui->buttonBox->buttonRole(button))
    {
    case QDialogButtonBox::ApplyRole:
        {
            mRequest = "";
            QModelIndex idx = ui->listView->currentIndex();
            if (idx.isValid())
            {
                QStringList l = idx.data().toString().split('\t');
                mRequest = l.at(0);
            }
            accept();
        }
        break;
    default:
        reject();
        break;
    }
}
