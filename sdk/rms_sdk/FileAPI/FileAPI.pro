REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin
TARGET    = rmsfile

TEMPLATE = lib
CONFIG  += plugin c++11 debug_and_release warn_on no_keywords

DEFINES     += RMS_LIBRARY

QMAKE_CFLAGS_WARN_ON -= -W3
QMAKE_CFLAGS_WARN_ON += -W4

QT       += core xml xmlpatterns widgets network
QT 	     -= gui

INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/Pole
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/ModernAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/PFile

LIBS        += -L$$REPO_ROOT/bin/ -L$$REPO_ROOT/bin/rms/ -L$$REPO_ROOT/bin/rms/platform/

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS += -lrmsd -lmodprotectedfiled -lmodcored -lmodrestclientsd -lmodconsentd -lmodcommond -lmodjsond
    LIBS += -lplatformhttpd -lplatformloggerd -lplatformxmld -lplatformjsond -lplatformfilesystemd -lplatformsettingsd
    LIBS += -lrmscryptod -lmodpoled -lmodprotectedofficefiled
} else {
    LIBS += -lrms -lmodprotectedfile -lmodcore -lmodrestclients -lmodconsent -lmodcommon -lmodjson
    LIBS += -lplatformhttp -lplatformlogger -lplatformxml -lplatformjson -lplatformfilesystem -lplatformsettings
    LIBS += -lrmscrypto -lmodpole -lmodprotectedofficefile
}

#link third-part library at the end to prevent logger implementation conflict

win32:LIBS += -L$$REPO_ROOT/third_party/lib/eay/ -lssleay32 -llibeay32 -lGdi32 -lUser32 -lAdvapi32
else:LIBS  += -lssl -lcrypto

HEADERS += \
    MetroOfficeProtector.h \
    PFileProtector.h \
    Protector.h \
    FileAPIStructures.h \
    ProtectorSelector.h \
    MsoOfficeProtector.h \
    OfficeUtils.h \
    TemplateConstants.h

SOURCES += \
    Protector.cpp \
    PFileProtector.cpp \
    MetroOfficeProtector.cpp \
    ProtectorSelector.cpp \
    MsoOfficeProtector.cpp \
    OfficeUtils.cpp

unix {
    contains(QMAKE_HOST.arch, x86_64) {
        target.path = /usr/lib64
        INSTALLS += target
    } else {
        target.path += /usr/lib
        INSTALLS += target
    }    
    CONFIG += link_pkgconfig
    PKGCONFIG += libgsf-1
}


