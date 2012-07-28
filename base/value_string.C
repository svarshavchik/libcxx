/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "messages.H"
#include "value_string.H"
#include "codecvtiter.H"
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

void value_conversion_error(const std::wstring &value)
{
	std::wostringstream o;

	o << gettextmsg(wlibmsg(_txt("invalid value: %1%")), value);

	throw EXCEPTION(value_string<std::wstring>
			::toString(o.str(), locale::base::global()));
}

void no_custom_class_default()
{
	throw EXCEPTION(_("Undefined default value for a custom class option"));
}


std::wstring value_string<bool>::toWideString(bool v,
					      const const_locale &locale)

{
	return wlibmsg(v ? _txt("true"):_txt("false"));
}

bool value_string<bool>::fromWideString(const std::wstring &s,
					const const_locale &locale)

{
	std::wstring falsies(wlibmsg(_txt("0NFnf")));

	return s.size() == 0 ||
		std::find(falsies.begin(), falsies.end(), s[0]) !=
		falsies.end() ? false:true;
}

bool value_string<bool>::fromString(const std::string &s,
				    const const_locale &locale)

{
	return fromWideString(value_string<std::wstring>::fromString(s, locale),
			      locale);
}

std::string value_string<bool>::toString(bool value, const const_locale &l)

{
	return value_string<std::wstring>::toString(toWideString(value, l),
						    l);
}

std::string value_string<std::wstring>::toString(const std::wstring &v,
						 const const_locale &localeRef)

{
	std::string s;

	typedef std::back_insert_iterator<std::string> ins_iter_t;

	std::copy(v.begin(), v.end(),
		  ocodecvtiter<ins_iter_t>::wtoc_iter_t::create(ins_iter_t(s),
								localeRef))
		.flush();

	return s;
}

std::wstring
value_string<std::wstring>::fromString(const std::string &v,
				       const const_locale &localeRef)

{
	std::wstring s;

	typedef std::back_insert_iterator<std::wstring> ins_iter_t;

	std::copy(v.begin(), v.end(),
		  ocodecvtiter<ins_iter_t>::ctow_iter_t::create(ins_iter_t(s),
								localeRef))
		.flush();

	return s;
}

std::wstring value_string<std::string>::toWideString(const std::string &v,
						     const const_locale &locale)

{
	return value_string<std::wstring>::fromString(v, locale);
}

std::string value_string<std::string>::fromWideString(const std::wstring &s,
						      const const_locale &locale)

{

	return value_string<std::wstring>::toString(s, locale);
}
#if 0
{
#endif
}
