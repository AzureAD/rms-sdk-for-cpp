REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/file
TARGET    = modopcfile

TEMPLATE = lib
CONFIG += staticlib warn_on c++11 debug_and_release
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI $$REPO_ROOT/sdk/file_sdk $$REPO_ROOT/sdk/file_sdk/Common
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/Profile

win32:INCLUDEPATH += $$REPO_ROOT/third_party/include/Libgsf
win32:INCLUDEPATH += $$REPO_ROOT/third_party/include

win32:LIBS += -L$$REPO_ROOT/third_party/lib/Libgsf -llibgsf-1-114 -lzlib1 -llibgobject-2.0-0 -llibglib-2.0-0 -llibbz2-1

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    win32:LIBS += -L$$REPO_ROOT/third_party/lib/libxml/debug/x64 -llibxml2_a_dll
} else {
    win32:LIBS += -L$$REPO_ROOT/third_party/lib/libxml/release/x64 -llibxml2_a_dll
}

SOURCES += opc_file_format.cpp \
    zip_file.cpp \
    xml/contenttypes.cpp \
    xml/relationships.cpp \
    xml/custom_properties.cpp \
    xml/opc_xml.cpp \
    xml/xml_document.cpp \
    xml/xml_helper.cpp \
    xml/xml_node.cpp

HEADERS += opc_file_format.h \
    zip_file.h \
    xml/contenttypes.h \
    xml/relationships.h \
    xml/custom_properties.h \
    xml/opc_xml.h \
    xml/xml_document.h \
    xml/xml_helper.h \
    xml/xml_node.h

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += libgsf-1
}
