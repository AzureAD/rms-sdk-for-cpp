REPO_ROOT = $$PWD/../../../..
DESTDIR   = $$REPO_ROOT/bin/tests
TARGET    = profileUnitTests

TEMPLATE = app

INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/profile

QT += core testlib
QT -= gui

CONFIG += console c++11 debug_and_release warn_on
CONFIG -= app_bundle

LIBS += -L$$REPO_ROOT/bin

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS += -lrmsd
} else {
    LIBS += -lrms
}

SOURCES += main.cpp \
    profile_test.cpp

HEADERS += \
    profile_test.h
