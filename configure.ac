#
# Copyright 2012-2013 Double Precision, Inc.
# See COPYING for distribution information.

AC_PREREQ(2.61)
AC_INIT(libcxx, 0.9, [mrsam@courier-mta.com])

AC_CONFIG_SRCDIR([includes/x/obj.H])
AC_CONFIG_HEADER([libcxx_config.h])
AM_INIT_AUTOMAKE([dist-bzip2 foreign -Wno-portability subdir-objects])

# Checks for programs.
AC_PROG_CXX
LT_CONFIG_LTDL_DIR([libltdl])
LT_INIT([dlopen])
LTDL_INIT([recursive])

AC_PROG_LIBTOOL
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.6])
AC_LANG(C++)

XGETTEXT_EXTRA_OPTIONS="--keyword=_txt --keyword=_txtw --keyword=_txtn:1,2 --keyword=_txtnw:1,2 \`cd \$(top_srcdir); echo gnutls/*.C base/*.C yaml/*.C\`"

# Checks for libraries.

AC_ARG_WITH(pkgconfigdir,[AS_HELP_STRING([--with-pkgconfigdir=dir],[Specify where pkgconfig configuration file gets installed])],
	[PKGCONFIGDIR="$withval"],
	[
for d in /usr/lib64/pkgconfig /usr/lib/pkgconfig /usr/local/libdata/pkgconfig
do
	if test -d "$d"
	then
		PKGCONFIGDIR="$d"
		break
	fi
done
])

if test "$PKGCONFIGDIR" = ""
then
	AC_MSG_ERROR([Unable to find pkgconfig installation directory (use --with-pkgconfigdir to set manually)])
fi
AC_SUBST(PKGCONFIGDIR)

gnutlspkg=gnutls

if pkg-config --atleast-version=3.1.0 gnutls3
then
	gnutlspkg=gnutls3
else
	if pkg-config --atleast-version=3.1.0 gnutls
	then
		:
	else
		AC_MSG_ERROR([GnuTLS library version 3.1.0, or higher, required])
	fi
fi

AC_CHECK_PROGS(GNUTLS_CONFIG,libgnutls-config,none,/usr/local/bin:$PATH)

if test "$GNUTLS_CONFIG" = "none"
then
	GNUTLS_CFLAGS="`pkg-config --cflags $gnutlspkg`"
	GNUTLS_LIBS="`pkg-config --libs $gnutlspkg`"
else
	GNUTLS_LIBS=`$GNUTLS_CONFIG --libs`
	GNUTLS_CFLAGS=`$GNUTLS_CONFIG --cflags`
fi


AC_SUBST(GNUTLS_LIBS)
AC_SUBST(GNUTLS_CFLAGS)

AC_CHECK_PROGS(GCRYPT_CONFIG,libgcrypt-config,none)
if test "$GCRYPT_CONFIG" = "none"
then
	AC_MSG_ERROR([libgcrypt-config not found, install the gcrypt library development files])
fi

GCRYPT_LIBS=`$GCRYPT_CONFIG --libs`
GCRYPT_CFLAGS=`$GCRYPT_CONFIG --cflags`

AC_SUBST(GCRYPT_LIBS)
AC_SUBST(GCRYPT_CFLAGS)

save_LIBS="$LIBS"
LIBS="-lcourier-unicode"
AC_LINK_IFELSE(
[AC_LANG_PROGRAM([
#include <courier-unicode.h>
],[
	unicode_default_chset();
])],[:],[AC_MSG_ERROR([The Courier Unicode Library 1.2 appears not to be installed. You may need to install a separate development subpackage, in addition to the main package])])
LIBS="$save_LIBS"

# Also update -std in includes/x/libcxx.m4.in
CXXFLAGS="$CXXFLAGS -std=c++17 -fno-omit-frame-pointer"

AC_CHECK_PROGS(XML2_CONFIG,xml2-config,none)
if test "$XML2_CONFIG" = "none"
then
	AC_MSG_ERROR([xml2-config not found, install libxml2 development files])
fi

LIBXML2_LDADD="`$XML2_CONFIG --libs`"
LIBXML2_CFLAGS="`$XML2_CONFIG --cflags`"
AC_SUBST(LIBXML2_LDADD)
AC_SUBST(LIBXML2_CFLAGS)

# Checks for header files.

AC_CHECK_HEADERS(endian.h sys/endian.h)
# Checks for typedefs, structures, and compiler characteristics.

AC_SYS_LARGEFILE
# Checks for library functions.

AC_CHECK_FUNCS(futimens ppoll ftruncate64 mmap64)

