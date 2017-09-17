TEMPLATE = subdirs

SUBDIRS += \
    Common \
    Consent \
    Core \
    Json \
    ModernAPI \
    PFile \
    Platform \
    RestClients \
    UnitTests \
    Pole \
    OfficeProtector \
    FileAPI

UnitTests.depends   = ModernAPI
RestClients.depends = PFile
ModernAPI.depends   = Common Core Json Consent RestClients
Common.depends      = Platform
FileAPI.depends     = ModernAPI Pole OfficeProtector
