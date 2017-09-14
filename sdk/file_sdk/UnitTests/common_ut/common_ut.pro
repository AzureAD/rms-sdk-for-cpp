REPO_ROOT = $$PWD/../../../..

DESTDIR   = $$REPO_ROOT/bin/tests
TARGET    = commonUnitTests

QT       += testlib

QT       -= gui

CONFIG   += console c++11 debug_and_release resources_big
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH = $$REPO_ROOT/sdk/file_sdk $$REPO_ROOT/sdk/file_sdk/Common
win32:INCLUDEPATH += $$REPO_ROOT/third_party/include/xmp
unix:!mac:INCLUDEPATH += /usr/include/xmp/

LIBS      +=  -L$$REPO_ROOT/bin/rms -L$$REPO_ROOT/bin/file
win32:LIBS += -L$$REPO_ROOT/third_party/lib/xmp

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS += -lmodcompoundfiled -lmoddefaultfiled -lmodopcfiled -lmodpdffiled -lmodpfilefiled -lmodxmpfiled -lmodfilecommond
    win32:LIBS += -lXMPCoreStaticD -lXMPFilesStaticD
} else {
    LIBS += -lmodcompoundfile -lmoddefaultfile -lmodopcfile -lmodpdffile -lmodpfilefile -lmodxmpfile -lmodfilecommon
    win32:LIBS += -lXMPCoreStatic -lXMPFilesStatic
}

unix:!mac:LIBS += -L/usr/lib/xmp -lXMPCore -lXMPFiles -ldl

SOURCES += common_ut.cpp \
    istream_mock.cpp \
    extended_property_ut.cpp \
    main.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    istream_mock.h \
    common_ut.h \
    extended_property_ut.h \
    include.h
