REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/rms
TARGET    = modrestclients

QT -= gui
QT += core xml network

TEMPLATE = lib
CONFIG  += staticlib c++11 debug_and_release

INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

SOURCES += \
    UsageRestrictionsClient.cpp \
    RestServiceUrlClient.cpp \
    RestHttpClient.cpp \
    AuthenticationHandler.cpp \
    RestServiceUrls.cpp \
    LicenseParser.cpp \
    Domain.cpp \
    CXMLUtils.cpp \
    DnsClientResult.cpp \
    DnsLookupClient.cpp \
    RestClientErrorHandling.cpp \
    ServiceDiscoveryClient.cpp \
    RestClientCache.cpp \
    TemplatesClient.cpp \
    PublishClient.cpp

HEADERS += \
    RestObjects.h \
    IUsageRestrictionsClient.h \
    IPublishClient.h \
    UsageRestrictionsClient.h \
    IRestServiceUrlClient.h \
    RestServiceUrlClient.h \
    ServiceDiscoveryDetails.h \
    RestHttpClient.h \
    AuthenticationHandler.h \
    RestServiceUrls.h \
    LicenseParser.h \
    Domain.h \
    CXMLUtils.h \
    DnsClientResult.h \
    DnsLookupClient.h \
    IDnsLookupClient.h \
    RestClientErrorHandling.h \
    IRestClientCache.h \
    RestClientCache.h \
    IServiceDiscoveryClient.h \
    ServiceDiscoveryClient.h \
    TemplatesClient.h \
    ITemplatesClient.h \
    PublishClient.h
