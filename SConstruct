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

print qt_lib_path

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
env.Append(CPPPATH = include_search_path)

# env.Append(CPPDEFINES = Split('RMS_SDK_IMPLEMENTATION UNICODE'))
# env.Append(CPPDEFINES = ['UNICODE'])
# env.Append(CPPDEFINES = ['QT_BUILD_CORE_LIB'])

if platform == 'win32':
  # env.Append(CCFLAGS=Split('/EHsc /DELPP_THREAD_SAFE /Zi /Fd${TARGET}.pdb'))
  # env.Append(LINKFLAGS='/DEBUG')
  # env.Append(CPPDEFINES = ['-D' + win_def])
  # env.Append(CPPDEFINES = Split('WIN32 WIN64'))
  env.Append(CPPPATH = qt_inc_dir, LIBPATH = [qt_bin_dir])
  env.Append(CPPPATH = qt_bin_dir) 
  if msvc12:
    env.Append(CPPDEFINES = ['MSVC12'])
# elif  platform == 'darwin':
#   env['STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME']=1
#   env.Append(CPPDEFINES = ['POSIX'])
#   env.Replace(CC='clang')
#   env.Replace(CPP='clang')
#   env.Replace(CXX='clang++')
#   env.Replace(LINK='clang++')
#   env.Append(CXXFLAGS=Split('-mmacosx-version-min=' + osx_version + ' -std=c++11 -stdlib=libc++ -arch '+ target_arch))
#   env.Append(CCFLAGS=Split('-mmacosx-version-min=' + osx_version + ' -DELPP_THREAD_SAFE -arch '+ target_arch))
#   env.Append(LINKFLAGS=Split('-mmacosx-version-min=' + osx_version + ' -arch '+ target_arch))

configuration = ''
if isRelease:
  configuration = 'release'
  env.Append(CPPDEFINES = ['NDEBUG'])
  
  if platform == 'win32':
    env.Append(CCFLAGS='/MD')
    env.Append(LINKFLAGS=Split('/RELEASE /OPT:REF /OPT:ICF /INCREMENTAL:NO'))
    env.Append(CCFLAGS=Split('-c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -GR -EHsc -DUNICODE -DWIN32 -DRMS_CRYPTO_LIBRARY -DQT_NO_DEBUG -DQT_PLUGIN -DQT_CORE_LIB -DNDEBUG'))
    env.Append(CXXFLAGS=Split('-c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -GR -EHsc -DUNICODE -DWIN32 -DRMS_CRYPTO_LIBRARY -DQT_NO_DEBUG -DQT_PLUGIN -DQT_CORE_LIB -DNDEBUG'))
    env.Append(LINKFLAGS='/NOLOGO /DYNAMICBASE /NXCOMPAT /INCREMENTAL:NO /SUBSYSTEM:CONSOLE')
  # elif platform == 'darwin':
  #   env.Append(CCFLAGS=osx_release_ccflags)
else:    
  configuration = 'debug'
  env.Append(CPPDEFINES =['DEBUG','_DEBUG'])
  if platform == 'win32':
    env.Append(CCFLAGS='/MDd')
    # env.Append(CCFLAGS=Split('-nologo -Zc:wchar_t -FS -Zc:strictStrings -Zi -MDd -W3 -w44456 -w44457 -w44458'))
    # env.Append(CXXFLAGS=Split('-c -nologo -W3 -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -Zi -MDd -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -GR -EHsc'))
    env.Append(LINKFLAGS='/NOLOGO')
  # elif platform == 'darwin':
  # env.Append(CXXFLAGS='-DDBG_ENABLED')

bins = env['BUILDROOT'] + "/" + configuration + "/" + target_arch
lib_suffix = ''
if configuration == 'debug':
  lib_suffix = 'd'


Export('arch_suffix bins env lib_suffix qt_lib_path target_name platform')

if samples:
  env.SConscript('samples/SConscript',variant_dir = bins + '/samples', duplicate=0)
else:
  env.SConscript('sdk/SConscript',variant_dir = bins + '/sdk', duplicate=0)
