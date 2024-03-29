#
# Copyright 2012 Double Precision, Inc.
# See COPYING for distribution information.
#

include $(shell pwd)/../includes/x/libcxx.mk

AM_CPPFLAGS=-I . -I $(srcdir) -I $(srcdir)/../includes -I ../includes

AM_CXXFLAGS=@GNUTLS_CFLAGS@ @GCRYPT_CFLAGS@

headerdir=$(includedir)/x/gnutls

BUILT_SOURCES=gettext_in.h tlsparamsgen.h tlsparamsdir.h gnutls_defs.h \
	gcrypt_md.h
CLEANFILES=gettext_in.h tlsparamsgen.h tlsparamsdir.h gnutls_defs.h \
	gcrypt_md.h

LINKTYPE=

gettext_in.h:
	ln -s $(srcdir)/../base/gettext_in.h gettext_in.h

tlsparamsdir.h:
	echo '#define TLSPARAMSDIR "@localstatedir@/local/tlsparams"' >tlsparamsdir.h.tmp
	mv -f tlsparamsdir.h.tmp tlsparamsdir.h


gnutls_defs.h: Makefile
	echo '#include <gnutls/gnutls.h>' >gnutls_defs.tmp.C
	echo '#define GNUTLS_SEC_ENUM ' `$(CPP) $(AM_CPPFLAGS) $(CPPFLAGS) gnutls_defs.tmp.C | fgrep GNUTLS_SEC_PARAM | sed 's/=[^,]*//'` >gnutls_defs.h.tmp
	rm -f gnutls_defs.tmp.C
	mv -f gnutls_defs.h.tmp gnutls_defs.h

gcrypt_md.h: Makefile
	echo '#include <gcrypt.h>' >gcrypt_md.tmp.C
	echo '#define GNUTLS_MD_ENUM ' `$(CPP) $(AM_CPPFLAGS) $(CPPFLAGS) gcrypt_md.tmp.C | sed '1,/enum gcry_md_algos/d;/}/,$$d' | fgrep GCRY_MD | sed 's/=[^,]*//'` >gcrypt_md.h.tmp
	rm -f gcrypt_md.tmp.C
	mv -f gcrypt_md.h.tmp gcrypt_md.h

abilibdir=$(libdir)/libcxx-@ABIVER@
abilib_LTLIBRARIES=libcxxtls.la

libcxxtls_la_SOURCES=\
	credentials_callbackobj.C	\
	credentials_certificateobj.C	\
	datumwrapper.C			\
	dhparamsobj.C			\
	digest_algorithm.C		\
	dsaprivkeyobj.C			\
	dsapubkeyobj.C			\
	exceptions.C			\
	fdtlsclientimpl.C		\
	fdtlsclientobj.C		\
	fdtlsserverimpl.C		\
	fdtlsserverobj.C		\
	ftp_client.C			\
	http_auth_internal.C http_auth_internal.H \
	http_serverauth.C		\
	init.C				\
	mdobj.C				\
	pkparamsobj.C			\
	privkeyobj.C			\
	pubkeyobj.C			\
	rdn.C				\
	rsaprivkeyobj.C			\
	rsapubkeyobj.C			\
	sec_param.C			\
	session.C			\
	sessioncacheobj.C		\
	sessionobj.C			\
	useragentobj.C			\
	x509_crtobj.C			\
	x509_privkeyobj.C

libcxxtls_la_LIBADD=../base/libcxx.la
libcxxtls_la_LDFLAGS=-version-info @VERSION_INFO@ $(GNUTLS_LIBS) $(GCRYPT_LIBS)

noinst_PROGRAMS=\
	testftp						\
	testhttpclientimpl				\
	testhttpserverimpl				\
	testmd						\
	testpkparams					\
	testserverauth					\
	testsession					\
	testuseragent_shared				\
	testuseragent_static				\
	testx509cert					\
	testx509privkey

check-am: testx509privkey testx509cert testsession
	./testx509privkey
	rm -rf tlsparamsdir.tst
	mkdir tlsparamsdir.tst
	cp dhparams.dat tlsparamsdir.tst/system.dh
	./testpkparams --set-property x::gnutls::tlsparamsdir=`pwd`/tlsparamsdir.tst
	./testx509cert
	./testsession --set-property x::gnutls::tlsparamsdir=`pwd`/tlsparamsdir.tst
	./testhttpclientimpl --set-property x::gnutls::tlsparamsdir=`pwd`/tlsparamsdir.tst
	./testhttpserverimpl --set-property x::gnutls::tlsparamsdir=`pwd`/tlsparamsdir.tst
	./testuseragent_static --set-property x::gnutls::tlsparamsdir=`pwd`/tlsparamsdir.tst
	./testuseragent_shared --set-property x::gnutls::tlsparamsdir=`pwd`/tlsparamsdir.tst
	./testftp --set-property x::gnutls::tlsparamsdir=`pwd`/tlsparamsdir.tst
	rm -rf testrsa[123].key testrsa*.crt dhparams.dat tlsparamsdir.tst
	./testmd
	./testserverauth

