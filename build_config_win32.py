#!python
from build_config import *

CCFLAGS = CCFLAGS_BASE + ' -nologo -Zc:wchar_t -FS -Zc:strictStrings -W3 -w44456 -w44457 -w44458 \
      -DUNICODE'
CCFLAGS_x32 = '-DWIN32'
CCFLAGS_x64 = '-DWIN64'
CCFLAGS_RELEASE = CCFLAGS_BASE_RELEASE + ' -c -O2 -MD'
CCFLAGS_DEBUG = CCFLAGS_BASE_DEBUG + ' -c -Zi -MDd'

CXXFLAGS = CXXFLAGS_BASE + ' -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -GR -EHsc \
    -DUNICODE'
CXXFLAGS_x32 = '-DWIN32'
CXXFLAGS_x64 = '-DWIN64'
CXXFLAGS_RELEASE = CXXFLAGS_BASE_RELEASE + ' -c -O2 -MD'
CXXFLAGS_DEBUG = CXXFLAGS_BASE_DEBUG + ' -c -Zi -MDd'

DELETE = 'rmdir /Q /S'

INCLUDE_PATH = INCLUDE_PATH_BASE

LINKFLAGS_RELEASE = '/NOLOGO /DYNAMICBASE /NXCOMPAT /INCREMENTAL:NO /SUBSYSTEM:CONSOLE'
LINKFLAGS_DEBUG = '/NOLOGO /DEBUG'

LIB_SUFFIX_RELEASE = ''
LIB_SUFFIX_DEBUG = 'd'

LIB_PATH = [
    '#third_party/lib/eay',
]

MSVC_14 = '14'
MSVC_12 = '12'
MSVC_PATH_PREFIX = 'msvc20'
MSVC_PATH_SUFFIX_x32 = ''
MSVC_PATH_SUFFIX_x64 = '_64'

PLATFORM_SLASH = '\\'

TARGET_ARCH_x32 = 'x86'
TARGET_ARCH_x64 = 'amd64'

QT_DIR_DEFAULT = 'C:/Qt/5.7'
QT_MKSPECS_PATH = 'win32-msvc20'
