REPO_ROOT = $$PWD/../..
DESTDIR   = $$REPO_ROOT/bin/
TARGET    = rmsauth_sample

QT       += core gui widgets
TEMPLATE = app
CONFIG  += c++11 debug_and_release warn_on
CONFIG  -= app_bundle

INCLUDEPATH = $$REPO_ROOT/sdk/rmsauth_sdk/rmsauth
win32:INCLUDEPATH += $$REPO_ROOT/log4cplus/include

SOURCES += main.cpp\
        MainWindow.cpp

HEADERS  += MainWindow.h

FORMS    += MainWindow.ui

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -L$$DESTDIR -lrmsauthd -lrmsauthWebAuthDialogd -lrmscryptod
    win32:LIBS +=  -L$$REPO_ROOT/log4cplus/lib/Debug/ -llog4cplusUD -llog4cplus-Qt5DebugAppender
} else {
    LIBS +=  -L$$DESTDIR -lrmsauth -lrmsauthWebAuthDialog -lrmscrypto
    win32:LIBS +=  -L$$REPO_ROOT/log4cplus/lib/Release/ -llog4cplusU -llog4cplus-Qt5DebugAppender
}
