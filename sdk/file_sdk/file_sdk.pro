TEMPLATE = subdirs

SUBDIRS += UnitTests \
    Common \
    XMPFileFormat \
    UnitTests \
    CompoundFileFormat \
    OPCFileFormat \
    PDFFileFormat \
    PFileFileFormat \
    DefaultFormat

UnitTests.depends   = Common XMPFileFormat CompoundFileFormat DefaultFormat OPCFileFormat PDFFileFormat PFileFileFormat
