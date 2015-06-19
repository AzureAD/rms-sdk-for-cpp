/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "rightsdialog.h"
#include "ui_rightsdialog.h"

RightsDialog::RightsDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::RightsDialog)
{
  ui->setupUi(this);
}

RightsDialog::~RightsDialog()
{
  delete ui;
}

void RightsDialog::InitRightsList(QStandardItemModel& model) {
  ui->UserRightsView->setModel(&model);
  ui->UserRightsView->resizeRowsToContents();

  for (int i = 1; i < 8; ++i) {
    ui->UserRightsView->setColumnWidth(i, 65);
  }
}
