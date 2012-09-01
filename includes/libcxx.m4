AC_DEFUN([LIBCXX_INIT],[
PKG_PROG_PKG_CONFIG
PKG_CHECK_EXISTS([libcxx],[:],[AC_MSG_ERROR([libcxx not found])])

libx_pkgdatadir="`$PKG_CONFIG --variable=pkgdatadir libcxx`"
CFLAGS="$CFLAGS `$PKG_CONFIG --cflags libcxx`"
CXXFLAGS="$CXXFLAGS `$PKG_CONFIG --cflags libcxx`"
LDFLAGS="$LDFLAGS `$PKG_CONFIG --libs libcxx`"
CC="`$PKG_CONFIG --variable=CC libcxx`"
CXX="`$PKG_CONFIG --variable=CXX libcxx`"
LIBCXX_AM="include $libx_pkgdatadir/libcxx.mk"
AC_SUBST([LIBCXX_AM])

LIBCXX_OPTGEN="$libx_pkgdatadir/optgen.xsl"
AC_SUBST([LIBCXX_OPTGEN])

])
