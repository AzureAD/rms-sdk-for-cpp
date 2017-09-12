#!python
import os
import os.path
import sys

include_search_path = [
    '#sdk',
    '#third_party',
    '#third_party/include',
    '#googletest',
    '#googletest/include',
]

def setup_vars(ix86, platform, msvc12):
  if isX86:
    if platform == 'win32':
        target_arch = "x86"
        arch_suffix = ''
        win_def = 'WIN32'
    else:
        target_arch = 'i386'
  else:
    if platform == 'win32':
        target_arch = "amd64"
        arch_suffix = '_64'
        win_def = 'WIN64'
    else:
        target_arch = 'x86_64'
  msvc_version = '14.0'
  msvc_dir = 'msvc14'
  msvc_path = 'msvc2015'
  if msvc12:
    msvc_dir = 'msvc12'
    msvc_version = '12.0'
    msvc_path = 'msvc2013'

qt_dir = 'C:/Qt/5.7/' + msvc_path + arch_suffix
qt_inc_dir = qt_dir + '/include'
qt_bin_dir = qt_dir + '/bin'
qt_lib_path = qt_dir + '/lib'
qt_include_search_path = [
    qt_dir + '/mkspecs/win32-msvc2015',
]

libxml2flavor = "libxml2-2.9.3-win32-x86_64"

libxml2headerpath = "#src/external/libxml2/" + libxml2flavor + "/include/libxml2"
