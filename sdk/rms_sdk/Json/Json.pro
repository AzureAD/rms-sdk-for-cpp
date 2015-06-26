REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/rms
TARGET    = modjson

QT       -= gui
TEMPLATE  = lib
CONFIG   += staticlib warn_on c++11 debug_and_release

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

SOURCES += \
    JsonSerializer.cpp

HEADERS += \
    IJsonSerializer.h \
    JsonSerializer.h
