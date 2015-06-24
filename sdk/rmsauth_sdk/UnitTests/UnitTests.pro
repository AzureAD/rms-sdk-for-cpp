REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/tests
TARGET    = rmsauthUnitTests

QT       += core testlib widgets

CONFIG   -= app_bundle
CONFIG   += console c++11 debug_and_release warn_on

DEFINES += SRCDIR=\\\"$$PWD/\\\"

unix:!mac:LIBS  += -lssl -lcrypto -lsecret-1 -lglib-2.0

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -L$$REPO_ROOT/bin -lrmsauthd -lrmsauthWebAuthDialogd -lrmscryptod
} else {
    LIBS +=  -L$$REPO_ROOT/bin -lrmsauth -lrmsauthWebAuthDialog -lrmscrypto
}

INCLUDEPATH = ../rmsauth/rmsauth

TEMPLATE = app

SOURCES += \
    main.cpp \
    NonInteractiveTests.cpp\
    InteractiveTests.cpp

HEADERS += \
    NonInteractiveTests.h\
    InteractiveTests.h
