#
# Copyright 2015 Double Precision, Inc.
# See COPYING for distribution information.
#

include $(shell pwd)/../includes/libcxx.mk

AM_CPPFLAGS=$(LTDLINC) -I . -I $(srcdir) -I $(srcdir)/../includes -I ../includes
BUILT_SOURCES=x
CLEANFILES=x

LINKTYPE=

x:
	ln -s $(srcdir)/../includes x

lib_LTLIBRARIES=libcxxltdl.la
libcxxltdl_la_DEPENDENCIES=$(LTDLDEPS)
libcxxltdl_la_LIBADD=../base/libcxx.la $(LIBLTDL)
libcxxltdl_la_LDFLAGS=-version-info @VERSION_INFO@ $(LINKTYPE)

libcxxltdl_la_SOURCES=cxxltdl.C
