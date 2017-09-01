#!python
import sys
import os
import os.path
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
if platform == 'win32':
  if msvc12:
    msvc_dir = 'msvc12'
    msvc_version = '12.0'
    msvc_path = 'msvc2013'
  print "MSVC: ", msvc_version

qt_dir = 'C:/Qt/5.7/' + msvc_path + arch_suffix
qt_inc_dir = qt_dir + '/include'
qt_bin_dir = qt_dir + '/bin'
qt_lib_path = qt_dir + '/lib'
qt_include_search_path = [
    qt_inc_dir,
    qt_inc_dir + '/QtCore',
    qt_inc_dir + '/QtTest',
    qt_inc_dir + '/QtWidgets',
    qt_inc_dir + '/QtGui',
    qt_inc_dir + '/QtANGLE',
    qt_inc_dir + '/QtNetwork',
    qt_inc_dir + '/QtXml',
    qt_inc_dir + '/QtXmlPatterns',
    qt_dir + '/mkspecs/win32-msvc2015',
]

print "Qt directory: ", qt_dir

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
env.Append(CPPPATH = include_search_path + qt_include_search_path)

if platform == 'win32':
  env.Append(CPPPATH = qt_inc_dir, LIBPATH = [qt_bin_dir])
  env.Append(CPPPATH = qt_bin_dir)
  env.Append(CCFLAGS=Split('-nologo -Zc:wchar_t -FS -Zc:strictStrings -W3 -w44456 -w44457 -w44458 \
      -DUNICODE -D' + win_def + ' -DQT_CORE_LIB'))
  env.Append(CXXFLAGS=Split('-nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -GR -EHsc \
      -DUNICODE -D' + win_def + ' -DQT_CORE_LIB'))
if msvc12:
    env.Append(CPPDEFINES = ['MSVC12'])

configuration = ''
if isRelease:
  configuration = 'release'
  env.Append(CPPDEFINES = ['NDEBUG'])
  
  if platform == 'win32':
    env.Append(LINKFLAGS='/NOLOGO /DYNAMICBASE /NXCOMPAT /INCREMENTAL:NO /SUBSYSTEM:CONSOLE')
    env.Append(CCFLAGS=Split('-c -O2 -MD -DQT_NO_DEBUG -DNDEBUG'))
    env.Append(CXXFLAGS=Split('-c -O2 -MD -DQT_NO_DEBUG -DNDEBUG')) # not including -DRMS_CRYPTO_LIBRARY
    # env.Append(LINKFLAGS='/NOLOGO /RELEASE /OPT:REF /OPT:ICF /DYNAMICBASE /NXCOMPAT /INCREMENTAL:NO /SUBSYSTEM:CONSOLE')
else:    
  configuration = 'debug'
  env.Append(CPPDEFINES =['DEBUG','_DEBUG'])

  if platform == 'win32':
    env.Append(LINKFLAGS='/NOLOGO')
    env.Append(CCFLAGS=Split('-c -Zi -MDd'))
    env.Append(CXXFLAGS=Split('-c -Zi -MDd')) # not including -DRMS_CRYPTO_LIBRARY

bins = env['BUILDROOT'] + "/" + configuration + "/" + target_arch
lib_suffix = ''
if configuration == 'debug':
  lib_suffix = 'd'

Export('arch_suffix bins configuration env lib_suffix qt_lib_path target_name platform')

if samples:
  env.SConscript('samples/SConscript',variant_dir = bins + '/samples', duplicate=0)
else:
  env.SConscript('sdk/SConscript',variant_dir = bins + '/sdk', duplicate=0)