changequote(<,>)

LB='['
RB=']'

changequote([,])


AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <sys/signalfd.h>
#include <sys/socket.h>
int flags=TFD_TIMER_ABSTIME;

static const int n=SCM_CREDENTIALS;

void foo()
{
  epoll_create1(EPOLL_CLOEXEC);
  eventfd(0, EFD_CLOEXEC);
}
])], [
     AC_DEFINE_UNQUOTED(HAVE_LINUXSYSCALLS,1,[Found Linux system calls])
     INCLUDE_SYS_EVENTFD="#include <sys/eventfd.h>"
     INCLUDE_SYS_EPOLL="#include <sys/epoll.h>"
     INCLUDE_SYS_TIMERFD="#include <sys/timerfd.h>"
     INCLUDE_SYS_SIGNALFD="#include <sys/signalfd.h>"
     CREDENTIALS_T="typedef struct ucred credentials_t; //!< For recv_credentials()"
     ],
     [
     AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/socket.h>

char foo[_NSIG];

struct cmsgcred uc;

static const int n=SCM_CREDS;

void has_kqueue()
{
	kqueue();
}
])], [
     AC_DEFINE_UNQUOTED(HAVE_KQUEUE,1,[Found BSD kqueue()])
     INCLUDE_SYS_EVENTFD="#include <stdint.h>"
     INCLUDE_SYS_EPOLL="#include <x/epoll_def.h>"
     INCLUDE_SYS_TIMERFD="#include <x/timerfd_def.h>"
     INCLUDE_SYS_SIGNALFD="#include <signal.h>"

     DEFINE_EVENTFD_T_AS_int64_t="typedef uint64_t eventfd_t;"
     KQUEUE_NONBLOCK="private: bool kqueue_nonblock; public: void nonblock(bool) throw(exception); bool nonblock() throw(exception);"
     TIMERFD_CLOCK="int timerfd_clock; bool timerfd_set; timespec timerfd_value; timespec timerfd_interval;"
     SIGNALFD_FAKESIGINFO="struct signalfd_siginfo { int ssi_signo; };"
     CREDENTIALS_T="typedef struct { pid_t pid; uid_t uid; gid_t gid; } credentials_t;"

     AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <stdint.h>

typedef union {
	void        *ptr;
	int          fd;
	uint32_t     u32;
	uint64_t     u64;
} u1;

typedef union {
	void        *ptr;
	int          fd;
	uint32_t     u32;
} u2;

void foo()
{
	char void_big_enough_for_uint64 $LB (sizeof(u1) == sizeof(u2)) * 2 - 1 $RB;
}
])], [
     INCLUDE_UPOLL_u64="uint64_t u64;"
])

     ]
     ,
     [
     AC_MSG_ERROR([Linux system calls not found - kernel 2.6.27 or later is required])])])

AC_SUBST(INCLUDE_SYS_EVENTFD)
AC_SUBST(INCLUDE_SYS_EPOLL)
AC_SUBST(INCLUDE_SYS_TIMERFD)
AC_SUBST(INCLUDE_SYS_SIGNALFD)
AC_SUBST(DEFINE_EVENTFD_T_AS_int64_t)
AC_SUBST(KQUEUE_NONBLOCK)
AC_SUBST(TIMERFD_CLOCK)
AC_SUBST(INCLUDE_UPOLL_u64)
AC_SUBST(SIGNALFD_FAKESIGINFO)
AC_SUBST(CREDENTIALS_T)

AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <sys/types.h>
#include <sys/unistd.h>
#include <stdio.h>
#include <stdlib.h>

void foo(off64_t n)
{
	lseek64(0, n, 0);
}
])], [AC_DEFINE_UNQUOTED(HAVE_LSEEK64,1,[Found lseek64() system call])
     LIBCXX_PREAD64='pread64'
     LIBCXX_PWRITE64='pwrite64'],

     [AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/param.h>
#include <stdint.h>

void foo(off_t n)
{
	char dummy $LB sizeof(n) == sizeof(int64_t) ? 1:-1 $RB;

	lseek(0, n, 0);
}

])], [AC_DEFINE_UNQUOTED(HAVE_LSEEK_OFFT64,1,[lseek() takes a 64 bit off_t])
     LIBCXX_SET_OFFT64="typedef off_t off64_t;"
     LIBCXX_PREAD64='pread'
     LIBCXX_PWRITE64='pwrite'
]
     ,
     AC_MSG_ERROR([Cannot figure out how to do 64 bit seeks]))])

