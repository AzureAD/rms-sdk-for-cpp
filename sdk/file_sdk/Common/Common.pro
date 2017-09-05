REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/file
TARGET    = modfilecommon

TEMPLATE  = lib
CONFIG += staticlib warn_on c++11 debug_and_release
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI $$REPO_ROOT/sdk/file_sdk

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

SOURCES += \
    tag.cpp \
    file_format.cpp \
    file_format_factory.cpp
    file_format_factory.cpp

HEADERS += \
    extended_property.h \
    macros.h \
    tag.h \
    ifile_format.h \
    file_format.h \
    file_format_factory.h \
    libgsf.h
    file_format_factory.h

