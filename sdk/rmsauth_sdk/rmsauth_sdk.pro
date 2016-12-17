TEMPLATE = subdirs
DEFINES += QTFRAMEWORK

SUBDIRS += \
    WebAuthDialog\
    rmsauth\
    UnitTests

rmsauth.depends   = WebAuthDialog
UnitTests.depends = rmsauth
