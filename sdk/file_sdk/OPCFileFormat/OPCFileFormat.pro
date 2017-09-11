REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/file
TARGET    = modopcfile

TEMPLATE = lib
CONFIG += staticlib warn_on c++11 debug_and_release
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI $$REPO_ROOT/sdk/file_sdk

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

SOURCES += opc_file_format.cpp \
    xml/contenttypes.cpp \
    xml/relationships.cpp \
    xml/customproperties.cpp \
    xml/opcxml.cpp

HEADERS += opc_file_format.h \
    xml/contenttypes.h \
    xml/relationships.h \
    xml/customproperties.h \
    xml/opcxml.h
