REPO_ROOT = $$PWD/../../../..
DESTDIR   = $$REPO_ROOT/bin/rms/platform
TARGET    = log4cpluslogger

QT += core
QT -= gui

DEFINES += QTFRAMEWORK

CONFIG += staticlib warn_on c++11 debug_and_release

TEMPLATE = lib

win32:INCLUDEPATH += $$REPO_ROOT/log4cplus/include
unix:!mac:INCLUDEPATH += /usr/local/include/log4cplus
unix:!mac:LIBS += -L/usr/local/lib/log4cplus -llog4cplus

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    win32:LIBS += -L$$REPO_ROOT/log4cplus/lib/Debug/ -llog4cplusUD -llog4cplus-Qt5DebugAppender
}else {
    win32:LIBS += -L$$REPO_ROOT/log4cplus/lib/Release/ -llog4cplusU -llog4cplus-Qt5DebugAppender
}

SOURCES += \
    Log4cplusImpl.cpp


HEADERS += \
    Log4cplusImpl.h \
    StaticLogger.h



