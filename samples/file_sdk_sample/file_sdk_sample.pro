REPO_ROOT = $$PWD/../..
DESTDIR   = $$REPO_ROOT/bin/debug/amd64/sdk
TARGET    = file_sdk_sample

TEMPLATE = app
CONFIG += console c++11 debug_and_release
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += $$REPO_ROOT/sdk/file_sdk $$REPO_ROOT/sdk/file_sdk/Common $$REPO_ROOT/sdk/rms_sdk/ModernAPI/ext $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI $$REPO_ROOT/sdk/rms_sdk/ModernAPI $$REPO_ROOT/sdk/rms_sdk/Profile
win32:INCLUDEPATH += $$REPO_ROOT/third_party/include/Libgsf
win32:INCLUDEPATH += $$REPO_ROOT/third_party/include

win32:LIBS += -L$$REPO_ROOT/third_party/lib/Libgsf -llibgsf-1-114 -lzlib1 -llibgobject-2.0-0 -llibglib-2.0-0 -llibbz2-1

win32:LIBS += -L$$REPO_ROOT/third_party/lib/libxml/debug/x64 -llibxml2_a_dll

unix:!mac:INCLUDEPATH += /usr/include/xmp/

LIBS      +=  -L$$DESTDIR

win32:LIBS += -L$$REPO_ROOT/third_party/lib/xmp

CONFIG(debug, debug|release) {
    LIBS += -lfile -lrms -lrmsfile
    win32:LIBS += -lXMPCoreStaticD -lXMPFilesStaticD
} else {
    LIBS +=  -lfile -lrms -lrmsfile
    win32:LIBS += -lXMPCoreStatic -lXMPFilesStatic
}

unix:!mac:LIBS += -L/usr/lib/xmp -lXMPCore -lXMPFiles -ldl

SOURCES += main.cpp \
    utils.cpp

HEADERS += \
    main.h \
    utils.h
