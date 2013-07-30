# New ports collection makefile for: libcxx
# Date created:		August 3, 2012
# Whom:                 Sam Varshavchik <mrsam@courier-mta.com>
# $FreeBSD$

PORTNAME=		libcxx
PORTVERSION=		0.7
CATEGORIES=		devel

MAINTAINER=		mrsam@courier-mta.com
COMMENT=		LibCXX library

LIB_DEPENDS=		pcre:${PORTSDIR}/devel/pcre \
			gnutls:${PORTSDIR}/security/gnutls \
			gcrypt:${PORTSDIR}/security/libgcrypt \
			unwind:${PORTSDIR}/devel/libunwind \
			intl:${PORTSDIR}/devel/gettext \
			iconv:${PORTSDIR}/converters/libiconv \
			idn:${PORTSDIR}/dns/libidn \
			xml2:${PORTSDIR}/textproc/libxml2 \
			yaml:${PORTSDIR}/textproc/libyaml

BUILD_DEPENDS=		${LOCALBASE}/share/certs/ca-root-nss.crt:${PORTSDIR}/security/ca_root_nss \
			${LOCALBASE}/bin/xsltproc:${PORTSDIR}/textproc/libxslt

RUN_DEPENDS=		$(BUILD_DEPENDS)

# Remove the following when bsd.gcc.mk knows about this
CC=gcc49
CXX=g++49
CPP=cpp49
CFLAGS+=   -L${LOCALBASE}/lib/$(CC) -Wl,-rpath=${LOCALBASE}/lib/$(CC)
CXXFLAGS+= -L${LOCALBASE}/lib/$(CC) -Wl,-rpath=${LOCALBASE}/lib/$(CC)
LDFLAGS+=  -L${LOCALBASE}/lib/$(CC) -rpath=${LOCALBASE}/lib/$(CC)
BUILD_DEPENDS += ${PORTSDIR}/lang/$(CC)
RUN_DEPENDS += ${PORTSDIR}/lang/$(CC)
USE_BINUTILS=yes

USE_BZIP2=1

USE_GMAKE=yes
GNU_CONFIGURE=1
USE_PKGCONFIG=both

CONFIGURE_ARGS=		--sbindir=${PREFIX}/libexec/libcxx CPPFLAGS="-DDEFAULTSOCKET=\\\"${PREFIX}/var/httportmap\\\"" --with-mime-types=${PREFIX}/share/libcxx/mime.types --disable-xlocale
MAN1=			maillogs.1 properties.1
MAN8=			httportmapd.8

MAN1COMPRESSED=		yes
MAN8COMPRESSED=		yes

SUB_FILES=		apache.conf
USE_RC_SUBR=		httportmap

pre-configure:
	PKG_PREFIX=${PREFIX} CC=${CC} WRKDIR=$(WRKDIR) ${SH} ${PKGINSTALL} ${PKGNAME} TEST-GCC

post-install:
	$(INSTALL_DATA) $(WRKDIR)/apache.conf $(PREFIX)/share/libcxx
	${CHMOD} 700 $(PREFIX)/libexec/libcxx/tlsparamsgen.sh
	echo 'Libs: -R$(PREFIX)/lib/$(CC) -R$(PREFIX)/lib -L$(PREFIX)/lib/$(CC) -L$(PREFIX)/lib' >>$(PREFIX)/libdata/pkgconfig/libcxx.pc
	PKG_PREFIX=${PREFIX} ${SH} ${PKGINSTALL} ${PKGNAME} POST-INSTALL
	mkdir -p ${PREFIX}/etc/periodic/monthly
	ln -fs ../../../libexec/libcxx/tlsparamsgen.sh ${PREFIX}/etc/periodic/monthly/tlsparamsgen.sh

post-deinstall:
	PKG_PREFIX=${PREFIX} ${SH} ${PKGDEINSTALL} ${PKGNAME} POST-DEINSTALL

.include <bsd.port.mk>
