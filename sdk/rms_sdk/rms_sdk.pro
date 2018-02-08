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
    FileAPI \
    PDFObjectModel

UnitTests.depends   = ModernAPI FileAPI
RestClients.depends = PFile
ModernAPI.depends   = Common Core Json Consent RestClients
Common.depends      = Platform
FileAPI.depends     = ModernAPI Pole OfficeProtector PDFObjectModel
