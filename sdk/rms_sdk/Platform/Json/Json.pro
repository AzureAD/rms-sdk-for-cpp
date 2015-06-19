REPO_ROOT = $$PWD/../../../..
DESTDIR   = $$REPO_ROOT/bin/rms/platform
TARGET    = platformjson

TEMPLATE = lib

DEFINES += QTFRAMEWORK

CONFIG += staticlib warn_on c++11 debug_and_release

QT += core
QT -= gui

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

SOURCES += \
    JsonArrayQt.cpp \
    JsonObjectQt.cpp \
    JsonParserQt.cpp

HEADERS += \
    IJsonArray.h \
    IJsonObject.h \
    JsonArrayQt.h \
    JsonObjectQt.h \
    JsonParserQt.h \
    IJsonParser.h
