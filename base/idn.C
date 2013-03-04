/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/idn.H"
#include "x/tostring.H"
#include "x/iconviofilter.H"
#include <idna.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

std::string idn::to_ascii(const std::string &str, int flags)
{
	return to_ascii(str, locale::base::environment(), flags);
}

std::string idn::to_ascii(const std::string &str,
			  const const_locale &locale,
			  int flags)
{
	return to_ascii(towstring(str, locale), flags);
}

std::string idn::to_ascii(const std::wstring &str, int flags)
{
	auto u32=iconviofilter::to_u32string(tostring(str,
						      locale::base::utf8()),
					     "UTF-8");

	char *p;

	Idna_rc rc=(Idna_rc)
		idna_to_ascii_4z(reinterpret_cast<const uint32_t *>
				 (u32.c_str()),	&p, flags);

	if (rc)
		throw EXCEPTION(std::string("to_ascii: ") +
				idna_strerror(rc));

	std::string s=p;
	free(p);
	return s;
}

std::string idn::from_ascii(const std::string &str, int flags)
{
	return from_ascii(str, locale::base::environment(), flags);
}

std::string idn::from_ascii(const std::string &str,
			    const const_locale &locale,
			    int flags)
{
	return tostring(from_ascii_tow(str, flags), locale);
}

std::wstring idn::from_ascii_tow(const std::string &str, int flags)
{
	std::u32string u32=iconviofilter::to_u32string(str, "UTF-8");

	uint32_t *output=0;

	Idna_rc rc=(Idna_rc)
		idna_to_unicode_4z4z(reinterpret_cast<const uint32_t *>
				     (u32.c_str()), &output, flags);

	if (rc)
		throw EXCEPTION(std::string("from_ascii: ") +
				idna_strerror(rc));

	u32=reinterpret_cast<const char32_t *>(output);
	free(output);

	return towstring(iconviofilter::from_u32string(u32, "UTF-8"),
			 locale::base::utf8());
}

#if 0
{
#endif
}
