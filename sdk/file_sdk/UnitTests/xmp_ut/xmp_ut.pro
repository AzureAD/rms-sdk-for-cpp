REPO_ROOT = $$PWD/../../../..

DESTDIR   = $$REPO_ROOT/bin/tests
TARGET    = xmpFileUnitTests

QT       += testlib

QT       -= gui

CONFIG   += console c++11 debug_and_release resources_big
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH = $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI $$REPO_ROOT/sdk/file_sdk $$REPO_ROOT/sdk/file_sdk/Common $$REPO_ROOT/sdk/rms_sdk/ModernAPI/ext

LIBS +=  -L$$REPO_ROOT/bin -L$$REPO_ROOT/bin/crypto -L$$REPO_ROOT/bin/rms  -L$$REPO_ROOT/bin/file

win32:LIBS += -L$$REPO_ROOT/third_party/lib/xmp
unix:!mac:LIBS += /usr/lib/xmp

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -lrmscryptod -lrmsd -lmodxmpfiled -lmodfilecommond -lXMPCoreStaticD -lXMPFilesStaticD
} else {
    LIBS +=  -lrmscrypto -lrms -lmodxmpfile -lmodfilecommon
    win32:LIBS += -lXMPCoreStatic -lXMPFilesStatic
    unix:!mac:LIBS += -lstaticXMPFiles -lstaticXMPCore
}

SOURCES += xmp_file_tests.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

RESOURCES += \
    xmp_files.qrc
