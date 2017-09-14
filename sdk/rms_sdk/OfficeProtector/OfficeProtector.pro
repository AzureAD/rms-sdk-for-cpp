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
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/Common
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/Core
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/Platform/Logger
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/ModernAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/FileAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/PFile
win32:INCLUDEPATH += $$REPO_ROOT/third_party/include/Libgsf

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

HEADERS += \
    data_spaces.h \
    metro_office_protector.h \
    mso_office_protector.h \
    office_utils.h \
    stream_constants.h

SOURCES += \
    data_spaces.cpp \
    metro_office_protector.cpp \
    mso_office_protector.cpp \
    office_utils.cpp

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += libgsf-1
}
