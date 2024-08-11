#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([libcxx-examples],[0.0.0],[libcxx-users@lists.libcxx.org])
AC_CONFIG_SRCDIR([managedsingleton.C])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([dist-bzip2 foreign -Wno-portability])
AM_SILENT_RULES([yes])

# Override compiler with the same one used to be libx.
# This sets CXXFLAGS to enable the correct C++ standard
# mode for libx-based applications.
LIBCXX_INIT

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_UID_T

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
