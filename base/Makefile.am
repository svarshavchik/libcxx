#
# Copyright 2012 Double Precision, Inc.
# See COPYING for distribution information.
#

include $(shell pwd)/../includes/libcxx.mk

AM_CPPFLAGS=-I . -I $(srcdir)/../includes -I ../includes

LDFLAGS += -export-dynamic

EXTRA_DIST=\
	propertiescli.xml \
	testcgiimpl.tst \
	testcgiimpl.txt \
	testfactory.tst \
	testfmtsize.txt \
	testgetprop.tst \
	testhttpclientauth.txt \
	testhttpmessageimpl.txt \
	testinpseq.tst \
	testlocale.txt \
	testlockpool.tst \
	testlogger.tst \
	testlogger.txt \
	testlogrotate.txt \
	testmessages.po \
	testmessages.txt \
	testobj.tst \
	testoptgen.sh \
	testoptgen.txt \
	testoptions.txt \
	testoutpseq.tst \
	testprop.txt \
	testseq.tst \
	testserialize.txt \
	testsingleton.tst \
	testtzfile.txt \
	testuri.txt \
	testweak2.tst \
	testweak3.tst \
	testweak.tst \
	testworkerpool.txt \
	testymd.txt

TESTLINKTYPE=

lib_LTLIBRARIES=libcxx.la

libcxx_la_SOURCES=\
	addoverflow.h		\
	gettext_in.h		\
	ymd_internal.h		\
\
	basicattr.C		\
	basiccodecvtcommon.C	\
	chrcasecmp.C		\
	condobj.C		\
	csv.C			\
	dequemsgdispatcher.C	\
	destroycallbackflagobj.C \
	destroycallbackflagwait4obj.C \
	destroycallbackobj.C	\
	df.C			\
	dir.C			\
	dirobj.C		\
	dirwalkobj.C		\
	dirwalktreeobj.C	\
	dispatchablebaseobj.C	\
	epoll.C			\
	epollobj.C		\
	eventcallback.C		\
	eventdestroynotify.C	\
	eventfdobj.C		\
	eventqueuemsgdispatcher.C \
	eventregistrationobj.C	\
	exception.C		\
	exceptionobj.C		\
	fd.C			\
	fditer.C		\
	fdlistenerimplobj.C	\
	fdlistenerobj.C		\
	fdobj.C			\
	fdreadlimitobj.C	\
	fdstreambufobj.C	\
	fdtimeoutconfig.C	\
	fdtimeoutobj.C		\
	fileattrobj.C		\
	filestat.C		\
	fmtsize.C		\
	forkexec.C              \
	getlinecrlf.C		\
	globlock.C		\
	globobj.C		\
	grp.C			\
	headersbase.C		\
	headersimpl.C		\
	hier.C			\
	hms.C			\
	http_accept_header.C	\
	http_auth_internal.H    \
	http_cgiimpl.C		\
	http_clientauthimplobj.C \
	http_clientauthcacheobj.C \
	http_clientauthobj.C	\
	http_clientimpl.C	\
	http_content_type_header.C \
	http_cookie.C           \
	http_cookiejarobj.C     \
	http_cookiemrulist.C    \
	http_domaincookies.C http_domaincookies.H \
	http_exception.C	\
	http_fdclientimpl.C	\
	http_fdclientobj.C	\
	http_fdimplbase.C	\
	http_fdserverobj.C	\
	http_fdserverimpl.C	\
	http_form.C		\
	http_messageimpl.C	\
	http_pathcookies.C http_pathcookies.H \
	http_receiverimpl.C	\
	http_requestimpl.C	\
	http_responseimpl.C	\
	http_senderimpl.C	\
	http_serverimpl.C	\
	http_storedcookie.C http_storedcookie.H \
	http_useragentobj.C	\
	httportmap.C		\
	iconviofilter.C		\
	interval.C		\
	join.C			\
	kqueuenb_internal.h	\
	localeobj.C		\
	localscope.C		\
	logger.C		\
	messages.C		\
	mime_contentheadercollector.C \
	mime_headercollector.C	\
	mime_headeriter.C	\
	mime_rfc2047.C          \
	msgdispatcher.C		\
	mutexobj.C		\
	mutexpool.C		\
	netaddrobj.C		\
	netaddrresultobj.C	\
	netifobj.C		\
	obj.C			\
	ondestroyobj.C		\
	opendirobj.C		\
	option_definitionbaseobj.C \
	option_definitionhelpobj.C \
	option_definitionobj.C  \
	option_definitionpropertiesobj.C \
	option_definitionsetpropertyobj.C \
	option_definitionusageobj.C \
	option_groupdefinitionobj.C \
	option_listobj.C	\
	option_mutexobj.C	\
	option_parserobj.C	\
	option_valuebaseobj.C	\
	pcreobj.C		\
	pidinfo.C pidinfo_internal.h \
	properties.C		\
	pwd.C			\
	refcnt.C		\
	runthreadbaseobj.C	\
	runthreadname.C		\
	runthreadsingleton.C	\
	rwlockobj.C		\
	rwlockpool.C		\
	rwmutex.C		\
	rwmutexdebug.C		\
	semaphore.C		\
	serialization.C		\
	servent.C		\
	sighandlerobj.C		\
	signalfdobj.C		\
	sigset.C		\
	singletonapp.C		\
	sockaddrobj.C		\
	stopexception.C		\
	stoppable.C		\
	timerfdobj.C		\
	timerobj.C		\
	timerobj_internal.H	\
	timertaskentry_internal.H \
	timertaskobj.C		\
	tokens.C		\
	tzfileobj.C		\
	tz_internal.H		\
	uriimpl.C		\
	uuid.C			\
	value_string.C		\
	vectorobj.C		\
	vipobj.C		\
	weakinfo.C		\
	weakref.C		\
	wordexp.C		\
	workerpoolobj.C		\
	xlocale.C		\
	xml_escape.C		\
	ymd.C			\
	ymd2.C			\
	ymdhms.C		\
	ymdhmsbase.C

