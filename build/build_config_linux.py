#!python
from os.path import expanduser
from build_config import *

home = expanduser("~")
QT_DIR_DEFAULT = home + '/Qt/5.7/gcc_64'

CCFLAGS = CCFLAGS_BASE + ' -pipe -Wall -W -D_REENTRANT -fPIC'
CCFLAGS_x86 = ''
CCFLAGS_x64 = ''
CCFLAGS_RELEASE = CCFLAGS_BASE_RELEASE + ' -O2'
CCFLAGS_DEBUG = CCFLAGS_BASE_DEBUG + ' -DQT_QML_DEBUG -g'

CXXFLAGS = CXXFLAGS_BASE + ' -pipe -std=gnu++11 -Wall -W -D_REENTRANT -fPIC'
CXXFLAGS_x86 = ''
CXXFLAGS_x64 = ''
CXXFLAGS_RELEASE = CXXFLAGS_BASE_RELEASE + ' -O2'
CXXFLAGS_DEBUG = CXXFLAGS_BASE_DEBUG + ' -DQT_QML_DEBUG -g'

DELETE = 'rm -rf'

LINKFLAGS_RELEASE = '-Wl,-O1 -Wl,-rpath,'
LINKFLAGS_DEBUG = '-Wl,-rpath,'

LIB_PATH = [
    '/usr/lib/x86_64-linux-gnu/',
    '/usr/lib64',
]
LIB_SUFFIX_RELEASE = ''
LIB_SUFFIX_DEBUG = ''

INCLUDE_PATH = INCLUDE_PATH_BASE + [
    '/usr/include/glib-2.0/',
    '/usr/include/libsecret-1/',
    '/usr/lib/x86_64-linux-gnu/glib-2.0/include/',
    '/usr/lib64',
]

MSVC_14 = ''
MSVC_12 = ''
MSVC_PATH_PREFIX = ''
MSVC_PATH_SUFFIX_x86 = ''
MSVC_PATH_SUFFIX_x64 = ''

PLATFORM_SLASH = '/'

TARGET_ARCH_x86 = 'i386'
TARGET_ARCH_x64 = 'x86_64'

QT_MKSPECS_PATH = 'linux-g++'
