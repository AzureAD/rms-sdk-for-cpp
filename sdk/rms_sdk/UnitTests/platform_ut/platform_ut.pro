REPO_ROOT = $$PWD/../../../..
DESTDIR   = $$REPO_ROOT/bin/tests
TARGET    = rmsplatformUnitTests

LIBS     += -L$$REPO_ROOT/bin -L$$REPO_ROOT/bin/rms -L$$REPO_ROOT/bin/rms/platform

QT       += network xml xmlpatterns testlib
QT       -= gui

CONFIG   += console c++11 debug_and_release
CONFIG   -= app_bundle

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS += -lplatformhttpd -lplatformloggerd -lplatformxmld -lplatformjsond -lrmscryptod -lplatformfilesystemd -lplatformsettingsd
} else {
    LIBS += -lplatformhttp -lplatformlogger -lplatformxml -lplatformjson -lrmscrypto -lplatformfilesystem -lplatformsettings
}

win32:LIBS += -L$$REPO_ROOT/third_party/lib/eay/ -lssleay32MDd -llibeay32MDd -lGdi32 -lUser32 -lAdvapi32
else:LIBS  += -lssl -lcrypto

TEMPLATE = app

win32:INCLUDEPATH += $$REPO_ROOT/third_party/include
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoStreams/CryptoAPI

DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += \
    main.cpp \
    PlatformHttpClientTest.cpp \
    PlatformXmlTest.cpp \
    PlatformJsonArrayTest.cpp \
    PlatformJsonObjectTest.cpp \
    PlatformFileSystemTest.cpp \
    PlatformFileTest.cpp

HEADERS += \
    PlatformHttpClientTest.h \
    PlatformXmlTest.h \
    PlatformJsonArrayTest.h \
    PlatformJsonObjectTest.h \
    PlatformFileSystemTest.h \
    PlatformFileTest.h \
    TestHelpers.h

