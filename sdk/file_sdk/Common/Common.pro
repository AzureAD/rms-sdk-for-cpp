REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/file
TARGET    = modfilecommon

TEMPLATE  = lib
CONFIG += staticlib warn_on c++11 debug_and_release
INCLUDEPATH += $$REPO_ROOT/sdk/file_sdk
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/Profile

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

SOURCES += \
    tag.cpp \
    file_format.cpp \
    file_format_factory.cpp \
    std_stream_adapter.cpp \
    string_utils.cpp

HEADERS += \
    extended_property.h \
    macros.h \
    tag.h \
    ifile_format.h \
    file_format.h \
    file_format_factory.h \
    istream.h \
    std_stream_adapter.h \
    exceptions.h \
    string_utils.h

