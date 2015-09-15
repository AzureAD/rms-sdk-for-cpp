#include "consentlistdialog.h"
#include "ui_consentlistdialog.h"

ConsentListDialog::ConsentListDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ConsentListDialog)
{
  ui->setupUi(this);
}

void ConsentListDialog::InitConsentList(QStandardItemModel& model) {
  ui->ConsentTableView->setModel(&model);

  for (int i = 0, width = ui->ConsentTableView->width(); i < 4; ++i) {
    ui->ConsentTableView->setColumnWidth(i, width / 4);
  }
}

ConsentListDialog::~ConsentListDialog()
{
  delete ui;
}

bool ConsentListDialog::PreventInFuture() {
    return ui->checkBox->checkState() == Qt::Checked;
}
