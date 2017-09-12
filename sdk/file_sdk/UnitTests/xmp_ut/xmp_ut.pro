REPO_ROOT = $$PWD/../../../..

DESTDIR   = $$REPO_ROOT/bin/tests
TARGET    = xmpFileUnitTests

QT       += testlib

QT       -= gui

CONFIG   += console c++11 debug_and_release resources_big
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH = $$REPO_ROOT/sdk/file_sdk $$REPO_ROOT/sdk/file_sdk/Common
win32:INCLUDEPATH += $$REPO_ROOT/third_party/include/xmp
unix:!mac:INCLUDEPATH += /usr/include/xmp/

LIBS +=  -L$$REPO_ROOT/bin -L$$REPO_ROOT/bin/file

win32:LIBS += -L$$REPO_ROOT/third_party/lib/xmp

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -lmodxmpfiled -lmodfilecommond -lXMPCoreStaticD -lXMPFilesStaticD
} else {
    LIBS +=  -lmodxmpfile -lmodfilecommon
    win32:LIBS += -lXMPCoreStatic -lXMPFilesStatic
}

unix:!mac:LIBS += -lstaticXMPFiles -lstaticXMPCore

SOURCES += xmp_file_tests.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

RESOURCES += \
    xmp_files.qrc
