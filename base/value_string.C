/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/messages.H"
#include "x/value_string.H"
#include "x/to_string.H"
#include "gettext_in.h"

#include <algorithm>
#include <iterator>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

void value_conversion_error(const std::string &value)
{
	std::ostringstream o;

	o << gettextmsg(libmsg(_txt("invalid value: %1%")), value);

	throw EXCEPTION(o.str());
}

bool value_string<bool>::from_string(const std::string &s,
				    const const_locale &locale)

{
	auto cs=LIBCXX_NAMESPACE::to_string(s, locale);

	std::string falsies(libmsg(_txt("0NFnf")));

	return s.size() == 0 ||
		std::find(falsies.begin(), falsies.end(), s[0]) !=
		falsies.end() ? false:true;
}

std::string value_string<bool>::to_string(bool value, const const_locale &l)

{
	return libmsg(value ? _txt("true"):_txt("false"));
}

#if 0
{
#endif
}
