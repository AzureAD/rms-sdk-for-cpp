REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin
TARGET    = rms

INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/Profile
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/logger
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/external/easyloggingpp

DEFINES     += RMS_LIBRARY

TEMPLATE  = lib
QT       += core xml xmlpatterns widgets network
QT 	     -= gui
CONFIG   += plugin c++11 debug_and_release warn_on
QMAKE_CFLAGS_WARN_ON -= -W3
QMAKE_CFLAGS_WARN_ON += -W4

LIBS        += -L$$REPO_ROOT/bin/ -L$$REPO_ROOT/bin/rms/ -L$$REPO_ROOT/bin/rms/platform/

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS += -lmodprotectedfiled -lmodcored -lmodrestclientsd -lmodconsentd -lmodcommond -lmodjsond
    LIBS += -lplatformhttpd -lplatformloggerd -lplatformxmld -lplatformjsond -lplatformfilesystemd -lplatformsettingsd
    LIBS += -lrmscryptod
    LIBS += -lloggerd
} else {
    LIBS += -lmodprotectedfile -lmodcore -lmodrestclients -lmodconsent -lmodcommon -lmodjson
    LIBS += -lplatformhttp -lplatformlogger -lplatformxml -lplatformjson -lplatformfilesystem -lplatformsettings
#link third-part library at the end to prevent logger implementation conflict
    LIBS += -lrmscrypto
    LIBS += -llogger
}

win32:LIBS += -L$$REPO_ROOT/third_party/lib/eay/ -lssleay32 -llibeay32 -lGdi32 -lUser32 -lAdvapi32 -ldnsapi
else:LIBS  += -lssl -lcrypto

SOURCES += \
    UserPolicy.cpp \
    TemplateDescriptor.cpp \
    ConsentCallbackImpl.cpp \
    PolicyDescriptor.cpp \
    ProtectedFileStream.cpp \
    CustomProtectedStream.cpp \
    HttpHelper.cpp \
    IRMSEnvironment.cpp \
    $$REPO_ROOT/sdk/rms_sdk/Profile/profile.cpp

HEADERS += \
    UserPolicy.h \
    IAuthenticationCallback.h \
    IConsentCallback.h \
    TemplateDescriptor.h \
    rights.h \
    ConsentCallbackImpl.h \
    IConsentCallbackImpl.h \
    AuthenticationCallbackImpl.h \
    IAuthenticationCallbackImpl.h \
    ConsentResult.h \
    IConsent.h \
    Roles.h \
    PolicyDescriptor.h \
    UserRights.h \
    UserRoles.h \
    AuthenticationParameters.h \
    ConsentType.h \
    ProtectedFileStream.h \
    CustomProtectedStream.h \
    HttpHelper.h \
    CacheControl.h \
    RMSExceptions.h \
    IRMSEnvironment.h \
    $$REPO_ROOT/sdk/rms_sdk/Profile/profile.h


unix {
    contains(QMAKE_HOST.arch, x86_64) {
        target.path = /usr/lib64
        INSTALLS += target
    } else {
        target.path += /usr/lib
        INSTALLS += target
    }
}
