REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/rms
TARGET    = modconsent

QT        -= gui
TEMPLATE   = lib
CONFIG    += staticlib warn_on c++11 debug_and_release

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}
SOURCES += IConsentManager.cpp \
    DocumentTrackingConsentManager.cpp \
    ServiceUrlConsentManager.cpp \
    ServiceUrlConsent.cpp \
    DocumentTrackingConsent.cpp \
    ConsentDBHelper.cpp

HEADERS += IConsentManager.h \
    DocumentTrackingConsentManager.h \
    ServiceUrlConsentManager.h \
    ServiceUrlConsent.h \
    DocumentTrackingConsent.h \
    Consent.h \
    ConsentDBHelper.h
