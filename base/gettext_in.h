#ifndef gettext_in_h
#define gettext_in_h

#include <libintl.h>

#include "x/namespace.h"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

#undef LIBCXX_DOMAIN
#define LIBCXX_DOMAIN "libcxx"

#define _TXT(a) dgettext(LIBCXX_DOMAIN, a)

template<typename int_type>
const char *_TXTN(const char *a, const char *b, int_type n)
{
	if (n < 0)
		n= -n;

	if (n > 1000000)
		n=(n % 1000000)+1000000;

	return dngettext(LIBCXX_DOMAIN, a, b, n);
}

#define _(a) _TXT(a)
#define _txt(a) a

#define _txtn(a,b) a, b

#if 0
{
#endif
}
#endif