$(call MSGDISPATCHER_GEN,singletonapp.msgs,all)

$(call MSGDISPATCHER_GEN,timerobj.msgs,def decl)

BUILT_SOURCES=x ymd_grdjd.h mksiglist.h mksigtab.h testoptgen.h maillogs.h propertiescli.h
CLEANFILES=x ymd_grdjd.h mksiglist.h mksigtab.h testoptgen.h maillogs.h propertiescli.h

XTEST_MO_DIR=xtest/en_US/LC_MESSAGES
XTEST_MO_FILE=xtest.mo

clean-local:
	rm -rf xtest

x:
	ln -s $(srcdir)/../includes x

ymd_grdjd.h: mkgrdjd
	./mkgrdjd >ymd_grdjd.h.tmp
	mv -f ymd_grdjd.h.tmp ymd_grdjd.h

mksiglist.h: mksiglist
	./mksiglist >mksiglist.h.tmp
	mv -f mksiglist.h.tmp mksiglist.h

mksigtab.h: mksigtab
	./mksigtab >mksigtab.h.tmp
	mv -f mksigtab.h.tmp mksigtab.h

libcxx_la_LIBADD=-lpthread -lrt -lpcre @LIBEXTRAPATH@ @LIBUNWIND@ @LINKLIBICONV@ @LINKLIBINTL@

libcxx_la_LDFLAGS=-version-info @VERSION_INFO@

