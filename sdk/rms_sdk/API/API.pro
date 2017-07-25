#-------------------------------------------------
#
# Project created by QtCreator 2017-07-24T21:32:46
#
#-------------------------------------------------

QT       -= core gui

TARGET = API
TEMPLATE = lib

INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/ModernAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/FileAPI

DEFINES += API_LIBRARY

SOURCES += profile.cpp

HEADERS += profile.h\
        api_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
