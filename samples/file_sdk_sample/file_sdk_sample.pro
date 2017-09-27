REPO_ROOT = $$PWD/../..
DESTDIR   = $$REPO_ROOT/bin/
TARGET    = file_sdk_sample

TEMPLATE = app
CONFIG += console c++11 debug_and_release
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += $$REPO_ROOT/sdk/file_sdk $$REPO_ROOT/sdk/file_sdk/Common $$REPO_ROOT/sdk/rms_sdk/ModernAPI/ext $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI $$REPO_ROOT/sdk/rms_sdk/ModernAPI $$REPO_ROOT/sdk/rms_sdk/Profile
unix:!mac:INCLUDEPATH += /usr/include/xmp/

LIBS      +=  -L$$DESTDIR -L$$DESTDIR/rms -L$$DESTDIR/file

win32:LIBS += -L$$REPO_ROOT/third_party/lib/xmp

CONFIG(debug, debug|release) {
    LIBS += -lrmscryptod -lrmsd -lmodfilecommond -lmodcompoundfiled -lmoddefaultfiled -lmodopcfiled -lmodpdffiled -lmodpfilefiled -lmodxmpfiled -lmip_file_sdkd
    win32:LIBS += -lXMPCoreStaticD -lXMPFilesStaticD
} else {
    LIBS += -lrmscrypto -lrms -lmodfilecommon -lmodcompoundfile -lmoddefaultfile -lmodopcfile -lmodpdffile -lmodpfilefile -lmodxmpfile -lmip_file_sdk
    win32:LIBS += -lXMPCoreStatic -lXMPFilesStatic
}

unix:!mac:LIBS += -L/usr/lib/xmp -lXMPCore -lXMPFiles -ldl

SOURCES += main.cpp \
    utils.cpp

HEADERS += \
    main.h \
    utils.h
