#-------------------------------------------------
#
# Project created by QtCreator 2017-06-08T11:30:57
#
#-------------------------------------------------
REPO_ROOT = $$PWD/../..

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rmssealer_ui_sample
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    AuthCallback.cpp \
    MainWindow.cpp

HEADERS += \
    AuthCallback.h \
    MainWindow.h

FORMS += \
        mainwindow.ui
INCLUDEPATH = $$REPO_ROOT/sdk/rmssealer_sdk/SealerAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rmsauth_sdk/rmsauth/rmsauth
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/ModernAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rmssealer_sdk/CLCClient
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/
win32:INCLUDEPATH += $$REPO_ROOT/third_party/include
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../bin/sealer/ -lSealerAPI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../bin/sealer/ -lSealerAPId
else:unix: LIBS += -L$$PWD/../../bin/sealer/ -lSealerAPI

INCLUDEPATH += $$PWD/../../bin/sealer
DEPENDPATH += $$PWD/../../bin/sealer

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../bin/ -lrms
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../bin/ -lrmsd
else:unix: LIBS += -L$$PWD/../../bin/ -lrms

INCLUDEPATH += $$PWD/../../bin
DEPENDPATH += $$PWD/../../bin

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../bin/ -lrmsauth
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../bin/ -lrmsauthd
else:unix: LIBS += -L$$PWD/../../bin/ -lrmsauth

INCLUDEPATH += $$PWD/../../bin
DEPENDPATH += $$PWD/../../bin

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../bin/ -lrmscrypto
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../bin/ -lrmscryptod
else:unix: LIBS += -L$$PWD/../../bin/ -lrmscrypto

INCLUDEPATH += $$PWD/../../bin
DEPENDPATH += $$PWD/../../bin

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../bin/ -lrmsauthWebAuthDialog
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../bin/ -lrmsauthWebAuthDialogd
else:unix: LIBS += -L$$PWD/../../bin/ -lrmsauthWebAuthDialog

INCLUDEPATH += $$PWD/../../bin
DEPENDPATH += $$PWD/../../bin
