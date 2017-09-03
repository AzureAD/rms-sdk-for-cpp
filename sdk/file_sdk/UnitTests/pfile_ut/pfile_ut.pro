REPO_ROOT = $$PWD/../../../..

DESTDIR   = $$REPO_ROOT/bin/tests
TARGET    = pFileUnitTests

QT       += testlib

QT       -= gui

CONFIG   += console c++11 debug_and_release resources_big
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH = $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI $$REPO_ROOT/sdk/file_sdk $$REPO_ROOT/sdk/file_sdk/Common $$REPO_ROOT/sdk/rms_sdk/ModernAPI/ext

LIBS                  +=  -L$$REPO_ROOT/bin -L$$REPO_ROOT/bin/crypto -L$$REPO_ROOT/bin/rms -L$$REPO_ROOT/bin/file

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -lrmscryptod -lrmsd -lmodfilecommond -lmodpfilefiled
} else {
    LIBS +=  -lrmscrypto -lrms -lmodfilecommon -lmodpfilefile
}

SOURCES += pfile_tests.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

RESOURCES += \
    pfiles.qrc
