/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/xml/readlock.H"
#include "x/uriimpl.H"
#include "x/messages.H"
#include <courier-unicode.h>
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

readlockObj::readlockObj()=default;

readlockObj::~readlockObj()=default;

void readlockObj::value_conversion_error(const std::string &s)
{
	throw EXCEPTION(gettextmsg(_("Cannot convert \"%1%\""
				     " to the required numeric value type"),
				s));
}

std::u32string readlockObj::get_u32text() const
{
	return unicode::iconvert::tou
		::convert(get_text(), unicode::utf_8).first;
}

std::string readlockObj::get_attribute(const attribute &attr) const
{
	return get_attribute(attr.attrname,
			     attr.attrnamespace);
}


std::u32string readlockObj::get_u32any_attribute(const std::string_view
						 &attr) const
{
	return unicode::iconvert::tou::convert(get_any_attribute(attr),
					       unicode::utf_8).first;
}

std::u32string readlockObj::get_u32attribute(const std::string_view &attribute_name,
					     const explicit_arg<uriimpl> &attribute_namespace) const
{
	return unicode::iconvert::tou::convert(get_attribute(attribute_name,
							     attribute_namespace
							     ),
					       unicode::utf_8).first;
}

std::u32string readlockObj::get_u32attribute(const std::string_view &attribute_name,
					     const std::string_view &attribute_namespace) const
{
	return unicode::iconvert::tou::convert(get_attribute(attribute_name,
							     attribute_namespace
							     ),
					       unicode::utf_8).first;
}

std::u32string readlockObj::get_u32attribute(const attribute &attr) const
{
	return unicode::iconvert::tou::convert(get_attribute(attr),
					       unicode::utf_8).first;
}

std::u32string readlockObj::get_u32attribute(const std::string_view &attr) const
{
	return unicode::iconvert::tou::convert(get_attribute(attr),
					       unicode::utf_8).first;
}

#if 0
{
#endif
}
