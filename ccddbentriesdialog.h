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
#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class CCDDBEntriesDialog;
}

class CCDDBEntriesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CCDDBEntriesDialog(const QStringList& entries, QWidget *parent = nullptr);
    ~CCDDBEntriesDialog();
    QString request();

protected:
    void showEvent(QShowEvent *e) override;
    QStringList mEntries;
    QString mRequest;

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::CCDDBEntriesDialog *ui;
};
