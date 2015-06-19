TEMPLATE = subdirs

DEFINES += QTFRAMEWORK

SUBDIRS += \
    Crypto \
    CryptoAPI \
    Platform     

CryptoAPI.depends = Crypto
Crypto.depends = Platform
