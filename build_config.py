#!python
import os
import os.path
import sys

build_base_dir = 'bin'
target_name = 'mip_upe_sdk'

INCLUDE_PATH_BASE = [
    '#sdk',
    '#googletest',
    '#googletest/include',
    '#third_party',
    '#third_party/include',
    '#googlemock',
    '#googlemock/include',
]

LIBXML2FLAVOR = "libxml2-2.9.3-win32-x86_64"
LIBXML2HEADERPATH = "#src/external/libxml2/" + LIBXML2FLAVOR + "/include/libxml2"

CCFLAGS_BASE = '-DQT_CORE_LIB'
CCFLAGS_BASE_RELEASE = '-DQT_NO_DEBUG -DNDEBUG -DNDEBUG'
CCFLAGS_BASE_DEBUG = '-DDEBUG -D_DEBUG'

CXXFLAGS_BASE = '-DQT_CORE_LIB'
CXXFLAGS_BASE_RELEASE = '-DQT_NO_DEBUG -DNDEBUG -DNDEBUG'
CXXFLAGS_BASE_DEBUG = '-DDEBUG -D_DEBUG'

# Scons does not have built-in support to create multiple object files at the same time.
# This helper fills that gap.
def create_objs(environ, SRCS):
  return [environ.Object(src) for src in SRCS]
