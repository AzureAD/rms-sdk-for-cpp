#!python
import os
import os.path
import sys

build_base_dir = 'bin'
target_name = 'mip_upe_sdk'

include_path = [
    '#sdk',
    '#googletest',
    '#googletest/include',
    '#third_party',
    '#third_party/include',
]

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
