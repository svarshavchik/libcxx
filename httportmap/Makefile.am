include $(shell pwd)/../includes/x/libcxx.mk

AM_CPPFLAGS=-I . -I $(srcdir) -I $(srcdir)/../includes -I ../includes

sbin_PROGRAMS=httportmapd

noinst_PROGRAMS=testhttportmap testhttportmap2 testhttportmap3 \
	testsingletonapp testsingletonapp2

EXTRA_DIST=$(portmapdata_DATA) apache.conf.in \
	httportmapd.properties httportmap_server.opts.xml

portmapdatadir=$(datadir)/httportmap

portmapdata_DATA=portmap.css portmap.xsl
sysconf_DATA=httportmapd.properties

httportmapd_SOURCES=\
	httportmap_server.C httportmap_server.H \
	portmap_server.C portmap_server.H \
	reexecfd.C reexecfd.H \
	server.C server.H

httportmapd_LDADD=../base/libcxx.la
httportmapd_LDFLAGS=$(TESTLINKTYPE)

BUILT_SOURCES=runstatedir.h portmapdatadir.h httportmap_server.opts.h
CLEANFILES=runstatedir.h portmapdatadir.h httportmap_server.opts.h

$(call OPTIONS_GEN,httportmap_server.opts.h,httportmap_server.opts.xml)

runstatedir.h:
	echo "#define RUNSTATEDIR \"@runstatedir@\"" >runstatedir.h

portmapdatadir.h:
	echo "#define PORTMAPDATADIR \"$(portmapdatadir)\"" >portmapdatadir.h

testhttportmap_SOURCES=testhttportmap.C
testhttportmap_LDADD=../base/libcxx.la
testhttportmap_LDFLAGS=$(TESTLINKTYPE)

testhttportmap2_SOURCES=testhttportmap2.C
testhttportmap2_LDADD=../base/libcxx.la
testhttportmap2_LDFLAGS=$(TESTLINKTYPE)

testhttportmap3_SOURCES=testhttportmap3.C
testhttportmap3_LDADD=../base/libcxx.la
testhttportmap3_LDFLAGS=$(TESTLINKTYPE)

testsingletonapp_SOURCES=testsingletonapp.C
testsingletonapp_LDADD=../base/libcxx.la
testsingletonapp_LDFLAGS=-static

testsingletonapp2_SOURCES=testsingletonapp2.C
testsingletonapp2_LDADD=../base/libcxx.la
testsingletonapp2_LDFLAGS=-static

check-am: testsuite
	@SHELL@ testsuite

install-exec-hook:
	../base/properties --nocheckset --set $(sysconfdir)/httportmapd.properties $(DESTDIR)$(sbindir)/httportmapd
