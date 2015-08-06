#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_INIT([libARController], [3.1.0], [ardrone@ardrone.org])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(m4)

m4_include([../../ARSDKBuildUtils/Utils/m4/ARSDK.m4])

# automake initialisation and check for minimal automake API version 1.9
AM_INIT_AUTOMAKE

# use the C compiler for the following checks
AC_LANG([C])

##########################################################################
# Debug compilation support
##########################################################################
AC_MSG_CHECKING([whether to build with debug information])
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],
    [enable debug data generation (def=no)])],
    [debugit="$enableval"],
    [debugit=no])
AC_MSG_RESULT([$debugit])

if test x"$debugit" = x"yes"; then
    AC_DEFINE([DEBUG],[],[Debug Mode])
    LOCAL_CFLAGS+=" -g -Wall -Werror -Wno-uninitialized -O0"
else
    AC_DEFINE([NDEBUG],[],[No-debug Mode])
    LOCAL_CFLAGS+=" -O3"
fi
AM_CONDITIONAL([DEBUG_MODE], [test "$debugit" = "yes"])

##########################################################################
# Project selection
##########################################################################
AC_ARG_WITH([projects],
            [AS_HELP_STRING([--with-projects=PROJECTS_NAMES],
                            [generate commands for the specified projects (project1,project2,...). `all` means use every available xml file @<:@default=all@:>@])],
            [],
            [with_projects=all])

AC_MSG_CHECKING([whether we need to generate debug commands])
AC_ARG_ENABLE([debug-commands],
            [AS_HELP_STRING([--enable-debug-commands],
                            [enable generation of debug commands @<:@default=disabled@:>@])],
            [dbgcmds="$enableval"],
            [dbgcmds=no])
AC_MSG_RESULT([$dbgcmds])

##########################################################################
# Non versionned .so compilation support (for Android)
##########################################################################
AC_MSG_CHECKING([whether to put version number in .so files])
AC_ARG_ENABLE([so-version],
    [AS_HELP_STRING([--disable-so-version],
    [disable version names in .so files [default=enabled]])],
    [sover="$enableval"],
    [sover=yes])
AC_MSG_RESULT([$sover])

##########################################################################
# Define includes directories
##########################################################################

# include local headers first
LOCAL_CFLAGS+=" -I$srcdir/../Includes -I$srcdir/../Sources -I$srcdir/../gen/Includes"

CPPFLAGS+="$LOCAL_CFLAGS"
CFLAGS+="$LOCAL_CFLAGS"
OBJCFLAGS+="$LOCAL_CFLAGS"

# Add libARxxx deps

AR_DEPENDS([libARSAL], [Y], [libARSAL/ARSAL.h])
AR_DEPENDS([libARNetworkAL], [Y], [libARNetworkAL/ARNetworkAL.h])
AR_DEPENDS([libARNetwork], [Y], [libARNetwork/ARNetwork.h])
AR_DEPENDS([libARDiscovery], [Y], [libARDiscovery/ARDiscovery.h])
AR_DEPENDS([libARCommands], [Y], [libARCommands/ARCommands.h])
AR_DEPENDS([libARStream], [Y], [libARStream/ARStream.h])
AR_DEPENDS([uthash], [Y], [uthash/uthash.h], [N], [Y])
AR_DEPENDS([json], [N], [json/json.h], [N])

##########################################################################
# Check headers
##########################################################################

# Check for PRIxx macros
AC_CHECK_DECLS([PRIu8, PRIi8, PRIu16, PRIi16, PRIu32, PRIi32, PRIu64, PRIi64],[],[AC_MSG_ERROR(PRIxx macros not found in inttypes.h)],[[#include <inttypes.h>]])

##########################################################################
# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

AC_CHECK_PROGS([DOXYGEN], [doxygen])
m4_include(doxygen.ac)
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN([libARController], [doxygen.cfg], [$srcdir/../Doc])
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])

# Check for python and run prebuild PY script
AM_PATH_PYTHON(,, AC_MSG_ERROR(Python is required to generate source files for this library))




# Check presence of the input XML file.
AC_MSG_CHECKING([presence of XML commands file])
line_proj=$(echo $with_projects | tr ',' '\n')
for project in $line_proj; do
    if ! test xall = x$project; then
        xmlfile="$srcdir/../../libARCommands/Xml/""$project""_commands.xml"
        if test -f "$xmlfile"; then
            AC_MSG_RESULT(["$xmlfile"])
        else
            AC_MSG_ERROR([Could not find commands XML file "$xmlfile".
                          Please check the value of the --with-projects option or create the file yourself.])
        fi
    else
        AC_MSG_RESULT([Use all available files])
    fi
done

# Generate source files from XML
AC_MSG_NOTICE([generating source files from XML description of commands])
#AM_CONDITIONAL([PYTHON_RUN_OK], [$PYTHON $srcdir/../Python/generateFeatureControllers.py -debug-cmds "$dbgcmds" -feature "$with_projects"])
AM_CONDITIONAL([PYTHON_RUN_OK], [$PYTHON $srcdir/../Python/generateControllersSources.py -feature "$with_projects"])
AM_COND_IF([PYTHON_RUN_OK], , AC_MSG_ERROR(Python processing failed --> XML files probably contains errors))




# Run prebuild PY script
AC_MSG_NOTICE([Executing python prebuild commands])
AM_CONDITIONAL([PYTHON_RUN_OK], [$PYTHON $srcdir/../../ARSDKBuildUtils/Utils/Python/ARSDK_PrebuildActions.py $srcdir])
AM_COND_IF([PYTHON_RUN_OK], , AC_MSG_ERROR(Python processing failed))

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Generate libtool version info number
if test x"$sover" = x"yes"; then
    LDFLAGS+=" -release $PACKAGE_VERSION"
else
    LDFLAGS+=" -avoid-version"
fi

# distribute additional compiler and linker flags
# --> set these variables instead of CFLAGS or LDFLAGS
AC_SUBST([CFLAGS])
AC_SUBST([LDFLAGS])

# Checks for library functions.


# Generates Makefile
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo "
  $PACKAGE_NAME version $PACKAGE_VERSION
  Prefix.........: $prefix
  Debug Build....: $debugit
  C Compiler.....: $CC $CFLAGS
  Linker.........: $LD $LDFLAGS $LIBS
  Doxygen........: ${DOXYGEN:-NONE}
"
