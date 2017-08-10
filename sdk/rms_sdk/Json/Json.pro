REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/rms
TARGET    = modjson

QT       -= gui
TEMPLATE  = lib
CONFIG   += staticlib warn_on c++11 debug_and_release

win32:INCLUDEPATH += $$REPO_ROOT/third_party/include
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

SOURCES += \
    JsonSerializer.cpp \
    CertificateJsonParser.cpp

HEADERS += \
    IJsonSerializer.h \
    JsonSerializer.h \
    CertificateJsonParser.h
