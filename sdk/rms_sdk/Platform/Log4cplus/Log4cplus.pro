REPO_ROOT = $$PWD/../../../..
DESTDIR   = $$REPO_ROOT/bin/rms/platform
TARGET    = log4cpluslogger

QT += core
QT -= gui

DEFINES += QTFRAMEWORK

CONFIG += staticlib warn_on c++11 debug_and_release

TEMPLATE = lib

win32:INCLUDEPATH += $$REPO_ROOT/log4cplus/include

win32:LIBS += -L$$REPO_ROOT/log4cplus/lib/ -llog4cplus-Qt5DebugAppender

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    win32:LIBS += -llog4cplusUD
}else {
    win32:LIBS += -llog4cplusU
}

SOURCES += \
    Log4cplusImpl.cpp \
    StaticLogger.cpp


HEADERS += \
    Log4cplusImpl.h \
    StaticLogger.h


DISTFILES += \
    Log4cplus.properties



