REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/tests
TARGET    = rmsauthUnitTests

QT       += core testlib widgets

CONFIG   -= app_bundle
CONFIG   += console c++11 debug_and_release warn_on

DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES += Q_COMPILER_INITIALIZER_LISTS

unix:!mac:LIBS  += -lssl -lcrypto -lsecret-1 -lglib-2.0

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -L$$REPO_ROOT/bin -lrmsauthd -lrmsauthWebAuthDialogd -lrmscryptod
    win32:LIBS +=  -L$$REPO_ROOT/log4cplus/lib/Debug/ -llog4cplusUD -llog4cplus-Qt5DebugAppender
} else {
    LIBS +=  -L$$REPO_ROOT/bin -lrmsauth -lrmsauthWebAuthDialog -lrmscrypto
    win32:LIBS +=  -L$$REPO_ROOT/log4cplus/lib/Release/ -llog4cplusU -llog4cplus-Qt5DebugAppender
}

INCLUDEPATH = ../rmsauth/rmsauth
win32:INCLUDEPATH += $$REPO_ROOT/log4cplus/include

TEMPLATE = app

SOURCES += \
    main.cpp \
    NonInteractiveTests.cpp\
    InteractiveTests.cpp

HEADERS += \
    NonInteractiveTests.h\
    InteractiveTests.h
