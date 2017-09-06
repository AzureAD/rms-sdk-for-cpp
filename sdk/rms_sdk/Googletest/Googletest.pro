REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/rms/platform
TARGET    = gtest

TEMPLATE = lib

DEFINES += QTFRAMEWORK

CONFIG += staticlib warn_on c++11 debug_and_release

INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/Googletest/include

QT -= gui

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

SOURCES += \
    ../../../../googletest/src/gtest.cc \
    ../../../../googletest/src/gtest_main.cc \
    ../../../../googletest/src/gtest-all.cc \
    ../../../../googletest/src/gtest-death-test.cc \
    ../../../../googletest/src/gtest-filepath.cc \
    ../../../../googletest/src/gtest-internal-inl.h \
    ../../../../googletest/src/gtest-port.cc \
    ../../../../googletest/src/gtest-printers.cc \
    ../../../../googletest/src/gtest-test-part.cc \
    ../../../../googletest/src/gtest-typed-test.cc

HEADERS += \
    ../../../../googletest/src/gtest-internal-inl.h
