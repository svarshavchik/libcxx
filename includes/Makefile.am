#
# Copyright 2012 Double Precision, Inc.
# See COPYING for distribution information.
#

SUBDIRS = http gcrypt gnutls mime threads yaml xml sys param

headerdir=$(includedir)/x

header_HEADERS = $(shell cd $(srcdir) && ls *.H *.h 2>/dev/null)

pkgdata_DATA=optgen.xsl optgen.sample.xml msgdispatcher.xsl libcxx.mk 

aclocaldir=@datadir@/aclocal
aclocal_DATA=libcxx.m4

BUILT_SOURCES=namespace.h epoll_def.h globlocale.H signal_def.h xlocale.H
DISTCLEANFILES=namespace.h optgen.xsl msgdispatcher.xsl libcxx.mk
EXTRA_DIST=optgen.xsl optgen.sample.xml

dist-hook:
	rm -f $(patsubst %,$(distdir)/%,$(BUILT_SOURCES))

EXTRA_HEADERS=
