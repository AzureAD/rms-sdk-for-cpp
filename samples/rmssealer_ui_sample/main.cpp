#include "MainWindow.h"
#include <QApplication>
#include <IRMSEnvironment.h>
#include <IRMSAuthEnvironment.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto envRMS = rmscore::modernapi::RMSEnvironment();

    auto envAuth = rmsauth::RMSAuthEnvironment();

    envRMS->LogOption(rmscore::modernapi::IRMSEnvironment::LoggerOption::Always);

    envAuth->LogOption(rmsauth::IRMSAuthEnvironment::LoggerOption::Always);
    MainWindow w;
    w.show();

    return a.exec();
}
