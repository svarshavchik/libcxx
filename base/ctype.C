/*
** Copyright 2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ctype.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

ctype::ctype() noexcept
{
}

ctype::ctype(//! The locale used for character conversion functions

	     const const_locale &l)
	: ptr_type( l->get_facet<ptr_type> ())
{
}

ctype::~ctype() noexcept
{
}

std::string ctype::toupper (const char *str) const
{
	return toupper(std::string(str));
}

std::string ctype::toupper(const std::string &str) const
{
	facet_t &f(*ptr_type::operator->());
	globlocale g(f.getLocale());

	std::string strcopy(str);

	f.getFacetConstRef()
		.toupper(&strcopy[0], &strcopy[strcopy.size()]);

	return strcopy;
}

std::string ctype::tolower (const char *str) const
{
	return tolower(std::string(str));
}

std::string ctype::tolower (const std::string &str) const
{
	facet_t &f(*ptr_type::operator->());
	globlocale g(f.getLocale());

	std::string strcopy(str);

	f.getFacetConstRef()
		.tolower(&strcopy[0], &strcopy[strcopy.size()]);

	return strcopy;
}

bool ctype::is(char c, std::ctype_base::mask m) const
{
	facet_t &f(*ptr_type::operator->());
	globlocale g(f.getLocale());

	return f.getFacetConstRef().is(m, c);
}

#if 0
{
#endif
}
