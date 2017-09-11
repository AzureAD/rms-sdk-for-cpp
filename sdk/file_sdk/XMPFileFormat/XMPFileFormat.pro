REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/file
TARGET    = modxmpfile

DEFINES += XMP_StaticBuild XML_DTD
win32:DEFINES += WIN_ENV
unix:!mac:DEFINES += UNIX_ENV

TEMPLATE    = lib
CONFIG      += staticlib warn_on c++11 debug_and_release
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI $$REPO_ROOT/sdk/file_sdk

INCLUDEPATH = $$REPO_ROOT/sdk/file_sdk $$REPO_ROOT/sdk/file_sdk/Common $$REPO_ROOT/sdk/rms_sdk/ModernAPI/ext $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI
win32:INCLUDEPATH += $$REPO_ROOT/third_party/include/xmp
unix:!mac:INCLUDEPATH += /usr/include/xmp/

LIBS +=  -L$$REPO_ROOT/bin -L$$REPO_ROOT/bin/rms -L$$REPO_ROOT/bin/file
win32:LIBS += -L$$REPO_ROOT/third_party/lib/xmp

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -lrmsd -lmodfilecommond
    win32:LIBS += -lXMPCoreStaticD -lXMPFilesStaticD
} else {
    LIBS +=  -lrms  -lmodfilecommon
    win32:LIBS += -lXMPCoreStatic -lXMPFilesStatic
}

unix:!mac:LIBS += -lstaticXMPFiles -lstaticXMPCore

SOURCES += xmp_file_format.cpp \
    xmp_helper.cpp \
    xmpio_over_istream.cpp

HEADERS += xmp_file_format.h \
    xmpio_over_istream.h \
    xmp_helper.h

