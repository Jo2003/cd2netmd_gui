/**
 * Copyright (C) 2023 Jo2003 (olenka.joerg@gmail.com)
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

#include "statuswidget.h"
#include "ui_statuswidget.h"
#include <QPixmap>

StatusWidget::StatusWidget(QWidget *parent, const QString &icon, const QString &text, int iconSz)
    : QWidget(parent),
      ui(new Ui::StatusWidget), mScale(iconSz)
{
    ui->setupUi(this);

    if (!icon.isEmpty())
    {
        QPixmap pic(icon);
        pic = pic.scaled(mScale, mScale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->icon->setPixmap(pic);
    }

    ui->text->setText(text);
}

StatusWidget::~StatusWidget()
{
    delete ui;
}

void StatusWidget::setText(const QString &text)
{
    ui->text->clear();
    ui->text->setText(text);
    ui->text->show();
}

void StatusWidget::setIcon(const QString &icon)
{
    if (!icon.isEmpty())
    {
        QPixmap pic(icon);
        pic = pic.scaled(mScale, mScale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->icon->clear();
        ui->icon->setPixmap(pic);
        ui->icon->show();
    }
}

