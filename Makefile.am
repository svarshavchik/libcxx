#
# Copyright 2012-2021 Double Precision, Inc.
# See COPYING for distribution information.
#

AUTOMAKE_OPTIONS=dist-bzip2 -Wno-portability
SUBDIRS=includes/x libltdl base cups yaml gnutls httportmap cxxltdl packaging/fedora po

define reverse
$(if $(1), $(call reverse,$(wordlist 2,$(words $(1)),$(1))) $(firstword $(1)))
endef

BUILT_SOURCES=abi.xml

$(shell :)DIST_SUBDIRS=$(call reverse,$(SUBDIRS) benchmarks)

DISTCHECK_CONFIGURE_FLAGS=--with-pkgconfigdir='$$(prefix)/local/pkgconf'
man_MANS = properties.1 maillogs.1 httportmapd.8
EXTRA_DIST=config.rpath DoxygenLayout.xml abi.xml.in $(man_MANS) \
	$(patsubst $(srcdir)/%,%, \
           $(wildcard $(srcdir)/examples/*.C) \
	   $(wildcard $(srcdir)/examples/*/configure.ac) \
	   $(wildcard $(srcdir)/examples/*/Makefile.am) \
	   $(wildcard $(srcdir)/examples/*/*.C) \
	   $(srcdir)/packaging/freebsd10/devel/libcxx/Makefile.in \
	   $(srcdir)/packaging/freebsd10/devel/libcxx/pkg-deinstall \
	   $(srcdir)/packaging/freebsd10/devel/libcxx/pkg-descr \
	   $(srcdir)/packaging/freebsd10/devel/libcxx/pkg-install \
	   $(srcdir)/packaging/freebsd10/devel/libcxx/pkg-message \
	   $(wildcard $(srcdir)/packaging/freebsd10/devel/libcxx/files/*.in) \
	   $(wildcard $(srcdir)/packaging/freebsd10/devel/libcxx/files/patch*C) \
        )

DOCS_SRC = $(shell find includes/x -name '*.H' -print)

docs: html/index.html html/book.css html/logo.png html/robots.txt html/.htaccess html/frame.js html/icon.gif $(patsubst %,html/%,$(notdir $(wildcard $(srcdir)/docbook/*.png)))
	chcon -R -t httpd_sys_content_t html 2>/dev/null || true
	cd html; for d in `ls */. | sed 's@/.$$@@'`; do rm -rf ~/www/hostrocket/libcxx.org/$$d; done
	cd ~/www/hostrocket/libcxx.org || exit 1; rm -f * 2>/dev/null; exit 0
	rsync -a -X html/* ~/www/hostrocket/libcxx.org

html/%.png: html/index.html docbook/%.png
	$(INSTALL) -m 0644 $(srcdir)/docbook/$(notdir $@) html

html/book.css: html/index.html docbook/book.css
	$(INSTALL) -m 0644 $(srcdir)/docbook/book.css html/book.css

html/robots.txt: html/index.html docbook/robots.txt
	$(INSTALL) -m 0644 $(srcdir)/docbook/robots.txt html/robots.txt

html/.htaccess: html/index.html docbook/.htaccess
	$(INSTALL) -m 0644 $(srcdir)/docbook/.htaccess html/.htaccess

html/frame.js: html/index.html docbook/frame.js
	$(INSTALL) -m 0644 $(srcdir)/docbook/frame.js html/frame.js

html/icon.gif: html/index.html docbook/icon.gif
	$(INSTALL) -m 0644 $(srcdir)/docbook/icon.gif html/icon.gif

html/index.html: html/ref/index.html $(wildcard docbook/*.xml) docbook/xmllinks.xsl
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
		$(srcdir)/docbook/docbook.xsl $(srcdir)/docbook/book.xml
	git ls-files examples | cpio -o --quiet | (cd html.tmp; cpio -i --quiet --make-directories)
	find html.tmp -name .gitignore -print | xargs rm -f
	echo 'ForceType text/plain' >html.tmp/examples/.htaccess
	echo 'Options +Indexes' >>html.tmp/examples/.htaccess
	links -dump -no-numbering -no-references html.tmp/INSTALL.html >INSTALL.tmp
	mv -f INSTALL.tmp INSTALL
	mkdir html.tmp/ref
	ln html/ref/* html.tmp/ref
	rm -rf html
	mv html.tmp html

html/ref/index.html: $(DOCS_SRC) Doxyfile header.html
	rm -rf html/ref
	mkdir -p html/ref
	doxygen; c=$$?;	perl -p -i -e 's/INSERT_LIBX_NAMESPACE/x/g' html/ref/*.html; exit $$c

header.html: Doxyfile
	rm -rf header.html.tmp
	mkdir -p header.html.tmp
	cd header.html.tmp && doxygen -w html header.html footer.html doxygen.css
	perl -p -i -e 's@<head>@<head><link rel="icon" href="/icon.gif" type="image/gif"/>@;s@\$$projectbrief@<a href="/">\$$projectbrief</a>@;' header.html.tmp/header.html
	mv -f header.html.tmp/header.html .
	rm -rf header.html.tmp

.PHONY: rpm

dist-hook:
	rm -f $(distdir)/includes/x/namespace.h
	rm -f $(distdir)/includes/x/msgdispatcher.xsl
	rm -f $(distdir)/includes/x/threadmsgdispatcher.xsl

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

pkgconf_DATA=libcxx-@ABIVER@.pc

libcxx-@ABIVER@.pc: Makefile
	echo "prefix=${prefix}" >libcxx.pc.tmp
	echo "exec_prefix=${exec_prefix}" >>libcxx.pc.tmp
	echo "datadir=${datadir}" >>libcxx.pc.tmp
	echo "pkgdatadir=${pkgdatadir}-@ABIVER@" >>libcxx.pc.tmp
	echo "libdir=${libdir}/libcxx-@ABIVER@" >>libcxx.pc.tmp
	echo "includedir=${includedir}/libcxx-@ABIVER@" >>libcxx.pc.tmp
	echo "CC=$(CC)" >>libcxx.pc.tmp
	echo "CXX=$(CXX)" >>libcxx.pc.tmp
	echo "" >>libcxx.pc.tmp
	echo "Name: libcxx-@ABIVER@" >>libcxx.pc.tmp
	echo "Description: libcxx library" >>libcxx.pc.tmp
	echo "Version: @VERSION@" >>libcxx.pc.tmp
	echo "Requires.private: gnutls" >>libcxx.pc.tmp
	echo "Cflags: -std=c++20 -fno-omit-frame-pointer -pthread -I$(includedir)/libcxx-@ABIVER@" >>libcxx.pc.tmp
	echo "Libs: -L${libdir}/libcxx-@ABIVER@ -Wl,-rpath -Wl,${libdir}/libcxx-@ABIVER@" >>libcxx.pc.tmp
	mv -f libcxx.pc.tmp libcxx-@ABIVER@.pc

CLEANFILES=$(pkgconf_DATA) abi.xml header.html $(man_MANS)

$(man_MANS): man.stamp
	touch $@

man.stamp: $(wildcard docbook/man*.xml)
	if test ! -f docbook/book.xml; then touch man.stamp; exit 0; fi; \
	test -f docbook/entities || $(MAKE) docs; \
	set -e; rm -rf manpages; \
	xsltproc --nonet -o manpages/ http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl docbook/book.xml; \
	mv -f manpages/* . ; touch man.stamp
	rm -rf manpages

abi.xml: abi.xml.in config.status
	sed 's~@INCLUDEDIR@~$(includedir)~;s~@LIBDIR@~$(libdir)~' <$(srcdir)/abi.xml.in >abi.xml.tmp; mv -f abi.xml.tmp abi.xml

CLEANFILES += man.stamp

ACLOCAL_AMFLAGS = -I m4

upload:
	GPG_TTY=`tty` git tag -s -a @PACKAGE@/@VERSION@/`date +%Y%m%d%H%M%S` -m 'Tag version @VERSION@'
	rm -f @PACKAGE@-@VERSION@.tar.bz2.sig; gpg --detach-sign --default-key mrsam@courier-mta.com @PACKAGE@-@VERSION@.tar.bz2; test -f @PACKAGE@-@VERSION@.tar.bz2.sig || exit 1
	pag upload @PACKAGE@-@VERSION@.tar.bz2
	pag upload @PACKAGE@-@VERSION@.tar.bz2.sig
