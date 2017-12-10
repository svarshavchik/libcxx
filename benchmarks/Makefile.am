AM_CXXFLAGS = -g @STDTHREAD_CXXFLAGS@

AM_CPPFLAGS = -I../base

noinst_PROGRAMS=sharedptr refptr sharedsize refsize sharedmempressure refmempressure

sharedptr_SOURCES=sharedptr.C

refptr_SOURCES=refptr.C
refptr_LDADD=../base/libcxx.la
refptr_LDFLAGS=-static

refsize_SOURCES=refsize.C
refsize_LDADD=../base/libcxx.la
refsize_LDFLAGS=-static

sharedsize_SOURCES=sharedsize.C
sharedsize_LDADD=../base/libcxx.la
sharedsize_LDFLAGS=-static

refmempressure_SOURCES=refmempressure.C
refmempressure_LDADD=../base/libcxx.la
refmempressure_LDFLAGS=-static

sharedmempressure_SOURCES=sharedmempressure.C
sharedmempressure_LDADD=../base/libcxx.la
sharedmempressure_LDFLAGS=-static
