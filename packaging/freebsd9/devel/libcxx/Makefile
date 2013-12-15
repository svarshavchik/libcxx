# New ports collection makefile for: libcxx
# Date created:		August 3, 2012
# Whom:                 Sam Varshavchik <mrsam@courier-mta.com>
# $FreeBSD$

PORTNAME=		libcxx
PORTVERSION=		0.8
CATEGORIES=		devel

MAINTAINER=		mrsam@courier-mta.com
COMMENT=		LibCXX library

LIB_DEPENDS=		pcre:${PORTSDIR}/devel/pcre \
			gcrypt:${PORTSDIR}/security/libgcrypt \
			unwind:${PORTSDIR}/devel/libunwind \
			intl:${PORTSDIR}/devel/gettext \
			iconv:${PORTSDIR}/converters/libiconv \
			idn:${PORTSDIR}/dns/libidn \
			xml2:${PORTSDIR}/textproc/libxml2 \
			yaml:${PORTSDIR}/textproc/libyaml

RUN_DEPENDS=		gnutls3>=3.1.0:${PORTSDIR}/security/gnutls3
BUILD_DEPENDS=		${LOCALBASE}/share/certs/ca-root-nss.crt:${PORTSDIR}/security/ca_root_nss \
			${LOCALBASE}/bin/xsltproc:${PORTSDIR}/textproc/libxslt\
                        $(RUN_DEPENDS)


# Remove the following when bsd.gcc.mk knows about this
#CC=gcc49
#CXX=g++49
#CPP=cpp49
#CFLAGS+=   -L${LOCALBASE}/lib/$(CC) -Wl,-rpath=${LOCALBASE}/lib/$(CC)
#CXXFLAGS+= -L${LOCALBASE}/lib/$(CC) -Wl,-rpath=${LOCALBASE}/lib/$(CC)
#LDFLAGS+=  -L${LOCALBASE}/lib/$(CC) -rpath=${LOCALBASE}/lib/$(CC)
#BUILD_DEPENDS += ${PORTSDIR}/lang/$(CC)
#RUN_DEPENDS += ${PORTSDIR}/lang/$(CC)
USE_GCC=4.8

USE_BINUTILS=yes

USE_BZIP2=1

USE_GMAKE=yes
GNU_CONFIGURE=1
USE_PKGCONFIG=both

CONFIGURE_ARGS=		--sbindir=${PREFIX}/libexec/libcxx CPPFLAGS="-I/usr/local/include/gnutls3 -DDEFAULTSOCKET=\\\"${PREFIX}/var/httportmap\\\"" --with-mime-types=${PREFIX}/share/libcxx/mime.types --disable-xlocale
MAN1=			maillogs.1 properties.1
MAN8=			httportmapd.8

MAN1COMPRESSED=		yes
MAN8COMPRESSED=		yes

SUB_FILES=		apache.conf
USE_RC_SUBR=		httportmap

pre-configure:
	PKG_PREFIX=${PREFIX} CC=${CC} WRKDIR=$(WRKDIR) ${SH} ${PKGINSTALL} ${PKGNAME} TEST-GCC

post-install:
	mkdir -p $(STAGEDIR)$(PREFIX)/share/libcxx
	$(INSTALL_DATA) $(WRKDIR)/apache.conf $(STAGEDIR)$(PREFIX)/share/libcxx
	${CHMOD} 700 $(STAGEDIR)$(PREFIX)/libexec/libcxx/tlsparamsgen.sh
	sed "s@gnutls@gnutls3@;s@^Cflags:@Cflags: -I$(PREFIX)/include/gnutls3@" <$(STAGEDIR)$(PREFIX)/libdata/pkgconfig/libcxx.pc >$(STAGEDIR)$(PREFIX)/libdata/pkgconfig/libcxx.pc.tmp
	echo 'Libs: -R$(PREFIX)/lib/$(CC) -R$(PREFIX)/lib -L$(PREFIX)/lib/$(CC) -L$(PREFIX)/lib' >>$(STAGEDIR)$(PREFIX)/libdata/pkgconfig/libcxx.pc.tmp
	mv -f $(STAGEDIR)$(PREFIX)/libdata/pkgconfig/libcxx.pc.tmp $(STAGEDIR)$(PREFIX)/libdata/pkgconfig/libcxx.pc
	chmod 444 $(STAGEDIR)$(PREFIX)/libdata/pkgconfig/libcxx.pc
	for f in /etc/tlsparams/system.dh.conf /var/tlsparams/system.dh ; do mv -f $(STAGEDIR)$(PREFIX)$$f $(STAGEDIR)$(PREFIX)$$f.dist; done
	# PKG_PREFIX=${PREFIX} ${SH} ${PKGINSTALL} ${PKGNAME} POST-INSTALL
	mkdir -p $(STAGEDIR)$(PREFIX)/etc/periodic/monthly
	ln -fs ../../../libexec/libcxx/tlsparamsgen.sh $(STAGEDIR)$(PREFIX)/etc/periodic/monthly/tlsparamsgen.sh

post-deinstall:
	# PKG_PREFIX=${PREFIX} ${SH} ${PKGDEINSTALL} ${PKGNAME} POST-DEINSTALL

.include <bsd.port.mk>
