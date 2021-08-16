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


new_attribute::new_attribute(const std::string &attrnameArg,
			     const value_t &attrvalueArg)
	: new_attribute{attrnameArg, "", attrvalueArg}
{
}

new_attribute::new_attribute(const std::string &attrnameArg,
			     const std::string &attrnamespaceArg,
			     const value_t &attrvalueArg)
	: attribute{attrnameArg, attrnamespaceArg}, attrvalue{
			std::visit(LIBCXX_NAMESPACE::visitor{
					[]
					(const std::string &s)
					{
						return s;
					},
					[]
					(const std::u32string &s)
					{
						return unicode::iconvert
							::fromu::convert
							(s, unicode::utf_8)
							.first;
					},
		}, attrvalueArg)}
{
}

new_attribute::new_attribute(const std::string &attrnameArg,
			     const uriimpl &attrnamespaceArg,
			     const value_t &attrvalueArg)
	: new_attribute{attrnameArg, to_string(attrnamespaceArg), attrvalueArg}
{
}

new_attribute::new_attribute(const std::string &attrnameArg,
			     const char *attrnamespaceArg,
			     const value_t &attrvalueArg)
	: new_attribute{attrnameArg, std::string(attrnamespaceArg),
			attrvalueArg}
{
}

new_attribute::~new_attribute()=default;


#if 0
{
#endif
}