noinst_PROGRAMS=\
	mkgrdjd                       \
	mksiglist                     \
	mksigtab                      \
	testcgiimpl                   \
	testcodecvt                   \
	testcredentials               \
	testcsv                       \
	testdestroycallbackwait4      \
	testeventcallback             \
	testfactory                   \
	testfactory2                  \
	testfdclientimpl              \
	testfdlistener                \
	testfdserverimpl              \
	testfdtimeouts                \
	testfmtsize                   \
	testforkexec                  \
	testfutime                    \
	testgetprop                   \
	testgetpwgr                   \
	testglob                      \
	testhier                      \
	testhttpclientauth            \
	testhttpclientimpl            \
	testhttpmessageimpl           \
	testhttpresponseimpl          \
	testhttpserverimpl            \
	testiconviofilter             \
	testinpseq                    \
	testjoin                      \
	testlocale                    \
	testlockpool                  \
	testlogger                    \
	testlogrotate                 \
	testmcguffinref               \
	testmessages                  \
	testmimebodystartiter         \
	testmimeheadercollector       \
	testmimenewlineiter           \
	testmimerfc2047               \
	testmimesectioniter           \
	testmsgdispatcher             \
	testnetif                     \
	testobj                       \
	testoptgen                    \
	testoptions                   \
	testoutpseq                   \
	testpcre                      \
	testpidinfo                   \
	testpostponedcall	      \
	testprop                      \
	testqp                        \
	testrefiterator               \
	testrun                       \
	testrunsingleton	      \
	testrwmutex                   \
	testsemaphore                 \
	testsendrecvfd                \
	testseq                       \
	testserialize                 \
	testsignalfd                  \
	testsingleton                 \
	teststoppable                 \
	testthreadlocal               \
	testthreadmsgdispatcher       \
	testtimer                     \
	testtzfile                    \
	testuri                       \
	testuseragent                 \
	testuuid                      \
	testweak                      \
	testweak2                     \
	testweak3                     \
	testworkerpool                \
	testxmlescape                 \
	testymd

mkgrdjd_SOURCES=mkgrdjd.C

mksiglist_SOURCES=mksiglist.C

mksigtab_SOURCES=mksigtab.C

mksigtab.o: mksiglist.h

testobj_SOURCES=testobj.C
testobj_LDADD=libcxx.la
testobj_LDFLAGS=$(TESTLINKTYPE)

testrefiterator_SOURCES=testrefiterator.C
testrefiterator_LDADD=libcxx.la
testrefiterator_LDFLAGS=$(TESTLINKTYPE)

testqp_SOURCES=testqp.C
testqp_LDADD=libcxx.la
testqp_LDFLAGS=$(TESTLINKTYPE)

testfutime_SOURCES=testfutime.C
testfutime_LDADD=libcxx.la
testfutime_LDFLAGS=$(TESTLINKTYPE)

testinpseq_SOURCES=testinpseq.C
testinpseq_LDADD=libcxx.la
testinpseq_LDFLAGS=$(TESTLINKTYPE)

testoutpseq_SOURCES=testoutpseq.C
testoutpseq_LDADD=libcxx.la
testoutpseq_LDFLAGS=$(TESTLINKTYPE)

testweak_SOURCES=testweak.C
testweak_LDADD=libcxx.la
testweak_LDFLAGS=$(TESTLINKTYPE)

testweak2_SOURCES=testweak2.C
testweak2_LDADD=libcxx.la
testweak2_LDFLAGS=$(TESTLINKTYPE)

testweak3_SOURCES=testweak3.C
testweak3_LDADD=libcxx.la
testweak3_LDFLAGS=$(TESTLINKTYPE)

testseq_SOURCES=testseq.C
testseq_LDADD=libcxx.la
testseq_LDFLAGS=$(TESTLINKTYPE)

testlogger_SOURCES=testlogger.C
testlogger_LDADD=libcxx.la
testlogger_LDFLAGS=$(TESTLINKTYPE)

testlogrotate_SOURCES=testlogrotate.C
testlogrotate_LDADD=libcxx.la
testlogrotate_LDFLAGS=$(TESTLINKTYPE)

testoptions_SOURCES=testoptions.C
testoptions_LDADD=libcxx.la
testoptions_LDFLAGS=$(TESTLINKTYPE)

testlocale_SOURCES=testlocale.C
testlocale_LDADD=libcxx.la
testlocale_LDFLAGS=$(TESTLINKTYPE)

testgetprop_SOURCES=testgetprop.C
testgetprop_LDADD=libcxx.la
testgetprop_LDFLAGS=$(TESTLINKTYPE)

testfactory_SOURCES=testfactory.C
testfactory_LDADD=libcxx.la
testfactory_LDFLAGS=$(TESTLINKTYPE)

testfactory2_SOURCES=testfactory2.C
testfactory2_LDADD=libcxx.la
testfactory2_LDFLAGS=$(TESTLINKTYPE)

testprop_SOURCES=testprop.C
testprop_LDADD=libcxx.la
testprop_LDFLAGS=$(TESTLINKTYPE)

testlockpool_SOURCES=testlockpool.C
testlockpool_LDADD=libcxx.la
testlockpool_LDFLAGS=$(TESTLINKTYPE)

