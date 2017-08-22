REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/filesdk
TARGET    = modfilecommon


TEMPLATE  = lib
CONFIG += staticlib warn_on c++11 debug_and_release
INCLUDEPATH += ../../rmscrypto_sdk/CryptoAPI

SOURCES += \
    string_utils.cpp \
    tag.cpp \
    ifile_format.cpp \
    file_format.cpp

HEADERS += \
    extended_property.h \
    macros.h \
    string_utils.h \
    tag.h \
    ifile_format.h \
    file_format.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
