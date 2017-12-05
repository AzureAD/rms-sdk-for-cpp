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
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/PDFObjectModel
INCLUDEPATH += $$REPO_ROOT/sdk/rms_sdk/PDFObjectModel/core/include
win32:
else:
#end fxcore

HEADERS += \
    core.h \
    pdf_object_model.h \
    pdf_module_mgr.h \
    basic.h \
    pdf_creator.h \
    unencrypted_wrapper.h

SOURCES += \
    core/src/fdrm/crypto/fx_crypt.cpp \
    core/src/fdrm/crypto/fx_crypt_aes.cpp \
    core/src/fdrm/crypto/fx_crypt_sha.cpp \
    core/src/fpdfapi/fpdf_basic_module.cpp \
    core/src/fpdfapi/fpdf_edit_create.cpp \
    core/src/fpdfapi/fpdf_edit_doc.cpp \
    core/src/fpdfapi/fpdf_parser_decode.cpp \
    core/src/fpdfapi/fpdf_parser_document.cpp \
    core/src/fpdfapi/fpdf_parser_encrypt.cpp \
    core/src/fpdfapi/fpdf_parser_filters.cpp \
    core/src/fpdfapi/fpdf_parser_objects.cpp \
    core/src/fpdfapi/fpdf_parser_parser.cpp \
    core/src/fpdfapi/fpdf_parser_utility.cpp \
    core/src/fpdfdoc/doc_basic.cpp \
    core/src/fxcodec/codec/fx_codec.cpp \
    core/src/fxcodec/codec/fx_codec_flate.cpp \
    core/src/fxcodec/fx_zlib/src/fx_zlib_adler32.c \
    core/src/fxcodec/fx_zlib/src/fx_zlib_compress.c \
    core/src/fxcodec/fx_zlib/src/fx_zlib_crc32.c \
    core/src/fxcodec/fx_zlib/src/fx_zlib_deflate.c \
    core/src/fxcodec/fx_zlib/src/fx_zlib_gzclose.c \
    core/src/fxcodec/fx_zlib/src/fx_zlib_gzlib.c \
    core/src/fxcodec/fx_zlib/src/fx_zlib_gzread.c \
    core/src/fxcodec/fx_zlib/src/fx_zlib_gzwrite.c \
    core/src/fxcodec/fx_zlib/src/fx_zlib_infback.c \
    core/src/fxcodec/fx_zlib/src/fx_zlib_inffast.c \
    core/src/fxcodec/fx_zlib/src/fx_zlib_inflate.c \
    core/src/fxcodec/fx_zlib/src/fx_zlib_inftrees.c \
    core/src/fxcodec/fx_zlib/src/fx_zlib_trees.c \
    core/src/fxcodec/fx_zlib/src/fx_zlib_uncompr.c \
    core/src/fxcodec/fx_zlib/src/fx_zlib_zutil.c \
    core/src/fxcrt/fx_charmap.cpp \
    core/src/fxcrt/fxcrt_platforms.cpp \
    core/src/fxcrt/fxcrt_posix.cpp \
    core/src/fxcrt/fxcrt_windows.cpp \
    core/src/fxcrt/fx_basic_array.cpp \
    core/src/fxcrt/fx_basic_bstring.cpp \
    core/src/fxcrt/fx_basic_buffer.cpp \
    core/src/fxcrt/fx_basic_coords.cpp \
    core/src/fxcrt/fx_basic_gcc.cpp \
    core/src/fxcrt/fx_basic_list.cpp \
    core/src/fxcrt/fx_basic_maps.cpp \
    core/src/fxcrt/fx_basic_memmgr.cpp \
    core/src/fxcrt/fx_basic_plex.cpp \
    core/src/fxcrt/fx_basic_utf.cpp \
    core/src/fxcrt/fx_basic_util.cpp \
    core/src/fxcrt/fx_basic_wstring.cpp \
    core/src/fxcrt/fx_extension.cpp \
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