AC_SUBST(LIBCXX_SET_OFFT64)
AC_SUBST(LIBCXX_PREAD64)
AC_SUBST(LIBCXX_PWRITE64)

AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <sys/xattr.h>
])],[
	AC_DEFINE_UNQUOTED(HAVE_XATTR,1,[Have Linux xattr.h API])
	LIBCXX_SET_XATTR='#include <sys/xattr.h>'
],[
	AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <sys/param.h>
#include <sys/extattr.h>

int ns $LB $RB={EXTATTR_NAMESPACE_USER, EXTATTR_NAMESPACE_SYSTEM};

void has_extattr()
{
	extattr_list_file("", EXTATTR_NAMESPACE_USER, NULL, 0);
}
])],[
	AC_DEFINE_UNQUOTED(HAVE_EXTATTR,1,[Have BSD extattr.h API])

	LIBCXX_SET_XATTR='#include <x/xattr_def.H>'

])])

AC_SUBST(LIBCXX_SET_XATTR)

AC_ARG_ENABLE(xlocale, [AS_HELP_STRING([--disable-xlocale],[do not use xlocale, not implemented in libstdc++])], [ xlocale="$enableval" ], [

AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <locale.h>

void foo(locale_t l)
{
	uselocale(l);
}

])], [xlocale='yes'], [xlocale='no'])])

if test "$xlocale" = "yes"
then
     AC_DEFINE_UNQUOTED(HAVE_XLOCALE,1,[locale_t and uselocale() exists])
     HAVE_XLOCALE=1
else
     HAVE_XLOCALE=0
     LIBCXX_LOCALE_T_GLOBLOCK=' : private globlock'
fi

AC_SUBST(LIBCXX_LOCALE_T)
AC_SUBST(LIBCXX_LOCALE_SAVE_ENV_T)
AC_SUBST(LIBCXX_LOCALE_T_GLOBLOCK)
AC_SUBST(HAVE_XLOCALE)

AC_TRY_LINK([
#include <execinfo.h>
#include <stdio.h>

],[
	backtrace(NULL, 0);

	char **p=backtrace_symbols( NULL, 0);
], [ AC_DEFINE_UNQUOTED(HAVE_BACKTRACE, 1, [Found glibc backtrace()]) ],
[

save_LIBS="$LIBS"
LIBS="$LIBS -lunwind"

	AC_TRY_LINK([
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <dlfcn.h>
], [
	unw_cursor_t cursor; unw_context_t uc;
	unw_word_t ip;
	Dl_info info;

	unw_getcontext(&uc);
	unw_init_local(&cursor, &uc);
	unw_step(&cursor);
	unw_get_reg(&cursor, UNW_REG_IP, &ip);
	dladdr((void *)ip, &info);
], [
   AC_DEFINE_UNQUOTED(HAVE_LIBUNWIND,1,[ Found libunwind ])
   LIBUNWIND="-lunwind"
], [AC_MSG_ERROR([Cannot find backtrace() or libunwind])]
)

LIBS="$save_LIBS"

])
AC_SUBST(LIBUNWIND)

AC_TRY_LINK([
#include <unistd.h>

], [
char **p=environ;
], [ : ],
   AC_TRY_LINK([

extern "C" char **environ;

], [
char **p=environ;
] , [
	AC_DEFINE_UNQUOTED(ENVIRON_EXPLICIT_DECLARE, 1, [ Whether "extern char **environ" is not defined in unistd.h ])
	],
  [
  AC_MSG_ERROR([Unable to find extern char **environ])
]))

save_LIBS="$LIBS"
LIBS="$LIBS $LIBCONV"

AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <iconv.h>

size_t foo(iconv_t cd, const char **inbuf, size_t *inbytesleft,
     		  char **outbuf, size_t *outbytesleft)
{
	return iconv(cd, inbuf, inbytesleft, outbuf, outbytesleft);
}
])], [ iconv_in_type='const char **' ],

[
AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <iconv.h>

size_t foo(iconv_t cd, char **inbuf, size_t *inbytesleft,
     		  char **outbuf, size_t *outbytesleft)
{
	return iconv(cd, inbuf, inbytesleft, outbuf, outbytesleft);
}])], [ iconv_in_type='char **' ],

[ AC_MSG_ERROR([Unable to find a working iconv()]) ])
])

LIBS="$save_LIBS"
AC_DEFINE_UNQUOTED(ICONV_INBUF_ARG,$iconv_in_type,[type of third argument to iconv()])

LIBS="$save_LIBS -lidn"