testuuid_SOURCES=testuuid.C
testuuid_LDADD=libcxx.la
testuuid_LDFLAGS=$(TESTLINKTYPE)

testymd_SOURCES=testymd.C
testymd_LDADD=libcxx.la
testymd_LDFLAGS=$(TESTLINKTYPE)

testsingleton_SOURCES=testsingleton.C
testsingleton_LDADD=libcxx.la
testsingleton_LDFLAGS=$(TESTLINKTYPE)

testmessages_SOURCES=testmessages.C
testmessages_LDADD=libcxx.la
testmessages_LDFLAGS=$(TESTLINKTYPE)

testtzfile_SOURCES=testtzfile.C
testtzfile_LDADD=libcxx.la
testtzfile_LDFLAGS=$(TESTLINKTYPE)

testserialize_SOURCES=testserialize.C
testserialize_LDADD=libcxx.la
testserialize_LDFLAGS=$(TESTLINKTYPE)

testsendrecvfd_SOURCES=testsendrecvfd.C
testsendrecvfd_LDADD=libcxx.la
testsendrecvfd_LDFLAGS=$(TESTLINKTYPE)

testcredentials_SOURCES=testcredentials.C
testcredentials_LDADD=libcxx.la
testcredentials_LDFLAGS=$(TESTLINKTYPE)

testnetif_SOURCES=testnetif.C
testnetif_LDADD=libcxx.la
testnetif_LDFLAGS=$(TESTLINKTYPE)

testglob_SOURCES=testglob.C
testglob_LDADD=libcxx.la
testglob_LDFALGS=$(TESTLINKTYPE)

testgetpwgr_SOURCES=testgetpwgr.C
testgetpwgr_LDADD=libcxx.la
testgetpwgr_LDFLAGS=$(TESTLINKTYPE)

testfmtsize_SOURCES=testfmtsize.C
testfmtsize_LDADD=libcxx.la
testfmtsize_LDFLAGS=$(TESTLINKTYPE)

testuri_SOURCES=testuri.C
testuri_LDADD=libcxx.la
testuri_LDFLAGS=$(TESTLINKTYPE)

testhttpmessageimpl_SOURCES=testhttpmessageimpl.C
testhttpmessageimpl_LDADD=libcxx.la
testhttpmessageimpl_LDFLAGS=$(TESTLINKTYPE)

testhttpresponseimpl_SOURCES=testhttpresponseimpl.C
testhttpresponseimpl_LDADD=libcxx.la
testhttpresponseimpl_LDFLAGS=$(TESTLINKTYPE)

testhttpserverimpl_SOURCES=testhttpserverimpl.C
testhttpserverimpl_LDADD=libcxx.la
testhttpserverimpl_LDFLAGS=$(TESTLINKTYPE)

testhttpclientimpl_SOURCES=testhttpclientimpl.C
testhttpclientimpl_LDADD=libcxx.la
testhttpclientimpl_LDFLAGS=$(TESTLINKTYPE)

testiconviofilter_SOURCES=testiconviofilter.C
testiconviofilter_LDADD=libcxx.la
testiconviofilter_LDFLAGS=$(TESTLINKTYPE)

testxmlescape_SOURCES=testxmlescape.C
testxmlescape_LDADD=libcxx.la
testxmlescape_LDFLAGS=$(TESTLINKTYPE)

testfdtimeouts_SOURCES=testfdtimeouts.C
testfdtimeouts_LDADD=libcxx.la
testfdtimeouts_LDFLAGS=$(TESTLINKTYPE)

testfdserverimpl_SOURCES=testfdserverimpl.C testimpl.h testfdserverimpl.h
testfdserverimpl_LDADD=libcxx.la
testfdserverimpl_LDFLAGS=-static

testfdclientimpl_SOURCES=testfdclientimpl.C testimpl.h testfdclientimpl.h
testfdclientimpl_LDADD=libcxx.la
testfdclientimpl_LDFLAGS=$(TESTLINKTYPE)

testfdlistener_SOURCES=testfdlistener.C
testfdlistener_LDADD=libcxx.la
testfdlistener_LDFLAGS=-static

