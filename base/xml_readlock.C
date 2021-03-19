/*
** Copyright 2013-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/xml/readlock.H"
#include <courier-unicode.h>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

readlockObj::readlockObj()=default;

readlockObj::~readlockObj()=default;

std::u32string readlockObj::get_u32text() const
{
	return unicode::iconvert::tou
		::convert(get_text(), unicode::utf_8).first;
}

#if 0
{
#endif
}
