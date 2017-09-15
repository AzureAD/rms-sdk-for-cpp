#!python
import sys
import os
import os.path
from os.path import expanduser
from build_config import *
from build_support import *

Help("""
Type: 'scons' to build on debug mode,
      'scons --release' to build the release version.
      'scons --x86' to build x86 target.
      'scons --package' to build a binary drop.
      'scons --msvc12' to build with msvc2012.
      'scons --sdk' to the rms sdk.
      'scons --samples' to build the samples.""")

#run scons --release in order to get it to build release mode, default is debug
AddOption(
    '--release',
    action='store_true',
    help='release',
    default=False)
#run scons --x86 in order to get it to build x86, default is x64
AddOption(
    '--x86',
    action='store_true',
    help='x86',
    default=False)
#run scons --package in order to build binary drop
AddOption(
    '--package',
    action='store_true',
    help='package',
    default=False)
#run scons --msvc12.0 in order to build win32 with msvc 12.0
AddOption(
    '--msvc12',
    action='store_true',
    help='msvc12',
    default=False)
#run scons --sdk in order to build the sdk
AddOption(
    '--sdk',
    action='store_true',
    help='sdk',
    default=False)
#run scons --samples in order to build the samples
AddOption(
    '--samples',
    action='store_true',
    help='samples',
    default=False)

isX86 = GetOption('x86')
build_arch = "x86" if isX86 else "x64"
isRelease = GetOption('release')
build_flavor = "release" if isRelease else "debug"
platform = sys.platform
msvc12 = GetOption('msvc12')
sdk = GetOption('sdk')
samples = GetOption('samples')


configuration = ''
lib_suffix = ''
msvc_version = ''
# TODO: move to build_config_<platform
if platform == 'win32':
  print "AAAAAA"
  from build_config_win32 import get_vars, get_qtvars, lib_path
  (arch_suffix, target_arch, win_def, msvc) = get_vars(isX86, msvc12)
  msvc_version = msvc + '.0'
  msvc_dir = 'msvc' + msvc
  msvc_path = msvc + 1
  msvc_path = 'msvc' + str(msvc_path)
  libxml2flavor = "libxml2-2.9.3-win32-x86_64"
  libxml2headerpath = "#src/external/libxml2/" + libxml2flavor + "/include/libxml2"
  (qt_dir, qt_include_path) = get_qtvars()
  print "MSVC: ", msvc_version
elif platform == 'linux2':
  print "BBBBB"
  from build_config_linux import include_path_linux, get_vars, get_qtvars, lib_path
  target_arch = get_vars(isX86)
  include_path += include_path_linux
  (qt_dir, qt_include_path) = get_qtvars()

# TODO: move to build_config_<platform>
home = expanduser("~")
qt_dir = home + '/Qt/5.7/gcc_64'
qt_inc_dir = qt_dir + '/include'
qt_bin_dir = qt_dir + '/bin'
qt_lib_path = qt_dir + '/lib'
qt_include_path += [
    qt_inc_dir,
    qt_inc_dir + '/QtCore',
    qt_inc_dir + '/QtTest',
    qt_inc_dir + '/QtWidgets',
    qt_inc_dir + '/QtGui',
    qt_inc_dir + '/QtANGLE',
    qt_inc_dir + '/QtNetwork',
    qt_inc_dir + '/QtXml',
    qt_inc_dir + '/QtXmlPatterns',
    qt_dir + 'mkspecs/linux-g++',
]

lib_path += [
    qt_lib_path, 
    '#bin/' + build_flavor + '/' + target_arch + '/sdk',
]
print "QT_Dir: ", qt_dir

env = Environment(BUILDROOT=build_base_dir, MSVC_VERSION=msvc_version, TARGET_ARCH=target_arch)
#--------------------------------------------------------------
def DumpEnv( env, key = None, header = None, footer = None ):
    """
    Using the standard Python pretty printer, dump the contents of the
    scons build environment to stdout.

    If the key passed in is anything other than 'env', then that will
    be used as an index into the build environment dictionary and
    whatever is found there will be fed into the pretty printer. Note
    that this key is case sensitive.

    The header and footer are simple mechanisms to allow printing a
    prefix and suffix to the contents that are dumped out. They are
    handy when using DumpEnv to dump multiple portions of the
    environment.
    """
    import pprint
    pp = pprint.PrettyPrinter( indent = 2 )
    if key:
        dict = env.Dictionary( key )
    else:
        dict = env.Dictionary()
    if header:
        print header
    pp.pprint( dict )
    if footer:
      print footer
        
#Call scons dump=env for the entire dump or scons dump=<flag> for specific dump
if 'dump' in ARGUMENTS:
  env_key = ARGUMENTS[ 'dump' ]
  if env_key == 'env':
    prefix = 'env.Dictionary()'
    env_key = None
  else:
    prefix = 'env.Dictionary( ' + env_key + ' )'
  DumpEnv( env,
      key = env_key,
      header = prefix + ' - start',
      footer = prefix + ' - end' )
#---------------------------------------------------------------
#run ParseConfig(env, <command>, <option>) to personalize the env
env.Append(CPPPATH = include_path + qt_include_path)
env.Append(CPPPATH = qt_inc_dir, LIBPATH = [qt_bin_dir])
env.Append(CPPPATH = qt_bin_dir)

(ccflags, cxxflags, linkflags) = get_flags(isRelease)
env.Append(CCFLAGS=Split(ccflags))
env.Append(CXXFLAGS=Split(cxxflags))
env.Append(LINKFLAGS=Split(linkflags))

print env['CCFLAGS']
print env['CXXFLAGS']
print env['LINKFLAGS']

if platform == 'linux2':
  env.Append(CPPDEFINES = { 'LD_LIBRARY_PATH' : qt_lib_path + '/:$LD_LIBRARY_PATH' })

if platform == 'win32' and not isRelease:
      lib_suffix = 'd'

bins = env['BUILDROOT'] + "/" + build_flavor + "/" + target_arch

Export("""
    bins
    build_arch
    build_flavor
    env
    lib_suffix
    lib_path
    qt_lib_path
    target_arch
    target_name
    platform
""")

if samples:
  env.SConscript('samples/SConscript',variant_dir = bins + '/samples', duplicate=0)
else:
  env.SConscript('sdk/SConscript',variant_dir = bins + '/sdk', duplicate=0)
