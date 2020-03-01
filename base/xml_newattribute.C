/*
** Copyright 2013-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"

#include "x/xml/new_attribute.H"
#include "x/to_string.H"
#include "x/uriimpl.H"

namespace LIBCXX_NAMESPACE {
	namespace xml {
#if 0
	};
};
#endif


new_attribute::new_attribute(const std::string &attrnameArg,
			     const std::string &attrvalueArg)
	: new_attribute{attrnameArg, "", attrvalueArg}
{
}

new_attribute::new_attribute(const std::string &attrnameArg,
			     const std::string &attrnamespaceArg,
			     const std::string &attrvalueArg)
	: attribute{attrnameArg, attrnamespaceArg}, attrvalue{attrvalueArg}
{
}

new_attribute::new_attribute(const std::string &attrnameArg,
			     const uriimpl &attrnamespaceArg,
			     const std::string &attrvalueArg)
	: new_attribute{attrnameArg, to_string(attrnamespaceArg), attrvalueArg}
{
}

new_attribute::new_attribute(const std::string &attrnameArg,
			     const char *attrnamespaceArg,
			     const std::string &attrvalueArg)
	: new_attribute{attrnameArg, std::string(attrnamespaceArg),
			attrvalueArg}
{
}

new_attribute::~new_attribute()=default;


#if 0
{
	{
#endif
	}
}
