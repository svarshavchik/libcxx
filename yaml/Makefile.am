#
# Copyright 2012 Double Precision, Inc.
# See COPYING for distribution information.
#

include $(shell pwd)/../includes/libcxx.mk

AM_CPPFLAGS=$(YAMLCPPFLAGS) -I . -I $(srcdir) -I $(srcdir)/../includes -I ../includes

BUILT_SOURCES=x gettext_in.h
CLEANFILES=x gettext_in.h

LINKTYPE=

x:
	ln -s $(srcdir)/../includes x

gettext_in.h:
	ln -s $(srcdir)/../base/gettext_in.h gettext_in.h


lib_LTLIBRARIES=libcxxyaml.la
libcxxyaml_la_LIBADD=../base/libcxx.la $(YAMLLIBS)
libcxxyaml_la_LDFLAGS=-version-info 1 $(LINKTYPE)

noinst_PROGRAMS=testwriter testparser testcookiejar

testwriter_SOURCES=testwriter.C
testwriter_LDADD=../base/libcxx.la libcxxyaml.la
testwriter_LDFLAGS=$(LINKTYPE)

testparser_SOURCES=testparser.C
testparser_LDADD=../base/libcxx.la libcxxyaml.la
testparser_LDFLAGS=$(LINKTYPE)

testcookiejar_SOURCES=testcookiejar.C
testcookiejar_LDADD=../base/libcxx.la libcxxyaml.la
testcookiejar_LDFLAGS=$(LINKTYPE)

check-am: $(noinst_PROGRAMS)
	./testparser
	./testwriter
	./testcookiejar

libcxxyaml_la_SOURCES=\
	documentobj.C                                                   \
	docnode.C                                                       \
	http_cookiejar.C						\
	mappingnode.C							\
	newaliasnode.C							\
	newdocumentnode.C						\
	newnode.C							\
	newscalarnode.C							\
	node.C                                                          \
	scalarnode.C							\
	sequencenode.C							\
	parserobj.C                                                     \
	writer.C							\
	yaml_internal.H

