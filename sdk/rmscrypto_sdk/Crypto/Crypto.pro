REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/crypto
TARGET    = modcrypto

QT       -= gui
TEMPLATE  = lib
CONFIG   += staticlib warn_on c++11 debug_and_release
QT       += core

win32:INCLUDEPATH += $$REPO_ROOT/third_party/include
LIBS += -L$$REPO_ROOT/bin/crypto/platform

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -lplatformcryptod
} else {
    LIBS +=  -lplatformcrypto
}


SOURCES += Cbc4kCryptoProvider.cpp \
    Cbc512NoPaddingCryptoProvider.cpp \
    EcbCryptoProvider.cpp

HEADERS += \
    Cbc4kCryptoProvider.h \
    Cbc512NoPaddingCryptoProvider.h \
    EcbCryptoProvider.h \
    CryptoConstants.h
