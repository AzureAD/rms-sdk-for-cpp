QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
REPO_ROOT = $$PWD/../..
DESTDIR   = $$REPO_ROOT/bin/
TARGET = context_sample
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

INCLUDEPATH = $$REPO_ROOT/sdk/rmssealer_sdk/SealerAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rmsauth_sdk/rmsauth
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/ModernAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rmssealer_sdk/CLCClient
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/
win32:INCLUDEPATH += $$REPO_ROOT/third_party/include
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -L$$DESTDIR -lrmsauthd -lrmsauthWebAuthDialogd -lrmsd -lrmscryptod
}else {
    LIBS +=  -L$$DESTDIR -lrmsauth -lrmsauthWebAuthDialog -lrms -lrmscrypto
}