internal-test:
	./testx509privkey
	rm -rf tlsparamsdir.tst
	mkdir tlsparamsdir.tst
	cp dhparams.dat tlsparamsdir.tst/system.dh
	sed -n '/BEGIN CERTIFICATE/,/END CERTIFICATE/p' </etc/pki/tls/certs/proftpd.pem >proftpd.pem
	./testftp --set-property x::gnutls::tlsparamsdir=`pwd`/tlsparamsdir.tst --set-property testftpcert=proftpd.pem --set-property domain=localhost --test
	rm -rf testrsa[123].key testrsa*.crt dhparams.dat tlsparamsdir.tst proftpd.pem


bin_PROGRAMS=tlsparamsgen
sbin_SCRIPTS=tlsparamsgen.sh

testx509privkey_SOURCES=testx509privkey.C
testx509privkey_LDADD=../base/libcxx.la libcxxtls.la
testx509privkey_LDFLAGS=$(LINKTYPE)

testx509cert_SOURCES=testx509cert.C
testx509cert_LDADD=../base/libcxx.la libcxxtls.la
testx509cert_LDFLAGS=$(LINKTYPE) $(GCRYPT_LIBS) $(GNUTLS_LIBS)

testsession_SOURCES=testsession.C
testsession_LDADD=../base/libcxx.la libcxxtls.la
testsession_LDFLAGS=$(LINKTYPE)

testpkparams_SOURCES=testpkparams.C
testpkparams_LDADD=../base/libcxx.la libcxxtls.la
testpkparams_LDFLAGS=$(LINKTYPE)

testftp_SOURCES=testftp.C testftp2.C
testftp_LDADD=../base/libcxx.la libcxxtls.la
testftp_LDFLAGS=-static

testhttpclientimpl_SOURCES=testhttpclientimpl.C testtlsimpl.h
testhttpclientimpl_LDADD=../base/libcxx.la libcxxtls.la
testhttpclientimpl_LDFLAGS=$(LINKTYPE)

testhttpserverimpl_SOURCES=testhttpserverimpl.C testtlsimpl.h
testhttpserverimpl_LDADD=../base/libcxx.la libcxxtls.la
testhttpserverimpl_LDFLAGS=$(LINKTYPE)

testuseragent_static_SOURCES=testuseragent.C
testuseragent_static_LDADD=../base/libcxx.la libcxxtls.la
testuseragent_static_LDFLAGS=-static

testuseragent_shared_SOURCES=testuseragent.C
testuseragent_shared_LDADD=../base/libcxx.la libcxxtls.la

testserverauth_SOURCES=testserverauth.C
testserverauth_LDADD=../base/libcxx.la libcxxtls.la
testserverauth_LDFLAGS=-static

testmd_SOURCES=testmd.C
testmd_LDADD=../base/libcxx.la libcxxtls.la
testmd_LDFLAGS=$(GCRYPT_LIBS)

$(call OPTIONS_GEN,tlsparamsgen.h,$(srcdir)/tlsparamsgen.xml)

tlsparamsgen_SOURCES=tlsparamsgen.C
tlsparamsgen_LDADD=../base/libcxx.la libcxxtls.la

DSACFG=--type dsa --bits 2048 --mode 444 "@localstatedir@/local/tlsparams/system.dh"

install-data-hook:
	test ! -d $(DESTDIR)@localstatedir@/local/tlsparams || exit 0; set -e; mkdir -p $(DESTDIR)@localstatedir@/local/tlsparams; chmod 755 $(DESTDIR)@localstatedir@/local/tlsparams
	./testx509privkey
	mkdir -p $(DESTDIR)@sysconfdir@/tlsparams
	test ! -f $(DESTDIR)@sysconfdir@/tlsparams/system.dh.conf || exit 0; set -e;\
		echo '$(DSACFG)' >$(DESTDIR)@sysconfdir@/tlsparams/system.dh.conf.tmp ; mv $(DESTDIR)@sysconfdir@/tlsparams/system.dh.conf.tmp $(DESTDIR)@sysconfdir@/tlsparams/system.dh.conf; chmod 444 $(DESTDIR)@sysconfdir@/tlsparams/system.dh.conf
	test ! -f "$(DESTDIR)@localstatedir@/local/tlsparams/system.dh$(INSTALL_TLSPARAM_SUFFIX)" || exit 0; set -e; echo '>>>DEFAULT PARAMETERS (to be regenerated when by the startup script)' >system.dh; cat dhparams.dat >>system.dh; mv -f system.dh "$(DESTDIR)@localstatedir@/local/tlsparams/system.dh$(INSTALL_TLSPARAM_SUFFIX)"
	rm -f dhparams.dat testrsa?.key

uninstall-hook:
	test "`cat $(DESTDIR)@sysconfdir@/tlsparams/system.dh.conf 2>/dev/null`" = '$(DSACFG)' || exit 0; rm -f $(DESTDIR)@sysconfdir@/tlsparams/system.dh.conf; rm -f $(DESTDIR)@localstatedir@/local/tlsparams/system.dh
	fgrep -q '>>>DEFAULT PARAMETERS' $(DESTDIR)@localstatedir@/local/tlsparams/system.dh || exit 0; rm -f $(DESTDIR)@localstatedir@/local/tlsparams/system.dh
	-rmdir $(DESTDIR)@localstatedir@/local/tlsparams 2>/dev/null
	-rmdir $(DESTDIR)@sysconfdir@/tlsparams 2>/dev/null
