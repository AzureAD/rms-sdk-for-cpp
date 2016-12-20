TEMPLATE = subdirs

SUBDIRS += \
    WebAuthDialog\
    rmsauth\
    UnitTests

rmsauth.depends   = WebAuthDialog
UnitTests.depends = rmsauth
