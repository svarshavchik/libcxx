#
# Copyright 2012 Double Precision, Inc.
# See COPYING for distribution information.
#

AUTOMAKE_OPTIONS=foreign dist-bzip2 -Wno-portability
SUBDIRS= po includes base gnutls httportmap packaging/fedora
DIST_SUBDIRS= $(SUBDIRS) benchmarks
DISTCHECK_CONFIGURE_FLAGS=--with-pkgconfigdir='$$(prefix)/local/pkgconf'
man_MANS = properties.1 maillogs.1 httportmapd.8
EXTRA_DIST=config.rpath DoxygenLayout.xml $(man_MANS) \
	$(wildcard examples/*.C) \
	examples/managedsingleton/configure.ac \
	examples/managedsingleton/Makefile.am \
	examples/managedsingleton/managedsingleton.C

DOCS_SRC = $(shell find includes -name '*.H' -print)

docs: html/index.html html/book.css

html/index.html: html/ref/index.html $(wildcard docbook/*.xml docbook/*.png) docbook/xmllinks.xsl
	mkdir -p docbook
	xsltproc $(srcdir)/docbook/xmllinks.xsl xml/index.xml >docbook/entities
	rm -rf html.tmp
	mkdir -p html.tmp
	xsltproc --nonet -o html.tmp/ --xinclude \
		--stringparam use.id.as.filename 1 \
		--stringparam html.stylesheet book.css \
		--stringparam root.filename 'toc' \
		--stringparam generate.id.attributes 1 \
		--param local.l10n.xml 'document("'`pwd`/docbook/l10.xml'")' \
		http://docbook.sourceforge.net/release/xsl/current/xhtml-1_1/chunk.xsl\
		 $(srcdir)/docbook/book.xml
	rm -f html/*.html
	$(INSTALL) -m 0644 $(srcdir)/docbook/*.png html.tmp
	mv html.tmp/* html
	rmdir html.tmp
	rm -rf html/examples
	find examples -depth \( -type d -o -name '*.C' -o -name Makefile.am -o -name configure.ac \) -print | cpio -o --quiet | (cd html; cpio -i --quiet --make-directories)
	echo 'ForceType text/plain' >html/examples/.htaccess

html/book.css: html/index.html docbook/book.css
	$(INSTALL) -m 0644 $(srcdir)/docbook/book.css html/book.css

html/ref/index.html: $(DOCS_SRC) Doxyfile
	rm -rf html
	mkdir html
	doxygen; c=$$?;	perl -p -i -e 's/INSERT_LIBX_NAMESPACE/x/g' html/ref/*.html; exit $$c

.PHONY: rpm

dist-hook:
	rm -f $(distdir)/includes/namespace.h
	rm -f $(distdir)/includes/msgdispatcher.xsl


rpm:
	make dist
	rm -rf rpm/SRPMS/*
	rm -rf rpm/RPMS/*
	rm -rf rpm/BUILD/*
	rm -rf rpm/BUILDROOT/*
	rpmbuild -ta --clean \
		--define "_topdir `pwd`/rpm" \
		--define '_rpmdir %{_topdir}/RPMS' \
		--define '_srcrpmdir %{_topdir}/SRPMS' \
		--define '_sourcedir %{_topdir}/SOURCES' \
		--define '_specdir %{_topdir}/SPECS' \
		--define '_builddir %{_topdir}/BUILD' \
		--define '_build_name_fmt %%{ARCH}/%%{NAME}-%%{VERSION}-%%{RELEASE}.%%{ARCH}.rpm' \
		--define '_tmppath %{_var}/tmp' \
		--define '__spec_prep_pre %{___build_pre}' \
		--define "extrarelease .`date '+%Y%m%d%H%M%S'`" @PACKAGE@-@VERSION@.tar.bz2
	sh $$HOME/repos.sh

pkgconfdir=@PKGCONFIGDIR@

pkgconf_DATA=libcxx.pc

libcxx.pc: Makefile
	echo "prefix=${prefix}" >libcxx.pc.tmp
	echo "exec_prefix=${exec_prefix}" >>libcxx.pc.tmp
	echo "datadir=${datadir}" >>libcxx.pc.tmp
	echo "pkgdatadir=${pkgdatadir}" >>libcxx.pc.tmp
	echo "libdir=${libdir}" >>libcxx.pc.tmp
	echo "includedir=${includedir}" >>libcxx.pc.tmp
	echo "CC=$(CC)" >>libcxx.pc.tmp
	echo "CXX=$(CXX)" >>libcxx.pc.tmp
	echo "" >>libcxx.pc.tmp
	echo "Name: libcxx" >>libcxx.pc.tmp
	echo "Description: libcxx library" >>libcxx.pc.tmp
	echo "Version: @VERSION@" >>libcxx.pc.tmp
	echo "Requires: gnutls" >>libcxx.pc.tmp
	echo "Cflags: -fno-omit-frame-pointer -std=c++0x" >>libcxx.pc.tmp
	mv -f libcxx.pc.tmp libcxx.pc

CLEANFILES=libcxx.pc $(man_MANS)

$(man_MANS): man.stamp
	touch $@

man.stamp: $(wildcard docbook/man*.xml)
	if test ! -f docbook/book.xml; then touch man.stamp; exit 0; fi; \
	set -e; rm -rf manpages; \
	xsltproc --nonet -o manpages/ http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl docbook/book.xml; \
	mv -f manpages/* . ; touch man.stamp
	rm -rf manpages

CLEANFILES += man.stamp

ACLOCAL_AMFLAGS = -I m4
