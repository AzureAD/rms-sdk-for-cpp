REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin
TARGET    = mip_file_sdk

QT       -= core gui

TEMPLATE  = lib
CONFIG += plugin warn_on c++11 debug_and_release
CONFIG -= qt

INCLUDEPATH = $$REPO_ROOT/sdk/file_sdk $$REPO_ROOT/sdk/rms_sdk/ModernAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/Profile
LIBS      +=  -L$$REPO_ROOT/bin -L$$REPO_ROOT/bin/file

win32:LIBS += -L$$REPO_ROOT/third_party/lib/xmp

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -lmodfilecommond -lmodcompoundfiled -lmoddefaultfiled -lmodopcfiled -lmodpdffiled -lmodpfilefiled -lmodxmpfiled
    win32:LIBS += -lXMPCoreStaticD -lXMPFilesStaticD
} else {
    LIBS +=  -lmodfilecommon -lmodcompoundfile -lmoddefaultfile -lmodopcfile -lmodpdffile -lmodpfilefile -lmodxmpfile
    win32:LIBS += -lXMPCoreStatic -lXMPFilesStatic
}

unix:!mac:LIBS += -L/usr/lib/xmp -lXMPCore -lXMPFiles -ldl

DEFINES += FILE_LIBRARY

SOURCES += \
    stream_handler.cpp

HEADERS += \
    stream_handler.h
