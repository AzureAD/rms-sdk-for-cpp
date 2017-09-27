REPO_ROOT = $$PWD/../../../..
DESTDIR   = $$REPO_ROOT/bin/tests
TARGET    = rmsfileapiUnitTests

TEMPLATE  = app

QT       += core widgets network xml xmlpatterns testlib
QT       -= gui

CONFIG   += console c++11 debug_and_release warn_on
CONFIG   -= app_bundle

INCLUDEPATH += $$REPO_ROOT/sdk/rmsauth_sdk/rmsauth
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/ModernAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/FileAPI

LIBS       += -L$$REPO_ROOT/bin -L$$REPO_ROOT/bin/rms -L$$REPO_ROOT/bin/rms/platform

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS += -lrmsd -lmodprotectedfiled -lmodcored -lmodrestclientsd -lmodconsentd -lmodcommond -lmodjsond
    LIBS += -lplatformhttpd -lplatformloggerd -lplatformxmld -lplatformjsond -lplatformfilesystemd -lplatformsettingsd
    LIBS += -lrmscryptod -lmodpoled -lmodprotectedofficefiled -lrmsfiled
} else {
    LIBS += -lrms -lmodprotectedfile -lmodcore -lmodrestclients -lmodconsent -lmodcommon -lmodjson
    LIBS += -lplatformhttp -lplatformlogger -lplatformxml -lplatformjson -lplatformfilesystem -lplatformsettings
    LIBS += -lrmscrypto -lmodpole -lmodprotectedofficefile -lrmsfile
}

win32:LIBS += -L$$REPO_ROOT/third_party/lib/eay/ -lssleay32 -llibeay32 -lGdi32 -lUser32 -lAdvapi32
else:LIBS  += -lssl -lcrypto

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    FileAPIProtectorSelectorTest.h

SOURCES += \
    FileAPIProtectorSelectorTest.cpp \
    main.cpp
