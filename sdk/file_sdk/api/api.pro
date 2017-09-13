REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin/

TEMPLATE = subdirs

INCLUDEPATH = $$REPO_ROOT/sdk/file_sdk $$REPO_ROOT/sdk/file_sdk/Common $$REPO_ROOT/sdk/rms_sdk/ModernAPI

HEADERS += \
    fileapi_export.h \
    label.h \
    istream_handler.h \
    ipolicy_engine.h \
    labeling_options.h

SOURCES +=
