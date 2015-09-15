#ifndef CONSENTLISTDIALOG_H
#define CONSENTLISTDIALOG_H

#include <QStandardItemModel>
#include <QAbstractTableModel>
#include <QDialog>

namespace Ui {
class ConsentListDialog;
}

class ConsentListDialog : public QDialog {
  Q_OBJECT

public:

  explicit ConsentListDialog(QWidget *parent = 0);
  ~ConsentListDialog();
  void InitConsentList(QStandardItemModel& model);
  bool PreventInFuture();

private:

  Ui::ConsentListDialog *ui;
};

#endif // CONSENTLISTDIALOG_H
