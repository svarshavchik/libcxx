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

new_element::new_element(const std::string &nameArg)
	: new_element{nameArg, "", ""}
{
	prefix_given=false;
}

new_element::new_element(const std::string &nameArg,
			 const std::string &uriArg)
	: new_element{nameArg, "", uriArg}
{
	prefix_given=false;
}

new_element::new_element(const std::string &nameArg,
			 const char *uriArg)
	: new_element{nameArg, "", uriArg}
{
	prefix_given=false;
}

new_element::new_element(const std::string &nameArg,
			 const uriimpl &uriArg)
	: new_element{nameArg, "", to_string(uriArg)}
{
	prefix_given=false;
}

new_element::new_element(const std::string &nameArg,
			 const std::string &prefixArg,
			 const std::string &uriArg)
	: name{nameArg}, prefix{prefixArg}, uri{uriArg}, prefix_given{true}
{
}

new_element::~new_element()=default;

#if 0
{
#endif
}
