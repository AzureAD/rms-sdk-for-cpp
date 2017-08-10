#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "AuthCallback.h"
#include "QMessageBox"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->AddToCollection->hide();
    ui->SignAndSerialize->hide();
}

void MainWindow::on_initDataCollection_clicked(){
    std::string userEmail = ui->User_Email->text().toStdString();
    std::string clientID = "4a63455a-cfa1-4ac6-bd2e-0d046cf1c3f7";
    std::string redirect = "https://client.test.app";
    AuthCallback authCallback(clientID, redirect);
    writableDataCollection = WritableDataCollection::AcquireWritableDataCollection(userEmail, authCallback, clientID);
    ui->AddToCollection->show();
    ui->SignAndSerialize->show();
}

void MainWindow::on_AddToCollection_clicked(){
    writableDataCollection->AddData(ui->Add_Key->text().toStdString(), ui->Add_Value->text().toStdString());
}

void MainWindow::on_SignAndSerialize_clicked(){
    ui->DataCollection_Output->setText(writableDataCollection->SignAndSerializeDataCollection().c_str());
}

void MainWindow::on_VerifyButton_clicked(){
    dataCollection = DataCollection::AcquireVerifiedDataCollection(ui->VerifyJSON->toPlainText().toStdString());
    if(dataCollection->IsVerified()){
        ui->ValidDataCollectionLabel->setText("Valid Data Collection");
    }else{
        ui->ValidDataCollectionLabel->setText("Invalid Data Collection");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
