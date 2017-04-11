REPO_ROOT = $$PWD/../..
DESTDIR   = $$REPO_ROOT/bin/
TARGET    = rms_sample

TEMPLATE = app

QT      += core gui xml xmlpatterns widgets webenginewidgets network
CONFIG  += c++11 debug_and_release
CONFIG  -= app_bundle

DEFINES += Q_COMPILER_INITIALIZER_LISTS

INCLUDEPATH += $$REPO_ROOT/sdk/rmsauth_sdk/rmsauth
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/ModernAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI
win32:INCLUDEPATH += $$REPO_ROOT/log4cplus/include

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    LIBS +=  -L$$DESTDIR -lrmsauthd -lrmsauthWebAuthDialogd -lrmsd -lrmscryptod
    win32:LIBS +=  -L$$REPO_ROOT/log4cplus/lib/Debug/ -llog4cplusUD -llog4cplus-Qt5DebugAppender
}else {
    LIBS +=  -L$$DESTDIR -lrmsauth -lrmsauthWebAuthDialog -lrms -lrmscrypto
    win32:LIBS +=  -L$$REPO_ROOT/log4cplus/lib/Release/ -llog4cplusU -llog4cplus-Qt5DebugAppender
}

SOURCES +=\
    main.cpp\
    mainwindow.cpp \
    pfileconverter.cpp \
    rightsdialog.cpp \
    consentlistdialog.cpp

HEADERS +=\
    mainwindow.h \
    pfileconverter.h \
    rightsdialog.h \
    consentlistdialog.h

FORMS +=\
    mainwindow.ui \
    templatesdialog.ui \
    rightsdialog.ui \
    consentlistdialog.ui

OTHER_FILES += \
    data/test.txt
