REPO_ROOT = $$PWD/../../../..
DESTDIR   = $$REPO_ROOT/bin/tests
TARGET    = rmsfileapiUnitTests

TEMPLATE  = app

QT       += core widgets network xml xmlpatterns testlib
QT       -= gui

CONFIG   += console c++11 debug_and_release warn_on
CONFIG   -= app_bundle


INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/UnitTests/fileapi_ut/pdf_protector_ut/
INCLUDEPATH += $$REPO_ROOT/sdk/rmsauth_sdk/rmsauth
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/ModernAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/FileAPI

INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/PFile
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/Pole
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/PDFObjectModel
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/OfficeProtector

LIBS       += -L$$REPO_ROOT/bin -L$$REPO_ROOT/bin/rms -L$$REPO_ROOT/bin/rms/platform

INCLUDEPATH += $$REPO_ROOT/googlemock/
INCLUDEPATH += $$REPO_ROOT/googlemock/include
INCLUDEPATH += $$REPO_ROOT/googletest/
INCLUDEPATH += $$REPO_ROOT/googletest/include


CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS += -lrmsd -lmodprotectedfiled -lmodcored -lmodrestclientsd -lmodconsentd -lmodcommond -lmodjsond
    LIBS += -lplatformhttpd -lplatformloggerd -lplatformxmld -lplatformjsond -lplatformfilesystemd -lplatformsettingsd
    LIBS += -lrmscryptod -lmodpoled -lmodprotectedofficefiled -lrmsfiled
    LIBS +=  -L$$DESTDIR -lrmsauthd -lrmsauthWebAuthDialogd -lrmsd -lrmscryptod -lrmsfiled -lpdfobjectmodeld
} else {
    LIBS += -lrms -lmodprotectedfile -lmodcore -lmodrestclients -lmodconsent -lmodcommon -lmodjson
    LIBS += -lplatformhttp -lplatformlogger -lplatformxml -lplatformjson -lplatformfilesystem -lplatformsettings
    LIBS += -lrmscrypto -lmodpole -lmodprotectedofficefile -lrmsfile
    LIBS +=  -L$$DESTDIR -lrmsauth -lrmsauthWebAuthDialog -lrms -lrmscrypto -lrmsfile -lpdfobjectmodel
}

win32:LIBS += -L$$REPO_ROOT/third_party/lib/eay/ -lssleay32 -llibeay32 -lGdi32 -lUser32 -lAdvapi32
else:LIBS  += -lssl -lcrypto

DEFINES += SRCDIR=\\\"$$PWD/\\\"
#DEFINES-= UNICODE

HEADERS += \
    FileAPIProtectorSelectorTest.h \
    pdf_protector_ut/depend.h \
    pdf_protector_ut/Auth.h \
    pdf_protector_ut/unittest_protector.h \
    pdf_protector_ut/unittest_pdfobjectmodel.h \
    pdf_protector_ut/pdfobjectmodel_mock.h




SOURCES += \
    FileAPIProtectorSelectorTest.cpp \
    main.cpp \
    $$REPO_ROOT/googletest/src/gtest-all.cc \
    $$REPO_ROOT/googlemock/src/gmock-all.cc \
    pdf_protector_ut/unittest_protector.cpp \
    pdf_protector_ut/Auth.cpp \
    pdf_protector_ut/depend.cpp \
    pdf_protector_ut/unittest_pdfobjectmodel.cpp \
    pdf_protector_ut/pdfobjectmodel_mock.cpp

