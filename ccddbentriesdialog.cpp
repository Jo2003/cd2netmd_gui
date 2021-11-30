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
