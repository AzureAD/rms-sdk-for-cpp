TEMPLATE = subdirs

SUBDIRS += \
    CryptoStreams\
    UnitTests
    
UnitTests.depends = CryptoStreams
