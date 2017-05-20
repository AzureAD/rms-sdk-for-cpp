REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/rms
TARGET    = modfileapi

TEMPLATE = lib
CONFIG  += staticlib warn_on c++11 debug_and_release

QMAKE_CFLAGS_WARN_ON -= -W3
QMAKE_CFLAGS_WARN_ON += -W4

QT       += core
QT 	     -= gui

INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/Pole
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/ModernAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/OfficeProtector
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/PFile

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

HEADERS += \
    IProtector.h \
    MetroOfficeProtector.h \
    PFileProtector.h \
    ProtectorFactory.h

SOURCES += \
    MetroOfficeProtector.cpp \
    PFileProtector.cpp \
    ProtectorFactory.cpp

DISTFILES +=