testcsv_SOURCES=testcsv.C
testcsv_LDADD=libcxx.la
testcsv_LDFLAGS=$(TESTLINKTYPE)

testcgiimpl_SOURCES=testcgiimpl.C
testcgiimpl_LDADD=libcxx.la
testcgiimpl_LDFLAGS=$(TESTLINKTYPE)

testuseragent_SOURCES=testuseragent.C
testuseragent_LDADD=libcxx.la
testuseragent_LDFLAGS=-static

teststoppable_SOURCES=teststoppable.C
teststoppable_LDADD=libcxx.la
teststoppable_LDFLAGS=$(TESTLINKTYPE)

testthreadmsgdispatcher_SOURCES=testthreadmsgdispatcher.C
testthreadmsgdispatcher_LDADD=libcxx.la
testthreadmsgdispatcher_LDFLAGS=$(TESTLINKTYPE)

testmsgdispatcher_SOURCES=testmsgdispatcher.C
testmsgdispatcher_LDADD=libcxx.la
testmsgdispatcher_LDFLAGS=$(TESTLINKTYPE)

$(call MSGDISPATCHER_GEN,testmsgdispatcher.testclass1,all,dispatch=public)
$(call MSGDISPATCHER_GEN,testmsgdispatcher.testclass2,def decl)

testsemaphore_SOURCES=testsemaphore.C
testsemaphore_LDADD=libcxx.la
testsemaphore_LDFLAGS=$(TESTLINKTYPE)

$(call OPTIONS_GEN,testoptgen.h.tmp,$(srcdir)/../includes/optgen.sample.xml)

testoptgen.h: testoptgen.h.tmp
	perl -p -e 's/^#include.*//;s/\b@LIBCXX_NAMESPACE@::/LIBCXX_NAMESPACE::/g' <testoptgen.h.tmp >testoptgen.h.tmp2
	mv testoptgen.h.tmp2 testoptgen.h

testoptgen_SOURCES=testoptgen.C
testoptgen_LDADD=libcxx.la
testoptgen_LDFLAGS=$(TESTLINKTYPE)

testeventcallback_SOURCES=testeventcallback.C
testeventcallback_LDADD=libcxx.la
testeventcallback_LDFLAGS=$(TESTLINKTYPE)

testmcguffinref_SOURCES=testmcguffinref.C
testmcguffinref_LDADD=libcxx.la
testmcguffinref_LDFLAGS=$(TESTLINKTYPE)

testcodecvt_SOURCES=testcodecvt.C
testcodecvt_LDADD=libcxx.la
testcodecvt_LDFLAGS=$(TESTLINKTYPE)

testsignalfd_SOURCES=testsignalfd.C
testsignalfd_LDADD=libcxx.la
testsignalfd_LDFLAGS=$(TESTLINKTYPE)

testthreadlocal_SOURCES=testthreadlocal.C
testthreadlocal_LDADD=libcxx.la
testthreadlocal_LDFLAGS=$(TESTLINKTYPE)

testjoin_SOURCES=testjoin.C
testjoin_LDADD=libcxx.la
testjoin_LDFLAGS=$(TESTLINKTYPE)

testrwmutex_SOURCES=testrwmutex.C
testrwmutex_LDADD=libcxx.la
testrwmutex_LDFLAGS=$(TESTLINKTYPE)

testrun_SOURCES=testrun.C
testrun_LDADD=libcxx.la
testrun_LDFLAGS=$(TESTLINKTYPE)

testrunsingleton_SOURCES=testrunsingleton.C
testrunsingleton_LDADD=libcxx.la
testrunsingleton_LDFLAGS=$(TESTLINKTYPE)

testtimer_SOURCES=testtimer.C
testtimer_LDADD=libcxx.la
testtimer_LDFLAGS=$(TESTLINKTYPE)

testworkerpool_SOURCES=testworkerpool.C
testworkerpool_LDADD=libcxx.la
testworkerpool_LDFLAGS=$(TESTLINKTYPE)

testpostponedcall_SOURCES=testpostponedcall.C
testpostponedcall_LDADD=libcxx.la
testpostponedcall_LDFLAGS=$(TESTLINKTYPE)

testdestroycallbackwait4_SOURCES=testdestroycallbackwait4.C
testdestroycallbackwait4_LDADD=libcxx.la
testdestroycallbackwait4_LDFLAGS=$(TESTLINKTYPE)

