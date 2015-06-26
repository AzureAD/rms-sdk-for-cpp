REPO_ROOT = $$PWD/../..
DESTDIR   = $$REPO_ROOT/bin/
TARGET    = rms_sample

TEMPLATE = app

QT      += core gui xml xmlpatterns widgets webkitwidgets network
CONFIG  += c++11 debug_and_release
CONFIG  -= app_bundle

INCLUDEPATH += $$REPO_ROOT/sdk/rmsauth_sdk/rmsauth
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/ModernAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -L$$DESTDIR -lrmsauthd -lrmsauthWebAuthDialogd -lrmsd -lrmscryptod
}else {
    LIBS +=  -L$$DESTDIR -lrmsauth -lrmsauthWebAuthDialog -lrms -lrmscrypto
}

SOURCES +=\
    main.cpp\
    mainwindow.cpp \
    pfileconverter.cpp \
    rightsdialog.cpp

HEADERS +=\
    mainwindow.h \
    pfileconverter.h \
    rightsdialog.h

FORMS +=\
    mainwindow.ui \
    templatesdialog.ui \
    rightsdialog.ui

OTHER_FILES += \
    data/test.txt