AC_TRY_LINK([
#include <idna.h>
],[
	const uint32_t *input=0;
	uint32_t *output;
	char *coutput;

	idna_to_unicode_4z4z(input, &output, 0);
	idna_to_ascii_4z(input, &coutput, 0);
	return 0;
],[LIBIDN=-lidn],AC_MSG_ERROR([Could not find or link with libidn]))

AC_SUBST(LIBIDN)
LIBS="$save_LIBS"

save_LIBS="$LIBS"
AC_TRY_LINK([
#include <libintl.h>

const char *foo(const char *domain, const char *msgid)
{
	return dgettext(domain, msgid);
}
],[
], [ : ], [
   LINKLIBINTL="-lintl"
   LIBS="$LINKLIBINTL $LIBS"

   AC_TRY_LINK([
#include <libintl.h>

   const char *foo(const char *domain, const char *msgid)
   {
	return dgettext(domain, msgid);
   }
   ],[
   ], [ : ], [
   LIBLINKINTLPATH="-L/usr/local/lib -R/usr/local/lib"
   LIBS="$LIBLINKINTLPATH $LIBS"
      AC_TRY_LINK([
#include <libintl.h>

      const char *foo(const char *domain, const char *msgid)
      {
	return dgettext(domain, msgid);
      }
      ],[
	LIBEXTRAPATH="LIBLINKINTLPATH $LIBEXTRAPATH"
	], [
	AC_MSG_ERROR([Unable to find a working libintl.h])
      ])
   ])
])

LIBS="$save_LIBS"

AC_SUBST(LINKLIBINTL)
AC_SUBST(EXTRACXXLIBS)

AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

pid_t gettid() throw()
{
	return syscall(SYS_gettid);
}
])], [ TID_T_DEF='typedef pid_t tid_t;' ], [
AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <sys/types.h>
#include <sys/thr.h>

long n()
{
	long y;
	thr_self(&y);
	return y;
}
])], [ AC_DEFINE_UNQUOTED(HAVE_THR_SELF,1,[Whether thr_self() is defined])
     TID_T_DEF='typedef long tid_t;'
], [
   AC_MSG_ERROR([Cannot find a working gettid()])
])
])

AC_SUBST(TID_T_DEF)

AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <chrono>

int main()
{
 typedef std::chrono::steady_clock clock_t;
}])],[
sys_clock_t=std::chrono::steady_clock
],[
AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <chrono>

int main()
{
 typedef std::chrono::monotonic_clock clock_t;
}])],[
sys_clock_t=std::chrono::monotonic_clock
],[
AC_MSG_ERROR([Cannot determine system clock type])
])
])

AC_SUBST(sys_clock_t)

AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>

#ifdef ifr_ifindex
#ifdef SIOCGIFHWADDR
#define found
#endif
#endif

#ifndef found
#error not found
#endif
])], [ : ],
AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/socket.h>
#include <net/if_dl.h>

#ifdef ifr_index
#ifdef LLADDR
struct sockaddr_dl sdl;
#define found
#endif
#endif

#ifndef found
#error not found
#endif
])], [ : ], [ AC_MSG_ERROR([Unable to find a compatible getifaddrs() call]) ]))

if test -e /proc/self/exe
then
	SELF=self
	EXE=/exe
	PROC_START=/stat
	PROC_START_FIELD=22
else
AC_COMPILE_IFELSE([AC_LANG_SOURCE([
extern "C" {
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/param.h>
#include <sys/user.h>
#include <sys/stat.h>
#include <kvm.h>
};

int symbols$LB $RB = {
CTL_KERN, KERN_PROC, KERN_PROC_FILEDESC, KERN_PROC_ARGS
};

struct kinfo_file kf;
])], [
     AC_DEFINE_UNQUOTED(HAVE_SYSCTL_KERN_PROC,1,[Found sysctl interface to kern.proc])
], [
   AC_MSG_ERROR([Unable to find a compatible way to get my executable pathname])
])
fi

AC_TRY_LINK( [
#include <stdint.h>

int64_t n;

],[
	__sync_fetch_and_add(&n, 0);
],[ HAS_INT64_SYNC_FETCH_AND_ADD=1 ], [ HAS_INT64_SYNC_FETCH_AND_ADD=0 ])

AC_SUBST(HAS_INT64_SYNC_FETCH_AND_ADD)

LIBEXTRAPATH="`echo $LIBEXTRAPATH | tr ' ' '\n' | sort | uniq | tr '\n' ' '`"
AC_SUBST(LIBEXTRAPATH)
CXXFLAGS="$CXXFLAGS -fvisibility-inlines-hidden"
export CXXFLAGS
LDFLAGS="$LDFLAGS -z relro"
export LDFLAGS

