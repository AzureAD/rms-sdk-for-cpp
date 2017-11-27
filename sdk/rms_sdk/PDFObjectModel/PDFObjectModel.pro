REPO_ROOT = $$PWD/../../..
DESTDIR   = $$REPO_ROOT/bin
TARGET    = pdfobjectmodel

TEMPLATE = lib
CONFIG  += plugin c++11 debug_and_release warn_on

DEFINES     += RMS_LIBRARY

#QMAKE_CFLAGS_WARN_ON -= -W3
#QMAKE_CFLAGS_WARN_ON += -W4

QT       += core xml xmlpatterns widgets network
QT 	 -= gui

#MIP SDK
INCLUDEPATH += $$REPO_ROOT/sdk/rmscrypto_sdk/CryptoAPI
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/ModernAPI

win32:LIBS += \
        -lGdi32\
        -lUser32\
        -lAdvapi32\
        -lws2_32\
        -lshell32\
        -lwsock32\
        -lShlwapi\
        -lmsimg32\
        -lIphlpapi\
        -luuid\
        -lrpcrt4\
        -lwinmm
else:
#end MIP SDK

#fxcore
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/PDFObjectModel/Core/include
win32:
else:
#end fxcore

HEADERS += \
    core.inc \
    pdf_object_model.h \
    pdf_module_mgr.h \
    basic.h \
    pdf_creator.h \
    unencrypted_wrapper.h

SOURCES += \
    Core/src/fdrm/crypto/fx_crypt.cpp \
    Core/src/fdrm/crypto/fx_crypt_aes.cpp \
    Core/src/fdrm/crypto/fx_crypt_sha.cpp \
    Core/src/fpdfapi/fpdf_basic_module.cpp \
    Core/src/fpdfapi/fpdf_edit_create.cpp \
    Core/src/fpdfapi/fpdf_edit_doc.cpp \
    Core/src/fpdfapi/fpdf_parser_decode.cpp \
    Core/src/fpdfapi/fpdf_parser_document.cpp \
    Core/src/fpdfapi/fpdf_parser_encrypt.cpp \
    Core/src/fpdfapi/fpdf_parser_filters.cpp \
    Core/src/fpdfapi/fpdf_parser_objects.cpp \
    Core/src/fpdfapi/fpdf_parser_parser.cpp \
    Core/src/fpdfapi/fpdf_parser_utility.cpp \
    Core/src/fpdfdoc/doc_basic.cpp \
    Core/src/fxcodec/codec/fx_codec.cpp \
    Core/src/fxcodec/codec/fx_codec_flate.cpp \
    Core/src/fxcodec/fx_zlib/src/fx_zlib_adler32.c \
    Core/src/fxcodec/fx_zlib/src/fx_zlib_compress.c \
    Core/src/fxcodec/fx_zlib/src/fx_zlib_crc32.c \
    Core/src/fxcodec/fx_zlib/src/fx_zlib_deflate.c \
    Core/src/fxcodec/fx_zlib/src/fx_zlib_gzclose.c \
    Core/src/fxcodec/fx_zlib/src/fx_zlib_gzlib.c \
    Core/src/fxcodec/fx_zlib/src/fx_zlib_gzread.c \
    Core/src/fxcodec/fx_zlib/src/fx_zlib_gzwrite.c \
    Core/src/fxcodec/fx_zlib/src/fx_zlib_infback.c \
    Core/src/fxcodec/fx_zlib/src/fx_zlib_inffast.c \
    Core/src/fxcodec/fx_zlib/src/fx_zlib_inflate.c \
    Core/src/fxcodec/fx_zlib/src/fx_zlib_inftrees.c \
    Core/src/fxcodec/fx_zlib/src/fx_zlib_trees.c \
    Core/src/fxcodec/fx_zlib/src/fx_zlib_uncompr.c \
    Core/src/fxcodec/fx_zlib/src/fx_zlib_zutil.c \
    Core/src/fxcrt/fx_charmap.cpp \
    Core/src/fxcrt/fxcrt_platforms.cpp \
    Core/src/fxcrt/fxcrt_posix.cpp \
    Core/src/fxcrt/fxcrt_windows.cpp \
    Core/src/fxcrt/fx_basic_array.cpp \
    Core/src/fxcrt/fx_basic_bstring.cpp \
    Core/src/fxcrt/fx_basic_buffer.cpp \
    Core/src/fxcrt/fx_basic_coords.cpp \
    Core/src/fxcrt/fx_basic_gcc.cpp \
    Core/src/fxcrt/fx_basic_list.cpp \
    Core/src/fxcrt/fx_basic_maps.cpp \
    Core/src/fxcrt/fx_basic_memmgr.cpp \
    Core/src/fxcrt/fx_basic_plex.cpp \
    Core/src/fxcrt/fx_basic_utf.cpp \
    Core/src/fxcrt/fx_basic_util.cpp \
    Core/src/fxcrt/fx_basic_wstring.cpp \
    Core/src/fxcrt/fx_extension.cpp \
    pdf_module_mgr.cpp \
    basic.cpp \
    unencrypted_wrapper.cpp \
    pdf_creator.cpp

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
} else {
}

unix {
    contains(QMAKE_HOST.arch, x86_64) {
        target.path = /usr/lib64
        INSTALLS += target
    } else {
        target.path += /usr/lib
        INSTALLS += target
    }
}


