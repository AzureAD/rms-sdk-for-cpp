TEMPLATE = subdirs

SUBDIRS += \
    Common \
    Consent \
    Core \
    Json \
    logger \
    ModernAPI \
    PFile \
    Platform \
    RestClients \
    UnitTests

UnitTests.depends   = ModernAPI
RestClients.depends = PFile
ModernAPI.depends   = Common Core Json Consent RestClients logger
Common.depends      = Platform
