#
# Copyright 2018 Double Precision, Inc.
# See COPYING for distribution information.
#

include $(shell pwd)/../includes/x/libcxx.mk

AM_CPPFLAGS=$(CUPS_CFLAGS) -I . -I $(srcdir) -I $(srcdir)/../includes -I ../includes

lib_LTLIBRARIES=libcxxcups.la
libcxxcups_la_LIBADD=../base/libcxx.la $(CUPS_LIBS)
libcxxcups_la_LDFLAGS=-version-info @VERSION_INFO@ $(CUPS_LDFLAGS)

libcxxcups_la_SOURCES=\
	available.C					\
	available_dests.C				\
	available_dests.H				\
	available_impl.C				\
	available_impl.H				\
	destination.C					\
	destination_impl.C				\
	destination_impl.H				\
	job.C						\
	job_impl.C					\
	job_impl.H					\
\
	#

$(call OPTIONS_GEN,testcups.h,$(srcdir)/testcups.xml)

noinst_PROGRAMS=testcups
testcups_SOURCES=testcups.C
testcups_LDADD=libcxxcups.la
testcups_LDFLAGS=$(TESTLINKTYPE)
