REPO_ROOT = $$PWD/../../../..
DESTDIR   = $$REPO_ROOT/bin/rms/platform
TARGET    = platformfilesystem

TEMPLATE = lib

DEFINES += QTFRAMEWORK

CONFIG += staticlib warn_on c++11 debug_and_release

QT += core
QT -= gui

INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

SOURCES += \
    FileQt.cpp \
    FileSystemQt.cpp

HEADERS += \
    IFile.h \
    IFileSystem.h \
    FileQt.h \
    FileSystemQt.h
