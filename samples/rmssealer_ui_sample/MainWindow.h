#ifndef MAINWINDOW_H
#define MAINWINDOW_H


//#include "../ModernAPI/SealPolicy.h"
//#include "../ModernAPI/Seal.h"
#include "ModernAPI/WritableDataCollection.h"
#include "ModernAPI/DataCollection.h"

#include <QMainWindow>

using namespace rmscore::modernapi;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<WritableDataCollection> writableDataCollection;
    std::shared_ptr<DataCollection> dataCollection;

private slots:
    void on_AddToCollection_clicked();
    void on_SignAndSerialize_clicked();
    void on_initDataCollection_clicked();
    void on_VerifyButton_clicked();
};

#endif // MAINWINDOW_H
