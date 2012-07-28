#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([libx-example], [0.01], [mrsam@courier-mta.com])
AC_CONFIG_SRCDIR([managedsingleton.C])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([dist-bzip2 foreign -Wno-portability])
LT_INIT

# Checks for programs.
AC_PROG_CXX

# Override compiler with the same one used to be libx.
# This sets CXXFLAGS to enable the correct C++ standard
# mode for libx-based applications.
LIBX_INIT

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_UID_T

# Checks for library functions.

AC_OUTPUT(Makefile)

