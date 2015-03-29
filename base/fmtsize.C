/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fmtsize.H"
#include "x/locale.H"
#include "x/ctype.H"
#include "gettext_in.h"

#include <sstream>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

static uint64_t sz[] = { 1024,
			 1024 * 1024,
			 1024L * 1024 * 1024,
			 1024LL * 1024 * 1024 * 1024 };

static const char * const sz_n[]={ "Kb",
				   "Mb",
				   "Gb",
				   "Tb" };

std::string fmtsize(uint64_t bytes) noexcept
{
	std::ostringstream o;

	int i;

	for (i=sizeof(sz)/sizeof(sz[0]); --i >= 0; )
	{
		if (bytes >= sz[i])
			break;
	}

	if (i < 0)
	{
		o << bytes << " " << _("bytes");
	}
	else
	{
		o << bytes / sz[i];

		if (bytes < sz[i] * 10)
			o << "." << (bytes % sz[i]) * 10 / sz[i];

		o << " " << sz_n[i];
	}

	return o.str();
}

uint64_t parsesize_int(const std::string &s,
		       const const_locale &localeArg) noexcept
	LIBCXX_INTERNAL;

uint64_t parsesize_int(const std::string &s,
		       const const_locale &localeArg) noexcept
{
	uint64_t n=0, frac=0;

	ctype ct(localeArg);

	std::string::const_iterator b=s.begin(), e=s.end();

	b=ct.scan_not(std::ctype_base::space, b, e);

	while (b != e && *b >= '0' && *b <= '9')
	{
		n=n * 10 + (*b++ - '0');
	}

	if (b != e && *b == '.')
	{
		++b;
		if (b != e && *b >= '0' && *b <= '9')
		{
			frac= *b - '0';

			do
			{
				++b;
			} while (b != e && *b >= '0' && *b <= '9');
		}
	}

	std::string scale=
		ct.toupper(std::string(ct.scan_not(std::ctype_base::space,
						   b, e), e));

	for (int i=sizeof(sz)/sizeof(sz[0]); --i >= 0; )
	{
		if (scale == ct.toupper(sz_n[i]))
		{
			n *= sz[i];
			n += (frac * sz[i]+9) / 10;
			break;
		}
	}

	return n;
}

uint64_t parsesize(const std::string &s,
		   const const_locale &localeArg) noexcept
{
	return parsesize_int(fmtsize(parsesize_int(s, localeArg)),
			     localeArg);
}

memsize::memsize(uint64_t bytesArg,
		 const const_locale &localeArg) noexcept
	: bytes(parsesize(fmtsize(bytesArg), localeArg))
{
}

#if 0
{
#endif
}
