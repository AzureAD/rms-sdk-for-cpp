REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin
TARGET    = rmsauthWebAuthDialog

QT       += core widgets webenginewidgets network

TEMPLATE  = lib

DEFINES  += RMSAUTH_WEB_AUTH_DIALOG_LIBRARY

CONFIG   += plugin c++11 debug_and_release warn_on

INCLUDEPATH = ../rmsauth/rmsauth

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

SOURCES +=\
    Dialog.cpp \
    Autosaver.cpp \
    CookieJar.cpp \
    RequestInterceptor.cpp

HEADERS  += Dialog.h \
    Autosaver.h \
    CookieJar.h \
    RequestInterceptor.h

FORMS    += Dialog.ui

RESOURCES = res.qrc

unix {
    contains(QMAKE_HOST.arch, x86_64) {
        target.path = /usr/lib64
        INSTALLS += target
    } else {
        target.path += /usr/lib
        INSTALLS += target
    }
}
