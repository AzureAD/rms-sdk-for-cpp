#!python
import os
import os.path
import sys
from os.path import expanduser

home = expanduser("~")
qt_dir = home + '/Qt/5.7/gcc_64'
qt_include_path = [
      qt_dir + 'mkspecs/linux-g++',
]

include_path_linux = [
    '/usr/include/glib-2.0/',
    '/usr/include/libsecret-1/',
    '/usr/lib/x86_64-linux-gnu/glib-2.0/include/',
    '/usr/lib64',
]

lib_path = [
    '/usr/lib/x86_64-linux-gnu/',
    '/home/admuller/Qt/5.7/gcc_64/lib/',
    '/usr/lib64',
]

def get_vars(isX86):
  print "Settings linux vars"
  if isX86:
    target_arch = "i386"
  else:
    target_arch = "x86_64"
  return (target_arch)

def get_flags_linux(isRelease):
  print "Setup flags linux"
  ccflags = '-pipe -Wall -W -D_REENTRANT -fPIC'
  cxxflags = '-pipe -std=gnu++11 -Wall -W -D_REENTRANT -fPIC'
  if isRelease:
    linkflags = '-Wl,-O1 -Wl,-rpath,' + qt_dir + '/lib'
    ccflags += ' -O2'
    cxxflags += ' -O2'
  else:
    linkflags = '-Wl,-rpath,' + qt_dir + '/lib'
    ccflags += ' -DQT_QML_DEBUG -g'
    cxxflags += ' -DQT_QML_DEBUG -g'
  return (ccflags, cxxflags, linkflags)

def get_qtvars():
  return (qt_dir, qt_include_path)

# libxml2flavor = ''

# libxml2headerpath = "#src/external/libxml2/" + libxml2flavor + "/include/libxml2"
