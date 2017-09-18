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
]

LIBXML2FLAVOR = "libxml2-2.9.3-win32-x86_64"
LIBXML2HEADERPATH = "#src/external/libxml2/" + LIBXML2FLAVOR + "/include/libxml2"

CCFLAGS_BASE = '-DQT_CORE_LIB'
CCFLAGS_BASE_RELEASE = '-DQT_NO_DEBUG -DNDEBUG -DNDEBUG'
CCFLAGS_BASE_DEBUG = '-DDEBUG -D_DEBUG'

CXXFLAGS_BASE = '-DQT_CORE_LIB'
CXXFLAGS_BASE_RELEASE = '-DQT_NO_DEBUG -DNDEBUG -DNDEBUG'
CXXFLAGS_BASE_DEBUG = '-DDEBUG -D_DEBUG'

def get_vars(isX86, isRelease, platform, msvc12):
  global target_arch
  global include_path
  global libxml2headerpath
  global lib_path
  if platform == 'win32':
    pass
  elif platform == 'linux2':
    from build_config_linux import include_path_linux, get_vars, get_qtvars, lib_path
    target_arch = get_vars(isX86)
    include_path += include_path_linux
    (qt_dir, qt_include_path) = get_qtvars()
    lib_path = lib_path_linux

def get_flags(isRelease):
  print "get_flags"
  ccflags = '-DQT_CORE_LIB'
  cxxflags = '-DQT_CORE_LIB'
  if isRelease:
    ccflags += ' -DQT_NO_DEBUG -DNDEBUG -DNDEBUG'
    cxxflags += ' -DQT_NO_DEBUG -DNDEBUG -DNDEBUG'
  else:
    ccflags += ' -DDEBUG -D_DEBUG'
    cxxflags += ' -DDEBUG -D_DEBUG'
  print "SYS PLATFORM: ", sys.platform
  if sys.platform == 'win32':
    from build_config_win32 import get_flags_win32
    (ccflags_, cxxflags_, linkflags_) = get_flags_win32(isRelease)
  elif sys.platform == 'linux2':
    from build_config_linux import get_flags_linux
    (ccflags_, cxxflags_, linkflags_) = get_flags_linux(isRelease)
  return (ccflags + ' ' + ccflags_, cxxflags + ' ' + cxxflags_, linkflags_)

# Scons does not have built-in support to create multiple object files at the same time.
# This helper fills that gap.
def create_objs(environ, SRCS):
  return [environ.Object(src) for src in SRCS]
