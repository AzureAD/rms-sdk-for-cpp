REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/tests
TARGET    = rmscryptoUnitTests

TEMPLATE = app

QT       += core testlib widgets

CONFIG   -= app_bundle
CONFIG   += console c++11 debug_and_release warn_on

INCLUDEPATH           += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI
win32:INCLUDEPATH     += $$REPO_ROOT/third_party/include
unix:!mac:INCLUDEPATH += /usr/include/glib-2.0/ /usr/include/libsecret-1/ /usr/lib/x86_64-linux-gnu/glib-2.0/include/
# mac:INCLUDEPATH     +=

DEFINES += SRCDIR=\\\"$$PWD/\\\"

LIBS                  +=  -L$$REPO_ROOT/bin -L$$REPO_ROOT/bin/crypto -L$$REPO_ROOT/bin/crypto/platform

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -lmodcryptod -lplatformkeystoraged -lplatformcryptod -lrmscryptod
} else {
    LIBS +=  -lmodcrypto -lplatformkeystorage -lplatformcrypto -lrmscrypto
}

win32:LIBS            += -L$$REPO_ROOT/third_party/lib/eay/ -lssleay32MDd -llibeay32MDd -lGdi32 -lUser32 -lAdvapi32
unix:!mac:LIBS        += -lssl -lcrypto -lsecret-1 -lglib-2.0
mac:LIBS              += -lssl -lcrypto

SOURCES += \
    main.cpp \
    PlatformCryptoTest.cpp \
    KeyStorageTests.cpp \
    CryptedStreamTests.cpp \
    CryptoAPITests.cpp

HEADERS += \
    KeyStorageTests.h \
    PlatformCryptoTest.h \
    CryptedStreamTests.h \
    TestHelpers.h \
    CryptoAPITests.h
