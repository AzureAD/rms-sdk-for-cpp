REPO_ROOT = $$PWD/../../../..

DESTDIR   = $$REPO_ROOT/bin/tests
TARGET    = commonUnitTests

QT       += testlib

QT       -= gui

CONFIG   += console c++11 debug_and_release resources_big
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH = $$REPO_ROOT/sdk/file_sdk $$REPO_ROOT/sdk/file_sdk/Common $$REPO_ROOT/sdk/rms_sdk/ModernAPI/ext $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI

LIBS      +=  -L$$REPO_ROOT/bin -L$$REPO_ROOT/bin/rms -L$$REPO_ROOT/bin/file

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -lrmsd -lmodfilecommond -lmodcompoundfiled -lmoddefaultfiled -lmodopcfiled -lmodpdffiled -lmodpfilefiled -lmodxmpfiled
} else {
    LIBS +=  -lrms -lmodfilecommon -lmodcompoundfile -lmoddefaultfile -lmodopcfile -lmodpdffile -lmodpfilefile -lmodxmpfile
}

SOURCES += common_ut.cpp \
    istream_mock.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    istream_mock.h
