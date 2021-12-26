/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"

#include "x/xml/new_element.H"
#include "x/to_string.H"
#include "x/uriimpl.H"

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

new_element::new_element(const std::string_view &nameArg)
	: new_element{nameArg, "", ""}
{
	prefix_given=false;
}

new_element::new_element(const std::string_view &nameArg,
			 const std::string_view &uriArg)
	: new_element{nameArg, "", uriArg}
{
	prefix_given=false;
}

new_element::new_element(const std::string_view &nameArg,
			 const explicit_arg<uriimpl> &uriArg)
	: new_element{nameArg, "", to_string(uriArg)}
{
	prefix_given=false;
}

new_element::new_element(const std::string_view &nameArg,
			 const std::string_view &prefixArg,
			 const std::string_view &uriArg)
	: name{nameArg}, prefix{prefixArg}, uri{uriArg}, prefix_given{true}
{
}

new_element::~new_element()=default;

#if 0
{
#endif
}
