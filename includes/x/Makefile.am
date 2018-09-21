#
# Copyright 2012 Double Precision, Inc.
# See COPYING for distribution information.
#

SUBDIRS = ftp http gcrypt gnutls mime threads yaml xml cups sys param

headerdir=$(includedir)/libcxx-@ABIVER@/x

header_HEADERS = $(shell cd $(srcdir) && ls *.H *.h 2>/dev/null)

pkgdataverdir=$(datadir)/libcxx-@ABIVER@
pkgdataver_DATA=optgen.xsl optgen.sample.xml msgdispatcher.xsl threadmsgdispatcher.xsl refcollection.xsl libcxx.mk

aclocaldir=@datadir@/aclocal
aclocal_DATA=libcxx-@VERSION@.m4

libcxx-@VERSION@.m4: libcxx.m4
	rm -f libcxx-@VERSION@.m4
	ln libcxx.m4 libcxx-@VERSION@.m4

BUILT_SOURCES=namespace.h epoll_def.h globlocale.H signal_def.h xlocale.H
DISTCLEANFILES=namespace.h optgen.xsl msgdispatcher.xsl threadmsgdispatcher.xsl refcollection.xsl libcxx.mk
EXTRA_DIST=optgen.xsl optgen.sample.xml

dist-hook:
	rm -f $(patsubst %,$(distdir)/%,$(BUILT_SOURCES))

EXTRA_HEADERS=
