/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"

#include "x/xml/attribute.H"

#include <iostream>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

attribute::attribute(const std::string_view &attrnameArg,
		     const std::string_view &attrnamespaceArg)
	: attrname{attrnameArg}, attrnamespace{attrnamespaceArg}
{
}

attribute::~attribute()=default;

#if 0
{
#endif
}

namespace std {


	size_t hash<LIBCXX_NAMESPACE::xml::attribute>
	::operator()(const LIBCXX_NAMESPACE::xml::attribute &o) const
	{
		hash<string> h;

		return h(o.attrname) + h(o.attrnamespace);
	}
}
