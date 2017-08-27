REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/filesdk
TARGET    = modfilecommon

TEMPLATE  = lib
CONFIG += staticlib warn_on c++11 debug_and_release
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI $$REPO_ROOT/sdk/file_sdk

SOURCES += \
    string_utils.cpp \
    tag.cpp \
    file_format.cpp \
    file_format_factory.cpp
    file_format_factory.cpp

HEADERS += \
    extended_property.h \
    macros.h \
    string_utils.h \
    tag.h \
    ifile_format.h \
    file_format.h \
    file_format_factory.h
    file_format_factory.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
