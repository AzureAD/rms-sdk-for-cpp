REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin
TARGET    = pdfobjectmodel

TEMPLATE = lib
CONFIG  += plugin c++11 debug_and_release warn_on

DEFINES     += RMS_LIBRARY

QMAKE_CFLAGS_WARN_ON -= -W3
QMAKE_CFLAGS_WARN_ON += -W4

QT       += core xml xmlpatterns widgets network
QT 	 -= gui

HEADERS += \
    CoreInc.h \
    PDFObjectModel.h \
    PDFModuleMgr.h \
    Basic.h \
    PDFCreator.h \
    UnencryptedWrapper.h

SOURCES += \
    PDFModuleMgr.cpp \
    Basic.cpp \
    UnencryptedWrapper.cpp \
    PDFCreator.cpp


#MIP SDK
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/ModernAPI

win32:LIBS += \
        -lGdi32\
        -lUser32\
        -lAdvapi32\
        -lws2_32\
        -lshell32\
        -lwsock32\
        -lShlwapi\
        -lmsimg32\
        -lIphlpapi\
        -luuid\
        -lrpcrt4\
        -lwinmm
else:
#end MIP SDK

#fxcore
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/PDFObjectModel/Core/include
win32:LIBS += \
        -L$$REPO_ROOT/sdk/rms_sdk/PDFObjectModel/Core/lib/dbg/mt_x86_vc14 \
        -L$$REPO_ROOT/sdk/rms_sdk/PDFObjectModel/fgas/lib/dbg/x86_vc14 \
        -L$$REPO_ROOT/sdk/rms_sdk/PDFObjectModel/fxgraphics/lib/dbg/x86_vc14 \
        -L$$REPO_ROOT/sdk/rms_sdk/PDFObjectModel/fxjse/lib/dbg/mt_x86_vc14 \
        -lfpdfapi[dbg_mt_x86_vc14]\
        -lfxcrt[dbg_mt_x86_vc14]\
        -lfpdfdoc[dbg_mt_x86_vc14]\
        -lfxmath[dbg_mt_x86_vc14]\
        -lfxge[dbg_mt_x86_vc14]\
        -lfxhal[dbg_mt_x86_vc14]\
        -lfxcodec[dbg_mt_x86_vc14]\
        -lfdrm[dbg_mt_x86_vc14]\
        -lfpdftext[dbg_mt_x86_vc14]\
        -lfxfontmgr[dbg_mt_x86_vc14]\
        -lfgas[dbg_x86_vc14]\
        -lfxgraphics[dbg_x86_vc14]\
        -lfxjse[dbg_mt_x86_vc14]
else:LIBS += \
        -L$$REPO_ROOT/sdk/rms_sdk/PDFObjectModel/Core/lib/dbg/mt_x64_linux_gcc \
        -llinux_dbg_x64
#end fxcore

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
} else {
}

unix {
    contains(QMAKE_HOST.arch, x86_64) {
        target.path = /usr/lib64
        INSTALLS += target
    } else {
        target.path += /usr/lib
        INSTALLS += target
    }
}


