REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/rms
TARGET    = modcommon


TEMPLATE  = lib
QT       += core

CONFIG   += staticlib warn_on c++11 debug_and_release

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

HEADERS += CommonTypes.h \
    FrameworkSpecificTypes.h \
    tools.h

SOURCES += \
    tools.cpp
