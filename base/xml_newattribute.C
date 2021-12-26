/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"

#include "x/xml/new_attribute.H"
#include "x/to_string.H"
#include "x/visitor.H"
#include "x/uriimpl.H"
#include <courier-unicode.h>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif


new_attribute::new_attribute(const std::string_view &attrnameArg,
			     value_t attrvalueArg)
	: new_attribute{attrnameArg, "", std::move(attrvalueArg)}
{
}

new_attribute::new_attribute(const std::string_view &attrnameArg,
			     const std::string_view &attrnamespaceArg,
			     value_t attrvalueArg)
	: attribute{attrnameArg, attrnamespaceArg},
	  attrvalue{std::move(attrvalueArg)}
{
}

new_attribute::new_attribute(const std::string_view &attrnameArg,
			     const explicit_arg<uriimpl> &attrnamespaceArg,
			     value_t attrvalueArg)
	: new_attribute{
			attrnameArg,
			to_string(attrnamespaceArg),
			std::move(attrvalueArg)
		}
{
}

new_attribute::~new_attribute()=default;


#if 0
{
#endif
}
