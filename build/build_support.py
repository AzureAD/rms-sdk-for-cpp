#!python
import sys
import os

def ParseConfig(env,command,options):
    env_dict = env.Dictionary();
    static_libs = []

    # setup all the dictionary options
    if not env_dict.has_key('CPPPATH'):
        env_dict['CPPPATH'] = []

    if not env_dict.has_key('LIBPATH'):
        env_dict['LIBPATH'] = []

    if not env_dict.has_key('LIBS'):
        env_dict['LIBS'] = []

    if not env_dict.has_key('CXXFLAGS'):
        env_dict['CXXFLAGS'] = []

    if not env_dict.has_key('LINKFLAGS'):
        env_dict['LINKFLAGS'] = []

    # run the config program to get the options we need
    full_cmd = "%s %s" %  (WhereIs(command), join(options))

    params = split(os.popen(full_cmd).read())

    i = 0
    while i < len(params):
        arg = params[i]
        switch = arg[0:1]
        opt = arg[1:2]
        if switch == '-':
            if opt == 'L':
                env_dict['LIBPATH'].append(arg[2:])
            elif opt == 'l':
                env_dict['LIBS'].append(arg[2:])
            elif opt == 'I':
                env_dict['CPPPATH'].append(arg[2:])
            elif arg[1:] == 'framework':
                flags = [env_dict['LINKFLAGS']]
                flags.append(join(params[i:i+2]))
                env_dict['LINKFLAGS'] = join(flags)
                i = i + 1
            else:
                env_dict['CXXFLAGS'].append(arg)
        else:
            static_libs.append(arg)
        i = i + 1

    return static_libs