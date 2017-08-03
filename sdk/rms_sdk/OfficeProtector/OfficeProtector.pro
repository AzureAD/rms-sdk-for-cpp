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
win32:INCLUDEPATH += C:\Libgsf\include

win32:LIBS += -LC:\Libgsf\dll -llibgthread-2.0-0 -llibiconv-2 -llibintl-8 -lliblzma-5 -llibpcre-1
win32:LIBS += -llibwinpthread-1 -llibxml2-2 -lzlib1 -llibbz2-1 -llibffi-6 -llibgio-2.0-0 -llibglib-2.0-0
win32:LIBS += -llibgmodule-2.0-0 -llibgobject-2.0-0 -llibgsf-1-114

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
