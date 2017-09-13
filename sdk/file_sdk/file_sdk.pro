TEMPLATE = subdirs

SUBDIRS += UnitTests \
    Common \
    XMPFileFormat \
    UnitTests \
    CompoundFileFormat \
    OPCFileFormat \
    PDFFileFormat \
    PFileFileFormat \
    DefaultFormat \
    api \
    api_impl

UnitTests.depends   = Common XMPFileFormat CompoundFileFormat DefaultFormat OPCFileFormat PDFFileFormat PFileFileFormat
api_impl.depends = api Common