for f in /etc/pki/tls/cert.pem /etc/ssl/certs/ca-certificates.crt /usr/local/share/certs/ca-root-nss.crt
do
	if test -f "$f"
	then
		cacerts="$f"
	fi
done

AC_ARG_WITH(cacerts,
	[AS_HELP_STRING([--with-cacerts=file],[Specify default TLS certificate authority file])],
		     [cacerts="$withval"])
if test "$cacerts" = ""
then
	AC_MSG_ERROR(--cacerts must be specified)
fi

if test ! -f "$cacerts"
then
	AC_MSG_WARN($cacerts does not exist)
fi

AC_DEFINE_UNQUOTED(LIBCXX_CACERTS,"$cacerts",[Default trusted CA certificate file])

mimetypes="/etc/mime.types"
AC_ARG_WITH(mime-types,
	[AS_HELP_STRING([--with-mime-types=file],[Override default location of /etc/mime.types])], [mimetypes="$withval"])
AC_DEFINE_UNQUOTED(MIMETYPES,"$mimetypes",[Location of mime.types file])

save_CPPFLAGS="$CPPFLAGS"
save_LIBS="$LIBS"
if test "$YAMLLIBS" = ""
then
	YAMLLIBS="-lyaml"
fi

CPPFLAGS="$YAMLCPPFLAGS $CPPFLAGS"
LIBS="$YAMLLIBS $LIBS"

AC_TRY_LINK([
#include <yaml.h>
], [
   yaml_get_version(0, 0, 0);
], [ : ], AC_MSG_ERROR([Test link with LibYAML failed. Please install the LibYAML library and its development package. See config.log for more details about this error.]))

CPPFLAGS="$save_CPPFLAGS"
LIBS="$save_LIBS"

AC_SUBST(YAMLLIBS)
AC_SUBST(YAMLCPPFLAGS)

AC_SUBST(LIBCXX_NAMESPACE,x)

VERSION_INFO=10:0:0
AC_SUBST(VERSION_INFO)

test "x$prefix" = xNONE && prefix=$ac_default_prefix
test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
eval "prefix=\"$prefix\""
eval "exec_prefix=\"$exec_prefix\""
eval "sbindir=\"$sbindir\""
eval "httportmapd=\"$sbindir/httportmapd\""

AC_SUBST(httportmapd)

AM_CONDITIONAL(HAVE_REPO, test -d ${srcdir}/docbook)

AC_CONFIG_FILES([Doxyfile po/Makefile.in
		 Makefile
		 libltdl/Makefile
		 includes/x/Makefile
		 includes/x/http/Makefile
		 includes/x/ftp/Makefile
		 includes/x/gcrypt/Makefile
		 includes/x/gnutls/Makefile
		 includes/x/mime/Makefile
		 includes/x/param/Makefile
		 includes/x/threads/Makefile
		 includes/x/xml/Makefile
		 includes/x/yaml/Makefile
		 includes/x/epoll_def.h
		 includes/x/namespace.h
		 includes/x/libcxx.mk
		 includes/x/libcxx.m4
		 includes/x/optgen.xsl
		 includes/x/globlocale.H
		 includes/x/signal_def.h
		 includes/x/sys/Makefile
		 includes/x/sys/clockt.h
		 includes/x/sys/credentialssys.h
		 includes/x/sys/epollsys.h
		 includes/x/sys/eventfdsys.h
		 includes/x/sys/eventfdt.h
		 includes/x/sys/kqueuenb.h
		 includes/x/sys/offt.h
		 includes/x/sys/prw64.h
		 includes/x/sys/signalfdsys.h
		 includes/x/sys/tidtsys.h
		 includes/x/sys/timerfdsys.h
		 includes/x/sys/timerfdpriv.h
		 includes/x/sys/valueimpl64sys.h
		 includes/x/sys/xattrsys.h
		 includes/x/xlocale.H
		 includes/x/msgdispatcher.xsl
		 includes/x/threadmsgdispatcher.xsl
		 includes/x/refcollection.xsl
                 base/Makefile
		 gnutls/Makefile
		 gnutls/tlsparamsgen.sh
		 yaml/Makefile
		 cxxltdl/Makefile
		 benchmarks/Makefile
		 httportmap/httportmap
		 httportmap/Makefile
		 httportmap/testsuite
		 httportmap/apache.conf
		 packaging/fedora/libcxx.spec
		 packaging/fedora/sysinit
		 packaging/fedora/Makefile
		 packaging/freebsd10/devel/libcxx/Makefile
		 ])
AC_OUTPUT
