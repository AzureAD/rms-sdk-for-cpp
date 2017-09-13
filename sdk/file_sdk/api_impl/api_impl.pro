REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin
TARGET    = mip_file_sdk

QT       -= core gui

TEMPLATE  = lib
CONFIG += plugin warn_on c++11 debug_and_release
CONFIG -= qt

INCLUDEPATH = $$REPO_ROOT/sdk/file_sdk $$REPO_ROOT/sdk/file_sdk/Common $$REPO_ROOT/sdk/rms_sdk/ModernAPI
LIBS      +=  -L$$REPO_ROOT/bin -L$$REPO_ROOT/bin/file

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -lmodfilecommond -lmodcompoundfiled -lmoddefaultfiled -lmodopcfiled -lmodpdffiled -lmodpfilefiled -lmodxmpfiled
} else {
    LIBS +=  -lmodfilecommon -lmodcompoundfile -lmoddefaultfile -lmodopcfile -lmodpdffile -lmodpfilefile -lmodxmpfile
}

DEFINES += FILE_LIBRARY

SOURCES += \
    stream_handler.cpp

HEADERS += \
    stream_handler.h
