REPO_ROOT = $$PWD/../../../..
DESTDIR   = $$REPO_ROOT/bin/tests
TARGET    = rmscryptoUnitTests

LIBS     += -L$$REPO_ROOT/bin -L$$REPO_ROOT/bin/rms -L$$REPO_ROOT/bin/rms/platform

QT       += testlib
QT       -= gui

CONFIG   += console c++11 debug_and_release
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += tst_CryptoTest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"



CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -lrmsd -lplatformhttpd -lplatformloggerd -lplatformxmld -lplatformjsond
} else {
    LIBS +=  -lrms -lplatformhttp -lplatformlogger -lplatformxml -lplatformjson
}

