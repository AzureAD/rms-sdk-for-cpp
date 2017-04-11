REPO_ROOT = $$PWD/../../../..
DESTDIR   = $$REPO_ROOT/bin/tests
TARGET    = RestClientsUnitTests

TEMPLATE  = app

QT       += core network xml xmlpatterns testlib
QT       -= gui

CONFIG   += console c++11 debug_and_release
CONFIG   -= app_bundle

INCLUDEPATH       += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI
win32:INCLUDEPATH += $$REPO_ROOT/third_party/include $$REPO_ROOT/log4cplus/include

LIBS       += -L$$REPO_ROOT/bin -L$$REPO_ROOT/bin/rms -L$$REPO_ROOT/bin/rms/platform

CONFIG(debug, debug|release) {
   TARGET = $$join(TARGET,,,d)
    LIBS += -lmodprotectedfiled -lmodcored -lmodrestclientsd -lmodconsentd -lmodcommond -lmodjsond
    LIBS += -lplatformhttpd -lplatformloggerd -llog4cplusloggerd -lplatformxmld -lplatformjsond -lplatformfilesystemd -lplatformsettingsd
    LIBS += -lrmscryptod
    LIBS += -lrmsd
    win32:LIBS +=  -L$$REPO_ROOT/log4cplus/lib/Debug/ -llog4cplusUD -llog4cplus-Qt5DebugAppender
} else {
    LIBS += -lmodprotectedfile -lmodcore -lmodrestclients -lmodconsent -lmodcommon -lmodjson    
    LIBS += -lplatformhttp -lplatformlogger -llog4cpluslogger -lplatformxml -lplatformjson -lplatformfilesystem -lplatformsettings
    LIBS += -lrmscrypto
    LIBS += -lrms
    win32:LIBS +=  -L$$REPO_ROOT/log4cplus/lib/Release/ -llog4cplusU -llog4cplus-Qt5DebugAppender
}

win32:LIBS += -L$$REPO_ROOT/third_party/lib/eay/ -lssleay32 -llibeay32 -lGdi32 -lUser32 -lAdvapi32
else:LIBS  += -lssl -lcrypto

DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += \
    main.cpp \
    LicenseParserTest.cpp \
    LicenseParserTestConstants.cpp \

HEADERS += \
    LicenseParserTest.h \
    LicenseParserTestConstants.h \
    
