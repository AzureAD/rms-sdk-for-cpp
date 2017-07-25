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
    src/gtest.cc \
    src/gtest_main.cc \
    src/gtest-all.cc \
    src/gtest-death-test.cc \
    src/gtest-filepath.cc \
    src/gtest-internal-inl.h \
    src/gtest-port.cc \
    src/gtest-printers.cc \
    src/gtest-test-part.cc \
    src/gtest-typed-test.cc

HEADERS += \
    src/gtest-internal-inl.h
