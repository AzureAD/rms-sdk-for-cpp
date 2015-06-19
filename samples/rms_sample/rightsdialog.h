/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RIGHTSDIALOG_H
#define RIGHTSDIALOG_H

#include <QStandardItemModel>
#include <QAbstractTableModel>
#include <QDialog>

namespace Ui {
class RightsDialog;
}

class RightsDialog : public QDialog {
  Q_OBJECT

public:

  explicit RightsDialog(QWidget *parent = 0);
  ~RightsDialog();

  void InitRightsList(QStandardItemModel& model);

private:

  Ui::RightsDialog *ui;
};

#endif // RIGHTSDIALOG_H
