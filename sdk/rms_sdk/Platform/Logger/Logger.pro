REPO_ROOT = $$PWD/../../../..
DESTDIR   = $$REPO_ROOT/bin/rms/platform
TARGET    = platformlogger

QT       -= gui
QT       += core

DEFINES += QTFRAMEWORK

INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/profile

TEMPLATE = lib

CONFIG += staticlib warn_on c++11 debug_and_release

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

SOURCES += \
    LoggerImplQt.cpp

HEADERS += \
    Logger.h \
    LoggerImplQt.h
