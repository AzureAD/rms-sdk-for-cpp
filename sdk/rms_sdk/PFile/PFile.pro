REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/rms
TARGET    = modprotectedfile

TEMPLATE = lib
CONFIG  += staticlib warn_on c++11 debug_and_release

QMAKE_CFLAGS_WARN_ON -= -W3
QMAKE_CFLAGS_WARN_ON += -W4

QT       += core
QT 	     -= gui

INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/ModernAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/FileAPI

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

SOURCES +=  PfileHeader.cpp \
            PfileHeaderReader.cpp \
            PfileHeaderWriter.cpp \
            PFileProtector.cpp \
            Utils.cpp

HEADERS +=  IPfileHeaderReader.h \
            IPfileHeaderWriter.h \
            PfileHeader.h \
            PfileHeaderReader.h \
            PfileHeaderWriter.h \
            PFileProtector.h \
            Utils.h
