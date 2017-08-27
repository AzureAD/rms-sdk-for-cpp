REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/filesdk
TARGET    = moddefaultfile

TEMPLATE = lib

TEMPLATE = lib
CONFIG += staticlib warn_on c++11 debug_and_release
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI $$REPO_ROOT/sdk/file_sdk/Common

SOURCES += default_format.cpp

HEADERS += default_format.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
