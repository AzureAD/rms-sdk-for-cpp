REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/filesdk
TARGET    = modfilecommon


TEMPLATE  = lib
CONFIG += staticlib warn_on c++11 debug_and_release

SOURCES += \
    string_utils.cpp \
    tag.cpp \
    file_format_base.cpp

HEADERS += \
    extended_property.h \
    macros.h \
    string_utils.h \
    tag.h \
    file_format_base.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
