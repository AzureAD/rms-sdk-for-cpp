#!python
import sys
import os
from build_support import *

Help("""
Type: 'scons' to build on debug mode,
      'scons --release' to build the release version.
      'scons --x86' to build x86 target.
      'scons --package' to build a binary drop.
      'scons --msvc12' to build with msvc2012.
      'scons --sdk' to the rms sdk.
      'scons --samples' to build the samples.
      'scons --qmake="<qmake path>"'
""")

#run scons --release in order to get it to build release mode, default is debug
AddOption(
    '--configuration',
    choices=['debug','release'],
    help='Configurations: [debug, release]',
    default='debug')
#run scons --x86 in order to get it to build x86, default is x64
AddOption(
    '--arch',
    choices=['x86','x64'],
    help='Architecture: [x86, x64]',
    default='x64')
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
#run scons --samples in order to build the samples
AddOption(
    '--samples',
    action='store_true',
    help='samples',
    default=False)
#run scons --qt in order to specify the qt installation path
AddOption(
    '--qt',
    type='string',
    nargs=1,
    action='store',
    help='qt',
    metavar='DIR')

build_arch = GetOption('arch')
build_flavor = GetOption('configuration').upper()
platform = sys.platform
msvc12 = GetOption('msvc12')
samples = GetOption('samples')
qt_dir = GetOption('qt')

if platform == 'win32':
    from build_config_win32 import *
elif platform == 'linux2':
    from build_config_linux import *

if not qt_dir:
    qt_dir = QT_DIR_DEFAULT 

ccflags = CCFLAGS + ' ' + eval("CCFLAGS_" + build_arch) + ' ' + eval("CCFLAGS_" + build_flavor)
cxxflags = CXXFLAGS + ' ' + eval("CXXFLAGS_" + build_arch) + ' ' + eval("CXXFLAGS_" + build_flavor)
include_path = INCLUDE_PATH
lib_path = LIB_PATH
lib_suffix = eval("LIB_SUFFIX_" + build_flavor)
linkflags = eval("LINKFLAGS_" + build_flavor)

if msvc12:
    msvc = MSVC_12
    ccflags += ' -DMSVC12'
    cxxflags += ' -DMSVC12'
else:
    msvc = MSVC_14

msvc_version = ''
if msvc != '':
    msvc_version = int(msvc) + 1
    msvc += '.0'

msvc_path = MSVC_PATH_PREFIX + str(msvc_version) + eval("MSVC_PATH_SUFFIX_" + build_arch)

target_arch = eval("TARGET_ARCH_" + build_arch)

qt_dir += '/' + msvc_path
qt_include_path = [
    qt_dir + '/mkspecs/' + QT_MKSPECS_PATH + str(msvc_version),
]
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
]

lib_path += [
    qt_lib_path, 
    '#bin/' + build_flavor + '/' + target_arch + '/sdk',
]

env = Environment(BUILDROOT=build_base_dir, MSVC_VERSION=msvc, TARGET_ARCH=target_arch)
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

# Scons does not have built-in support to create unit test moc files that Qt generates.
# This helper fills that gap by calling the Qt tool moc.exe.
def SetupUtMocs(test_dir, build_flavor):
    os.system(DELETE + ' ' + test_dir + PLATFORM_SLASH + build_flavor)
    os.system('mkdir ' + test_dir + PLATFORM_SLASH + build_flavor)
    test_files = Glob(test_dir + '/*Test*.cpp')
    for test_file in test_files:
        os.system(qt_bin_dir + '/moc -o ' + test_dir + '/' + build_flavor + '/moc_' + test_file.name +' ' + test_dir + '/'+ os.path.splitext(test_file.name)[0]+'.h')

env.Append(CPPPATH = include_path + qt_include_path)
env.Append(CPPPATH = qt_inc_dir, LIBPATH = [qt_bin_dir])
env.Append(CPPPATH = qt_bin_dir)

if platform == 'linux2':
  env.Append(CPPDEFINES = { 'LD_LIBRARY_PATH' : qt_lib_path + '/:$LD_LIBRARY_PATH' })
  linkflags += qt_lib_path

env.Append(CCFLAGS=Split(ccflags))
env.Append(CXXFLAGS=Split(cxxflags))
env.Append(LINKFLAGS=Split(linkflags))

bins = env['BUILDROOT'] + "/" + build_flavor + "/" + target_arch

Export("""
    bins
    build_arch
    build_flavor
    env
    lib_suffix
    lib_path
    qt_bin_dir
    qt_lib_path
    target_arch
    target_name
    platform
    SetupUtMocs
""")

env.SConscript('sdk/SConscript',variant_dir = bins + '/sdk', duplicate=0)
if samples:
  env.SConscript('samples/SConscript',variant_dir = bins + '/samples', duplicate=0)
