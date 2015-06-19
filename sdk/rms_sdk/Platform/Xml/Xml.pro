REPO_ROOT = $$PWD/../../../..
DESTDIR   = $$REPO_ROOT/bin/rms/platform
TARGET    = platformxml

QT       += xml xmlpatterns
QT       -= gui

TEMPLATE = lib
CONFIG += staticlib c++11 debug_and_release

DEFINES += QTFRAMEWORK

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

SOURCES += \
    DomAttributeQt.cpp \
    DomDocumentQt.cpp \
    DomElementQt.cpp \
    DomNodeQt.cpp

HEADERS += \
    IDomDocument.h \
    IDomNode.h \
    IDomElement.h \
    IDomAttribute.h \
    DomNamedNodeMap.h \
    DomNodeList.h \
    DomAttributeQt.h \
    DomDocumentQt.h \
    DomElementQt.h \
    DomNodeQt.h
