REPO_ROOT = $$PWD/../../../..
DESTDIR   = $$REPO_ROOT/bin/tests
TARGET = zipFileUnitTests

QT       += testlib

QT       -= gui

CONFIG   += console c++11 debug_and_release resources_big
CONFIG   -= app_bundle

INCLUDEPATH += $$REPO_ROOT/third_party/include/Libgsf
INCLUDEPATH += $$REPO_ROOT/sdk/rmsauth_sdk/rmsauth/rmsauth  $$REPO_ROOT/sdk/file_sdk $$REPO_ROOT/sdk/file_sdk/Common $$REPO_ROOT/sdk/rms_sdk/ModernAPI/ext $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI

win32:LIBS += -L$$REPO_ROOT/bin/file -L$$REPO_ROOT/bin -L$$REPO_ROOT/bin/rms
win32:LIBS += -L$$REPO_ROOT/third_party/lib/Libgsf -llibgsf-1-114 -lzlib1\
               -llibgobject-2.0-0 -llibglib-2.0-0 -llibbz2-1

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -lmodopcfiled -lrmsd -lrmscryptod -lmodfilecommond
} else {
    LIBS +=  -lmodopcfile -lrms -lrmscrypto -lmodfilecommon
}

TEMPLATE = app


SOURCES += zip_tests.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

RESOURCES += \
    zip_files.qrc
