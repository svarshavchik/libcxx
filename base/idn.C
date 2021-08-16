/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/idn.H"
#include "x/to_string.H"
#include "x/iconviofilter.H"
#include <idna.h>
#include <courier-unicode.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

std::string idn::to_ascii(const std::string &str, int flags)
{
	std::u32string u32;

	unicode::iconvert::convert(str, unicode::utf_8, u32);

	u32.push_back(0);

	char *p;

	Idna_rc rc=(Idna_rc)
		idna_to_ascii_4z(reinterpret_cast<const uint32_t *>
				 (&u32[0]), &p, flags);

	if (rc)
		throw EXCEPTION(std::string("to_ascii: ") +
				idna_strerror(rc));

	std::string s=p;
	free(p);
	return s;
}

std::string idn::from_ascii(const std::string &str, int flags)
{
	std::u32string u(str.begin(), str.end());

	u.push_back(0);

	uint32_t *output=0;

	Idna_rc rc=(Idna_rc)
		idna_to_unicode_4z4z(reinterpret_cast<const uint32_t *>
				     (&u[0]), &output, flags);

	if (rc)
		throw EXCEPTION(std::string("from_ascii: ") +
				idna_strerror(rc));

	size_t n;
	for (n=0; output[n]; ++n)
		;
	u=std::u32string(output, output+n);
	free(output);

	return unicode::iconvert::convert(u, unicode::utf_8);
}

#if 0
{
#endif
}
