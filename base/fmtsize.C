/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fmtsize.H"
#include "x/locale.H"
#include "gettext_in.h"

#include <sstream>
#include <algorithm>
#include <courier-unicode.h>

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

static uint64_t parsesize_int(const std::string &s,
			      const const_locale &localeArg) noexcept
{
	std::vector<unicode_char> uc;

	unicode::iconvert::convert(s, localeArg->charset(), uc);

	uint64_t n=0, frac=0;

	auto b=uc.begin(), e=uc.end();

	while (b != e && unicode_isspace(*b))
		++b;

	while (b != e && unicode_isdigit(*b))
	{
		n=n * 10 + (*b++ & 0x0F);
	}

	if (b != e && *b == '.')
	{
		++b;
		if (b != e && unicode_isdigit(*b))
		{
			frac= *b & 0x0F;

			do
			{
				++b;
			} while (b != e && unicode_isdigit(*b));
		}
	}

	while (b != e && unicode_isspace(*b))
		++b;

	while (b != e && unicode_isspace(e[-1]))
		--e;

	unicode_char scale=b == e ? 0: unicode_uc(*b);

	for (int i=sizeof(sz)/sizeof(sz[0]); --i >= 0; )
	{
		if (scale == unicode_uc(sz_n[i][0]))
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
