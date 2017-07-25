TEMPLATE = subdirs

SUBDIRS += \
    Common \
    Consent \
    Core \
    Googletest \
    Json \
    ModernAPI \
    PFile \
    Platform \
    RestClients \
    UnitTests

UnitTests.depends   = ModernAPI
RestClients.depends = PFile
ModernAPI.depends   = Common Core Json Consent RestClients
Common.depends      = Platform
