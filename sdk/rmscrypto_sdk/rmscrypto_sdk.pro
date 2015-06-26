TEMPLATE = subdirs
DEFINES += QTFRAMEWORK


SUBDIRS += \
    Crypto \
    CryptoAPI \
    Platform \
    UnitTests

CryptoAPI.depends  = Crypto
Crypto.depends     = Platform
UnitTests.depends  = CryptoAPI
