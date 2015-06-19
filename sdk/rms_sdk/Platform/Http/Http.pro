REPO_ROOT = $$PWD/../../../..
DESTDIR   = $$REPO_ROOT/bin/rms/platform
TARGET    = platformhttp

TEMPLATE = lib

DEFINES += QTFRAMEWORK

CONFIG += staticlib warn_on c++11 debug_and_release

QT += core network
QT -= gui

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

SOURCES += \
    HttpClientQt.cpp \
    UriQt.cpp \
    DnsServerResolverQt.cpp

HEADERS += \
    IUri.h \
    IHttpClient.h \
    IDnsServerResolver.h \
    HttpClientQt.h \
    UriQt.h \
    DnsServerResolverQt.h \
    mscertificates.h
