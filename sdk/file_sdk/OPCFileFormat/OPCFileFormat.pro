REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/file
TARGET    = modopcfile

TEMPLATE = lib
CONFIG += staticlib warn_on c++11 debug_and_release
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI
INCLUDEPATH += $$REPO_ROOT/sdk/file_sdk/Common
INCLUDEPATH += $$REPO_ROOT/sdk/rmsauth_sdk/rmsauth/rmsauth

win32:INCLUDEPATH += $$REPO_ROOT/third_party/include/Libgsf
win32:LIBS += -L$$REPO_ROOT/third_party/lib/Libgsf -llibgsf-1-114 -lzlib1\
               -llibgobject-2.0-0\
               -llibglib-2.0-0 -llibbz2-1

SOURCES += opc_file_format.cpp \
    zipapi.cpp \
    gsfinputistream.cpp

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

HEADERS += opc_file_format.h \
    zipapi.h \
    gsfinputistream.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
