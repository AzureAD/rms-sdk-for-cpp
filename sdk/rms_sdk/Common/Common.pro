REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/rms
TARGET    = modcommon


TEMPLATE  = lib
QT       += core

CONFIG   += staticlib warn_on c++11 debug_and_release

win32:INCLUDEPATH += $$REPO_ROOT/third_party/include

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

HEADERS += CommonTypes.h \
    FrameworkSpecificTypes.h \
    tools.h \
    Constants.h

SOURCES += \
    tools.cpp \
    Constants.cpp \
    Constants.cpp
