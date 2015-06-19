REPO_ROOT = $$PWD/../../../../..
DESTDIR   = $$REPO_ROOT/bin/crypto/platform
TARGET    = platformkeystorage

QT       -= gui

TEMPLATE  = lib
CONFIG   += staticlib warn_on c++11 debug_and_release
QT       += core

unix:!mac:INCLUDEPATH  += /usr/include/glib-2.0/ /usr/include/libsecret-1/ /usr/lib/x86_64-linux-gnu/glib-2.0/include/
# win32:INCLUDEPATH += DPAPI
# mac:INCLUDEPATH += osxkeychain

LIBS +=  -L$$DESTDIR -L$$DESTDIR/Platform/

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}
unix {
    target.path = /usr/lib
    INSTALLS += target
}

HEADERS += \
    base64.h \
    IKeyStorage.h

SOURCES += \
    base64.cpp \
    IKeyStorage.cpp

#include different versions of keystorage
win32 {
    HEADERS += KeyStorageWindows.h
    SOURCES += KeyStorageWindows.cpp
} unix:!mac {
    HEADERS += KeyStoragePosix.h
    SOURCES += KeyStoragePosix.cpp
} mac {
    HEADERS += KeyStorageOSX.h
    SOURCES += KeyStorageOSX.cpp
}

