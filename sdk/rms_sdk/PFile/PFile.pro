REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/rms
TARGET    = modprotectedfile

TEMPLATE = lib
CONFIG += staticlib warn_on c++11 debug_and_release


INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoStreams/CryptoAPI
QT       += core
QT 	 -= gui

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

SOURCES += PfileHeader.cpp \
        PfileHeaderReader.cpp \
        PfileHeaderWriter.cpp

HEADERS += IPfileHeaderReader.h \
	IPfileHeaderWriter.h \
	PfileHeader.h \
        PfileHeaderReader.h \
        PfileHeaderWriter.h

QMAKE_CFLAGS_WARN_ON -= -W3
QMAKE_CFLAGS_WARN_ON += -W4
