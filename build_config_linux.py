#!python
from os.path import expanduser
from build_config import *

home = expanduser("~")
QT_DIR = home + '/Qt/5.7/gcc_64'

CCFLAGS = CCFLAGS_BASE + ' -pipe -Wall -W -D_REENTRANT -fPIC'
CCFLAGS_32 = ''
CCFLAGS_64 = ''
CCFLAGS_RELEASE = CCFLAGS_BASE_RELEASE + ' -O2'
CCFLAGS_DEBUG = CCFLAGS_BASE_DEBUG + ' -DQT_QML_DEBUG -g'

CXXFLAGS = CXXFLAGS_BASE + ' -pipe -std=gnu++11 -Wall -W -D_REENTRANT -fPIC'
CXXFLAGS_32 = ''
CXXFLAGS_64 = ''
CXXFLAGS_RELEASE = CXXFLAGS_BASE_RELEASE + ' -O2'
CXXFLAGS_DEBUG = CXXFLAGS_BASE_DEBUG + ' -DQT_QML_DEBUG -g'

DELETE = 'rm -rf'

LINKFLAGS_RELEASE = '-Wl,-O1 -Wl,-rpath,' + QT_DIR + '/lib'
LINKFLAGS_DEBUG = '-Wl,-rpath,' + QT_DIR + '/lib'

LIB_PATH = [
    '/usr/lib/x86_64-linux-gnu/',
    '/home/admuller/Qt/5.7/gcc_64/lib/',
    '/usr/lib64',
]
LIB_SUFFIX_RELEASE = ''
LIB_SUFFIX_DEBUG = ''

LIBXML2FLAVOR = "libxml2-2.9.3-win32-x86_64"
LIBXML2HEADERPATH = "#src/external/libxml2/" + LIBXML2FLAVOR + "/include/libxml2"

INCLUDE_PATH = INCLUDE_PATH_BASE + [
    '/usr/include/glib-2.0/',
    '/usr/include/libsecret-1/',
    '/usr/lib/x86_64-linux-gnu/glib-2.0/include/',
    '/usr/lib64',
]

MSVC_14 = ''
MSVC_12 = ''
MSVC_PATH_PREFIX = ''
MSVC_PATH_SUFFIX_32 = ''
MSVC_PATH_SUFFIX_64 = ''

PLATFORM_SLASH = '/'

TARGET_ARCH_32 = 'i386'
TARGET_ARCH_64 = 'x86_64'

QT_DIR_PREFIX = QT_DIR
QT_MKSPECS_PATH = 'linux-g++'