testpidinfo_SOURCES=testpidinfo.C
testpidinfo_LDADD=libcxx.la
testpidinfo_LDFLAGS=-static

testpcre_SOURCES=testpcre.C
testpcre_LDADD=libcxx.la
testpcre_LDFLAGS=$(TESTLINKTYPE)

testforkexec_SOURCES=testforkexec.C
testforkexec_LDADD=libcxx.la
testforkexec_LDFLAGS=$(TESTLINKTYPE)

testhier_SOURCES=testhier.C
testhier_LDADD=libcxx.la
testhier_LDFLAGS=$(TESTLINKTYPE)

testhttpclientauth_SOURCES=testhttpclientauth.C
testhttpclientauth_LDADD=libcxx.la
testhttpclientauth_LDFLAGS=$(TESTLINKTYPE)

testmimenewlineiter_SOURCES=testmimenewlineiter.C
testmimenewlineiter_LDADD=libcxx.la
testmimenewlineiter_LDFLAGS=$(TESTLINKTYPE)

testmimebodystartiter_SOURCES=testmimebodystartiter.C
testmimebodystartiter_LDADD=libcxx.la
testmimebodystartiter_LDFLAGS=$(TESTLINKTYPE)

testmimeheadercollector_SOURCES=testmimeheadercollector.C
testmimeheadercollector_LDADD=libcxx.la
testmimeheadercollector_LDFLAGS=$(TESTLINKTYPE)

testmimesectioniter_SOURCES=testmimesectioniter.C
testmimesectioniter_LDADD=libcxx.la
testmimesectioniter_LDFLAGS=$(TESTLINKTYPE)

testmimerfc2047_SOURCES=testmimerfc2047.C
testmimerfc2047_LDADD=libcxx.la
testmimerfc2047_LDFLAGS=$(TESTLINKTYPE)

propertiescli.h : propertiescli.h.tmp
	perl -p -e 's/^#include.*//;s/\b@LIBCXX_NAMESPACE@::/LIBCXX_NAMESPACE::/g' <propertiescli.h.tmp >propertiescli.h.tmp2
	mv propertiescli.h.tmp2 propertiescli.h

maillogs.h : maillogs.h.tmp
	perl -p -e 's/^#include.*//;s/\b@LIBCXX_NAMESPACE@::/LIBCXX_NAMESPACE::/g' <maillogs.h.tmp >maillogs.h.tmp2
	mv maillogs.h.tmp2 maillogs.h

$(call OPTIONS_GEN,propertiescli.h.tmp,$(srcdir)/propertiescli.xml)
$(call OPTIONS_GEN,maillogs.h.tmp,$(srcdir)/maillogs.xml)

bin_PROGRAMS=properties maillogs

properties_SOURCES=propertiescli.C
properties_LDADD=libcxx.la

maillogs_SOURCES=maillogs.C
maillogs_LDADD=libcxx.la
maillogs_LDFLAGS=-static

noinst_DATA=$(XTEST_MO_DIR)/$(XTEST_MO_FILE)

