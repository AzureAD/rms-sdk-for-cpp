#-------------------------------------------------
#
# Project created by QtCreator 2015-04-01T13:58:04
#
#-------------------------------------------------

QT       += core xml xmlpatterns network webkitwidgets

QT       += gui widgets

TARGET = getsymkeysample
CONFIG   += console c++11 debug_and_release
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

LIBS += -L../../bin/rms/ -lrms_sdk

win32:LIBS += -L../../../lib/ -lssleay32MDd -llibeay32MDd -lGdi32 -lUser32 -lAdvapi32
else:LIBS  += -lssl -lcrypto
