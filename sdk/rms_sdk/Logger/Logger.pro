REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin
TARGET    = logger

QT       -= core gui

TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/Profile
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/external/easyloggingpp

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

SOURCES += \
    $$REPO_ROOT/sdk/rms_sdk/External/easyloggingpp/easylogging++.cc \
    logger.cpp

HEADERS += \
    $$REPO_ROOT/sdk/rms_sdk/External/easyloggingpp/easylogging++.h \
    logger.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
