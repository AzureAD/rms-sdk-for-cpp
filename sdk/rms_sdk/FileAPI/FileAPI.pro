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

INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/Platform/Logger
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/ModernAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/PFile
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/OfficeProtector
win32:INCLUDEPATH += $$REPO_ROOT/third_party/include/Libgsf

LIBS        += -L$$REPO_ROOT/bin/ -L$$REPO_ROOT/bin/rms/ -L$$REPO_ROOT/bin/rms/platform/
win32:LIBS += -L$$REPO_ROOT/third_party/lib/Libgsf -llibgthread-2.0-0 -llibiconv-2 -llibintl-8 -lliblzma-5 -llibpcre-1
win32:LIBS += -llibwinpthread-1 -llibxml2-2 -lzlib1 -llibbz2-1 -llibffi-6 -llibgio-2.0-0 -llibglib-2.0-0
win32:LIBS += -llibgmodule-2.0-0 -llibgobject-2.0-0 -llibgsf-1-114 -ldnsapi

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
    protector.h \
    file_api_structures.h \
    protector_selector.h

SOURCES += \
    protector.cpp \
    protector_selector.cpp

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


