REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin
TARGET    = rmscrypto


TEMPLATE = lib
CONFIG  += plugin warn_on c++11 debug_and_release
QT      -= gui
QT      += core

DEFINES += RMS_CRYPTO_LIBRARY

win32:INCLUDEPATH += $$REPO_ROOT/third_party/include $$REPO_ROOT/log4cplus/include
unix:!mac:INCLUDEPATH += /usr/include/glib-2.0/ /usr/include/libsecret-1/ /usr/lib/x86_64-linux-gnu/glib-2.0/include/

LIBS    += -L$$REPO_ROOT/bin/crypto -L$$REPO_ROOT/bin/crypto/platform

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -lmodcryptod -lplatformkeystoraged -lplatformcryptod -lplatformloggerd -llog4cplusloggerd -lplatformsettingsd
    win32:LIBS +=  -L$$REPO_ROOT/log4cplus/lib/Debug/ -llog4cplusUD -llog4cplus-Qt5DebugAppender
} else {
    LIBS +=  -lmodcrypto -lplatformkeystorage -lplatformcrypto -lplatformlogger -llog4cpluslogger -lplatformsettings
    win32:LIBS +=  -L$$REPO_ROOT/log4cplus/lib/Release/ -llog4cplusU -llog4cplus-Qt5DebugAppender
}

win32:LIBS += -L$$REPO_ROOT/third_party/lib/eay/ -lssleay32 -llibeay32 -lGdi32 -lUser32 -lAdvapi32
win32:LIBS += -llegacy_stdio_definitions
unix:!mac:LIBS  += -lssl -lcrypto -lsecret-1 -lglib-2.0
mac:LIBS += -lssl -lcrypto

unix {
    contains(QMAKE_HOST.arch, x86_64) {
        target.path = /usr/lib64
        INSTALLS += target
    } else {
        target.path += /usr/lib
        INSTALLS += target
    }
}

HEADERS += \
    BlockBasedProtectedStream.h \
    CachedBlock.h \
    IStream.h \
    SimpleProtectedStream.h \
    ICryptoStream.h \
    CryptoAPI.h \
    StdStreamAdapter.h \
    ICryptoProvider.h \
    ICryptoEngine.h \
    ICryptoHash.h \
    ICryptoKey.h \
    CryptoAPIExport.h \
    RMSCryptoExceptions.h \
    IRMSCryptoEnvironment.h

SOURCES += \
    BlockBasedProtectedStream.cpp \
    CachedBlock.cpp \
    SimpleProtectedStream.cpp \
    CryptoAPI.cpp \
    StdStreamAdapter.cpp \
    IRMSCryptoEnvironment.cpp
