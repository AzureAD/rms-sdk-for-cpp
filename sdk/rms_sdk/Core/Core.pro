REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/rms
TARGET    = modcore

TEMPLATE = lib
CONFIG  += staticlib warn_on c++11 debug_and_release
QT      += core
QT      -= gui

INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI

win32:INCLUDEPATH += $$REPO_ROOT/third_party/include

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

SOURCES += ProtectionPolicy.cpp

HEADERS += ProtectionPolicy.h \
    FeatureControl.h
