#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(fu, 1.2.0, https://github.com/klesh/fu/issues)
AC_PROG_RANLIB
AC_CONFIG_SRCDIR([fu/src/main.cpp])
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC()

# Checks for libraries.

# -------------------------------------------------------------------------------
# wxwidget 3.0.2.
# -------------------------------------------------------------------------------
reqwx=3.0.2
AM_PATH_WXCONFIG($reqwx, wxWin=1)
if test "$wxWin" != 1; then
	AC_MSG_ERROR([
		wxWidgets must be installed on your system.

		Please check that wx-config is in path, the directory
		where wxWidgets libraries are installed (returned by
		'wx-config --libs' or 'wx-config --static --libs' command)
		is in LD_LIBRARY_PATH or equivalent variable and
		wxWidgets version is $reqwx or above.
		])
fi


# -------------------------------------------------------------------------------
# Application indicator.
# -------------------------------------------------------------------------------

APPINDICATOR_REQUIRED_VERSION=0.2.4

AC_ARG_ENABLE([appindicator],
	AS_HELP_STRING([--enable-appindicator[=@<:@no/auto/yes@:>@]],[Build support for application indicators]),
	[enable_appindicator=$enableval],
	[enable_appindicator="auto"])


if test x$enable_appindicator = xauto ; then
	PKG_CHECK_EXISTS(appindicator-0.1 >= $APPINDICATOR_REQUIRED_VERSION,
		[enable_appindicator="yes"],
		[enable_appindicator="no"])
fi

if test x$enable_appindicator = xyes ; then
	PKG_CHECK_MODULES(APPINDICATOR,
		[appindicator-0.1 >= $APPINDICATOR_REQUIRED_VERSION],
		[AC_DEFINE(HAVE_APPINDICATOR, 1, [Have AppIndicator])])
fi

AM_CONDITIONAL(HAVE_APPINDICATOR, test x$enable_appindicator = xyes)
AC_SUBST(APPINDICATOR_CFLAGS)
AC_SUBST(APPINDICATOR_LIBS)

AC_SUBST(X11_LIBS, -lX11)

# -------------------------------------------------------------------------------
# libcurl libssh2
# -------------------------------------------------------------------------------
PKG_CHECK_MODULES([curl], [libcurl])
PKG_CHECK_MODULES([libssh2], [libssh2])
AM_PATH_GTK_2_0(2.10.0, [], AC_MSG_ERROR(Requirement not met: gtk >= 2.10.0))

# Checks for header files.

# -------------------------------------------------------------------------------
# libcurl libssh2 boost
# -------------------------------------------------------------------------------
AC_LANG_PUSH([C++])
AC_CHECK_HEADER([boost/uuid/uuid.hpp], [], AC_MSG_ERROR([boost is required]))
AC_LANG_POP([C++])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.


CPPFLAGS="-std=gnu++14 $CPPFLAGS $WX_CPPFLAGS $curl_CFLAGS $libssh2_CFLAGS $GTK_CFLAGS $APPINDICATOR_CFLAGS"
CXXFLAGS="-std=gnu++14 $CXXFLAGS $WX_CXXFLAGS_ONLY $curl_CFLAGS $libssh2_CFLAGS $GTK_CFLAGS $APPINDICATOR_CFLAGS"
CFLAGS="$CFLAGS $WX_CFLAGS_ONLY $curl_CFLAGS $libssh2_CFLAGS $GTK_CFLAGS $APPINDICATOR_CFLAGS"
LIBS="$LIBS $WX_LIBS $curl_LIBS $libssh2_LIBS $APPINDICATOR_LIBS $GTK_LIBS"
AC_OUTPUT([Makefile \
           fu/Makefile \
           fu/src/Makefile \
           fu/src/core/Makefile \
           fu/src/os/Makefile \
           fu/src/protocols/Makefile \
           fu/src/protocols/imgur/Makefile \
           fu/src/protocols/local/Makefile \
           fu/src/protocols/sftp/Makefile \
           fu/src/protocols/smms/Makefile \
           fu/src/ui/Makefile \
           fu/resources/Makefile
])
