AM_CXXFLAGS = -std=c++0x -g

AM_CPPFLAGS = -I../base

noinst_PROGRAMS=sharedptr refptr sharedsize refsize sharedmempressure refmempressure

sharedptr_SOURCES=sharedptr.C
sharedptr_LDADD=-lpthread

refptr_SOURCES=refptr.C
refptr_LDADD=../base/libx.la
refptr_LDFLAGS=-static

refsize_SOURCES=refsize.C
refsize_LDADD=../base/libx.la
refsize_LDFLAGS=-static

sharedsize_SOURCES=sharedsize.C
sharedsize_LDADD=../base/libx.la
sharedsize_LDFLAGS=-static

refmempressure_SOURCES=refmempressure.C
refmempressure_LDADD=../base/libx.la
refmempressure_LDFLAGS=-static

sharedmempressure_SOURCES=sharedmempressure.C
sharedmempressure_LDADD=../base/libx.la
sharedmempressure_LDFLAGS=-static
