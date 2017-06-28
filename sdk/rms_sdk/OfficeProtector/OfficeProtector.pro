REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/rms
TARGET    = modprotectedofficefile

TEMPLATE = lib
CONFIG  += staticlib warn_on c++11 debug_and_release no_keywords

QMAKE_CFLAGS_WARN_ON -= -W3
QMAKE_CFLAGS_WARN_ON += -W4

QT       += core
QT 	     -= gui

INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

HEADERS += \
    DataSpaces.h \
    Utils.h \
    IDataSpaces.h

SOURCES += \
    DataSpaces.cpp \
    Utils.cpp

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += libgsf-1
}
