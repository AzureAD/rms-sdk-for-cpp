REPO_ROOT = $$PWD/../../../..
DESTDIR   = $$REPO_ROOT/bin/tests
TARGET = zip_tests

QT       += testlib

QT       -= gui

CONFIG   += console c++11 debug_and_release resources_big
CONFIG   -= app_bundle

INCLUDEPATH += $$REPO_ROOT/third_party/include/Libgsf

win32:LIBS += -L$$REPO_ROOT/bin/file
win32:LIBS += -L$$REPO_ROOT/third_party/lib/Libgsf -llibgsf-1-114 -lzlib1\
               -llibgobject-2.0-0 -llibglib-2.0-0 -llibbz2-1

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -lmodopcfiled
} else {
    LIBS +=  -lmodopcfile
}

TEMPLATE = app


SOURCES += zip_tests.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
