REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/file
TARGET    = modpdffile

TEMPLATE = lib
CONFIG += staticlib warn_on c++11 debug_and_release
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI $$REPO_ROOT/sdk/file_sdk/Common

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

SOURCES += pdf_file_format.cpp

HEADERS += pdf_file_format.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
