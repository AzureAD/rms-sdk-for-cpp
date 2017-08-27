REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/filesdk
TARGET    = modpfilefile

TEMPLATE = lib
CONFIG += staticlib warn_on c++11 debug_and_release
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI $$REPO_ROOT/sdk/file_sdk/Common

SOURCES += pfile_file_format.cpp

HEADERS += pfile_file_format.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
