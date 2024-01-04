#
# Copyright 2018 Double Precision, Inc.
# See COPYING for distribution information.
#

include $(shell pwd)/../includes/x/libcxx.mk

AM_CPPFLAGS=$(CUPS_CFLAGS) -I . -I $(srcdir) -I $(srcdir)/../includes -I ../includes

abilibdir=$(libdir)/libcxx-@ABIVER@
abilib_LTLIBRARIES=libcxxcups.la
libcxxcups_la_LIBADD=../base/libcxx.la $(CUPS_LIBS) -lcourier-unicode
libcxxcups_la_LDFLAGS=-version-info @VERSION_INFO@ $(CUPS_LDFLAGS)

$(call THREADMSGDISPATCHER_GEN,cupsthread.inc.H,cupsthread.xml)


libcxxcups_la_SOURCES=\
	available.C					\
	available_dests.C				\
	available_dests.H				\
	available_impl.C				\
	available_impl.H				\
	cupsthread.C					\
	cupsthread.H					\
	destination.C					\
	destination_impl.C				\
	destination_impl.H				\
	job.C						\
	job_impl.C					\
	job_impl.H					\
\
	#

# automake generates a direct all: all-am dependency if BUILT_SOURCES is not
# set. We need it to generate a dependency on BUILT_SOURCES, and a rule
# to manually make all-am, in order to get the dependencies right.

BUILT_SOURCES=testcups.h

$(call OPTIONS_GEN,testcups.h,$(srcdir)/testcups.xml)

noinst_PROGRAMS=testcups testserialize
testcups_SOURCES=testcups.C
testcups_LDADD=libcxxcups.la ../base/libcxx.la
testcups_LDFLAGS=$(TESTLINKTYPE)

testserialize_SOURCES=testserialize.C
testserialize_LDADD=libcxxcups.la ../base/libcxx.la
testserialize_LDFLAGS=-static

check-am: $(noinst_PROGRAMS)
	./testserialize
