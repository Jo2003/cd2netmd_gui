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
#include "cnamingdialog.h"
#include "ui_cnamingdialog.h"

CNamingDialog::CNamingDialog(QWidget *parent, const QString &wtitle, const QString &descr) :
    QDialog(parent),
    ui(new Ui::CNamingDialog)
{
    ui->setupUi(this);
    setWindowTitle(wtitle);
    ui->labelName->setText(descr);
}

CNamingDialog::~CNamingDialog()
{
    delete ui;
}

QString CNamingDialog::name() const
{
    return ui->lineName->text();
}

void CNamingDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole)
    {
        accept();
    }
    else
    {
        reject();
    }
}
