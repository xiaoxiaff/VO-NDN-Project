# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-
VERSION = "0.1.0"
APPNAME = "ndnabac"
GIT_TAG_PREFIX = "ndnabac"

from waflib import Logs, Utils, Context
import os

def options(opt):
    opt.load(['compiler_cxx', 'gnu_dirs'])
    opt.load(['boost', 'default-compiler-flags', 'sqlite3', 'glib2',
              'coverage', 'sanitizers',
              'doxygen', 'sphinx_build'], tooldir=['.waf-tools'])

    syncopt = opt.add_option_group ("ndnabac options")
    syncopt.add_option('--with-tests', action='store_true', default=False, dest='with_tests',
                       help='''build unit tests''')

def configure(conf):
    conf.load(['compiler_cxx', 'gnu_dirs',
               'boost', 'default-compiler-flags', 'sqlite3', 'glib2',
               'doxygen', 'sphinx_build'])

    if 'PKG_CONFIG_PATH' not in os.environ:
       os.environ['PKG_CONFIG_PATH'] = Utils.subst_vars('${LIBDIR}/pkgconfig', conf.env)

    conf.check_cfg(package='libndn-cxx', args=['--cflags', '--libs'],
                   uselib_store='NDN_CXX', mandatory=True)

    conf.check_cfg(package='libndn-abac', args=['--cflags', '--libs'],
                   uselib_store='NDN_ABAC', mandatory=True)

    USED_BOOST_LIBS = ['system', 'filesystem', 'iostreams',
                       'program_options', 'thread', 'log', 'log_setup']

    conf.env['WITH_TESTS'] = conf.options.with_tests
    if conf.env['WITH_TESTS']:
        USED_BOOST_LIBS += ['unit_test_framework']
        conf.define('HAVE_TESTS', 1)

    conf.check_boost(lib=USED_BOOST_LIBS, mt=True)
    if conf.env.BOOST_VERSION_NUMBER < 105400:
        Logs.error("Minimum required boost version is 1.54.0")
        Logs.error("Please upgrade your distribution or install custom boost libraries" +
                    " (https://redmine.named-data.net/projects/nfd/wiki/Boost_FAQ)")
        return

    # Loading "late" to prevent tests to be compiled with profiling flags
    conf.load('coverage')

    conf.load('sanitizers')

    conf.define('SYSCONFDIR', conf.env['SYSCONFDIR'])

    # If there happens to be a static library, waf will put the corresponding -L flags
    # before dynamic library flags.  This can result in compilation failure when the
    # system has a different version of the ndnabac library installed.
    conf.env['STLIBPATH'] = ['.'] + conf.env['STLIBPATH']

    conf.check_cfg (package='glib-2.0', uselib_store='GLIB', atleast_version='2.25.0',
	                args='--cflags --libs')

    conf.env.LIBPATH_PBC = ['/usr/local/Cellar/pbc/0.5.14/lib']
    conf.env.INCLUDES_PBC  = ['/usr/local/Cellar/pbc/0.5.14/include/pbc']
    conf.check_cxx(lib = 'pbc', use = 'PBC', args='--cflags --libs')

    conf.env.LIBPATH_GMP = ['/usr/local/lib']
    conf.env.INCLUDES_GMP  = ['/usr/local/include']
    conf.check_cxx(lib = 'gmp', use = 'GMP', args='--cflags --libs')

    conf.env.STLIBPATH_BSWABE = ['/usr/local/lib']
    conf.env.INCLUDES_BSWABE  = ['/usr/local/include']
    conf.check_cxx(lib = 'bswabe', use = 'BSWABE')

    conf.write_config_header('src/ndnabac-config.hpp')

def build(bld):
    core = bld(
        target='core-objects',
        name='core-objects',
        features='cxx',
        source=bld.path.ant_glob(['daemon/*.cpp']),
        use='version NDN_CXX NDN_ABAC BOOST LIBRT PBC GLIB',
        includes='. core',
        export_includes='.',
        headers='daemon/ndnabacdaemon-common.hpp')

    attribute_authority = bld(
        target='bin/attribute_authority',
        name='attribute_authority',
        features='cxx cxxprogram',
        source=bld.path.ant_glob(['daemon/AttributeAuthority/main.cpp']),
        use='core-objects',
        includes='daemon')

    consumer = bld(
        target='bin/consumer',
        name='consumer',
        features='cxx cxxprogram',
        source=bld.path.ant_glob(['daemon/Consumer/main.cpp']),
        use='core-objects',
        includes='daemon')

    token_issuer = bld(
        target='bin/token_issuer',
        name='token_issuer',
        features='cxx cxxprogram',
        source=bld.path.ant_glob(['daemon/TokenIssuer/main.cpp']),
        use='core-objects',
        includes='daemon')

    data_owner = bld(
        target='bin/data_owner',
        name='data_owner',
        features='cxx cxxprogram',
        source=bld.path.ant_glob(['daemon/DataOwner/main.cpp']),
        use='core-objects',
        includes='daemon')

    producer = bld(
        target='bin/producer',
        name='producer',
        features='cxx cxxprogram',
        source=bld.path.ant_glob(['daemon/Producer/main.cpp']),
        use='core-objects',
        includes='daemon')
    bld.recurse('tests')