check-am: $(noinst_PROGRAMS) $(noinst_DATA)
	./testrwmutex
	./testpostponedcall
	./testsingleton 2>testsingleton.tst.tmp >&2
	diff $(srcdir)/testsingleton.tst testsingleton.tst.tmp
	rm -f testsingleton.tst.tmp
	./testuuid
	./testymd 2>testymd.txt.tmp >&2
	diff $(srcdir)/testymd.txt testymd.txt.tmp
	rm -f testymd.txt.tmp
	./testobj 2>test.obj.tmp >&2
	diff $(srcdir)/testobj.tst test.obj.tmp
	rm test.obj.tmp
	./testrefiterator
	./testqp
	./testrun
	./testrunsingleton
	./testdestroycallbackwait4
	./testtimer
	./testthreadmsgdispatcher
	./testthreadlocal
	echo "foo . bar = foo " >testgetprop.propfile
	echo "foo.baz=ba" >>testgetprop.propfile
	echo "foo.baz=baa" >testgetprop.propfile2
	PROPERTIES=testgetprop.propfile ./testgetprop "foo" "foo.bar" "foo.baz" "foo.bar.baz" "foo.bar" >testgetprop.tmp
	diff $(srcdir)/testgetprop.tst testgetprop.tmp
	./testgetprop --load-files=testgetprop.propfile --load-files=testgetprop.propfile2 "foo" "foo.bar" "foo.baz" "foo.bar.baz" "foo.bar" >testgetprop.tmp
	diff $(srcdir)/testgetprop.tst testgetprop.tmp
	rm -f testgetprop.tmp
	echo "dummy" >testgetprop.propfile
	PROPERTIES=testgetprop.propfile ./testgetprop "foo" "foo.bar" "foo.baz" "foo.bar.baz" "foo.bar"
	./testgetprop --load-files=testgetprop.propfile "foo" "foo.bar" "foo.baz" "foo.bar.baz" "foo.bar" || exit 0; exit 1
	rm -f testgetprop.propfile testgetprop.propfile2
	test "`./testgetprop --load-string 'foo.bar=foo' --load-string 'foo.bar=foobar' --update-string 'baz=baz' --update-string 'baz=foobar' foo.bar baz`" = "`echo foo; echo foobar`"
	echo "foo=foo" >testgetprop.propfile
	echo "bar=bar" >>testgetprop.propfile
	echo "bar=foobar" >testgetprop.propfile2
	echo "baz=baz" >>testgetprop.propfile2
	test "`./testgetprop --update-files=testgetprop.propfile --update-files=testgetprop.propfile2 foo bar baz`" = "`echo foo; echo foobar; echo baz`"
	echo 'intvalue=foo' >testgetprop.propfile
	PROPERTIES=testgetprop.propfile ./testgetprop
	./testgetprop --load-string 'intvalue=foo'
	./testgetprop --load-files testgetprop.propfile
	./testgetprop --update-string 'intvalue=foo' || exit 0; exit 1
	./testgetprop --update-files testgetprop.propfile || exit 0; exit 1
	echo '#LOCALE en_US.ISO8859-1' >testgetprop.propfile
	echo 'hello=world' | tr 'o' '\363' >>testgetprop.propfile
	test "`PROPERTIES=testgetprop.propfile LANG=en_US.UTF-8  ./testgetprop helló`" = "wórld"
	rm -f testgetprop.propfile testgetprop.propfile2
	./testinpseq 2>test.inpseq.tmp >&2
	diff $(srcdir)/testinpseq.tst test.inpseq.tmp
	rm test.inpseq.tmp
	./testoutpseq 2>test.outpseq.tmp >&2
	diff $(srcdir)/testoutpseq.tst test.outpseq.tmp
	rm test.outpseq.tmp
	./testweak 2>test.weak.tmp >&2
	diff $(srcdir)/testweak.tst test.weak.tmp
	rm test.weak.tmp
	./testweak2 2>test.weak2.tmp >&2
	diff $(srcdir)/testweak2.tst test.weak2.tmp
	rm test.weak2.tmp
	./testweak3 2>test.weak3.tmp >&2
	diff $(srcdir)/testweak3.tst test.weak3.tmp
	rm test.weak3.tmp
	./testseq 2>test.seq.tmp >&2
	diff $(srcdir)/testseq.tst test.seq.tmp
	rm test.seq.tmp
	rm -rf testlogdir; PROPERTIES=$(srcdir)/testlogger.txt ./testlogger 2>testlogger.tst.tmp >&2
	diff $(srcdir)/testlogger.tst testlogger.tst.tmp
	rm testlogger.tst.tmp
	rm -rf testlogdir
	mkdir -p testlogdir/2000/01/01
	>testlogdir/2000/01/01/01.log
	>testlogdir/2000/01/01/02.log
	>testlogdir/2000/01/01/03.log
	>testlogdir/2000/01/01/04.log
	>testlogdir/2000/01/01/05.log
	touch -t "200001010100" testlogdir/2000/01/01/01.log
	touch -t "200001010200" testlogdir/2000/01/01/02.log
	touch -t "200001010300" testlogdir/2000/01/01/03.log
	touch -t "200001010400" testlogdir/2000/01/01/04.log
	touch -t "200001010500" testlogdir/2000/01/01/05.log
	PROPERTIES=$(srcdir)/testlogrotate.txt ./testlogrotate
	test "`echo testlogdir/*/*/*/*`" = "testlogdir/2000/01/01/02.log testlogdir/2000/01/01/03.log testlogdir/2000/01/01/04.log testlogdir/2000/01/01/05.log testlogdir/2000/02/01/20000201120000.log"
	rm -rf testlogdir
	./testoptions 2>&1 >testoptions.tmp
	diff $(srcdir)/testoptions.txt testoptions.tmp
	rm -rf testoptions.tmp
	./testlocale 2>testlocale.tmp >&2
	diff $(srcdir)/testlocale.txt testlocale.tmp
	rm -rf testlocale.tmp
	./testfactory >testfactory.tmp 2>&1
	./testfactory2 >>testfactory.tmp 2>&1
	diff $(srcdir)/testfactory.tst testfactory.tmp
	rm -rf testfactory.tmp
	LC_ALL=en_US.UTF-8 ./testprop >testprop.tmp 2>&1
	diff $(srcdir)/testprop.txt testprop.tmp
	rm -rf testprop.tmp
	./testlockpool >testlockpool.tmp 2>&1
	diff $(srcdir)/testlockpool.tst testlockpool.tmp
	rm -rf testlockpool.tmp
	./testmessages >testmessages.tmp 2>&1
	diff $(srcdir)/testmessages.txt testmessages.tmp
	rm -rf testmessages.tmp
	./testtzfile >testtzfile.tmp 2>&1
	diff $(srcdir)/testtzfile.txt testtzfile.tmp
	rm -rf testtzfile.tmp
	./testserialize >testserialize.tmp 2>&1
	diff $(srcdir)/testserialize.txt testserialize.tmp
	rm -rf testserialize.tmp
	./testworkerpool >testworkerpool.tmp 2>&1
	diff $(srcdir)/testworkerpool.txt testworkerpool.tmp
	rm -f testworkerpool.tmp
	./testsendrecvfd
	./testcredentials
	./testgetpwgr
	./testfmtsize >testfmtsize.tmp
	diff $(srcdir)/testfmtsize.txt testfmtsize.tmp
	rm -f testfmtsize.tmp
	./testnetif
	./testuri >testuri.tmp
	diff $(srcdir)/testuri.txt testuri.tmp
	rm -f testuri.tmp
	./testhttpmessageimpl >testhttpmessageimpl.tmp 2>&1
	diff $(srcdir)/testhttpmessageimpl.txt testhttpmessageimpl.tmp
	rm testhttpmessageimpl.tmp
	./testhttpresponseimpl
	./testiconviofilter
	./testxmlescape
	./testfdtimeouts
	./testhttpserverimpl
	./testhttpclientimpl
	./testfdserverimpl
	./testfdclientimpl
	./testfdlistener
	./testhttpclientauth >testhttpclientauth.tmp
	diff $(srcdir)/testhttpclientauth.txt testhttpclientauth.tmp
	rm -f testhttpclientauth.tmp
	./testuseragent
	./testcsv
	./testeventcallback
	./testmcguffinref
	./testcodecvt
	@SHELL@ $(srcdir)/testcgiimpl.tst 2>&1 | diff $(srcdir)/testcgiimpl.txt -
	@SHELL@ $(srcdir)/testoptgen.sh | diff $(srcdir)/testoptgen.txt -
	./testsemaphore
	./testsignalfd
	./testjoin
	./testpidinfo
	./testforkexec
	./testhier
	./testmimenewlineiter
	./testmimebodystartiter
	./testmimeheadercollector
	./testmimesectioniter
	./testmimerfc2047

$(XTEST_MO_DIR)/$(XTEST_MO_FILE): testmessages.po
	mkdir -p $(XTEST_MO_DIR)
	rm -f $(XTEST_MO_DIR)/$(XTEST_MO_FILE)
	msgfmt -o $(XTEST_MO_DIR)/$(XTEST_MO_FILE).tmp `test -f $(srcdir)/testmessages.po && echo $(srcdir)/testmessages.po && exit 0; echo testmessages.po`
	mv $(XTEST_MO_DIR)/$(XTEST_MO_FILE).tmp $(XTEST_MO_DIR)/$(XTEST_MO_FILE)
