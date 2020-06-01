/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/option_definitionpropertiesobj.H"
#include "x/option_parserobj.H"
#include "x/property_properties.H"
#include "x/to_string.H"
#include "x/messages.H"
#include "gettext_in.h"
#include <string>
#include <iostream>
#include <cwctype>
#include <cwchar>

namespace LIBCXX_NAMESPACE::option {
#if 0
};
#endif
definitionPropertiesObj::definitionPropertiesObj(std::ostream &outputStreamArg)
	noexcept
	: definitionbaseObj(0, "properties", 0,
			    libmsg(_txt("List application properties (\"=all\" - show hidden options)")),
			    ""),
	  outputStream(outputStreamArg)
{
}

definitionPropertiesObj::~definitionPropertiesObj()
{
}

int definitionPropertiesObj::set(parserObj &parserArg) noexcept
{
	return set(parserArg, "");
}

int definitionPropertiesObj::set(parserObj &parserArg,
				 const std::string &valueArg) noexcept
{
	bool all=valueArg == libmsg(_txt("all"));

	auto properties=property::enumerate_properties();

	for (auto &p:properties)
	{
		if (!all && p.first.find('@') != p.first.npos)
			continue;
		outputStream << p.first << "=" << p.second << std::endl;
	}
	return option::parser::base::err_builtin;
}

bool definitionPropertiesObj::is_set() const noexcept
{
	return false;
}

void definitionPropertiesObj::reset() noexcept
{
}

#if 0
{
#endif
}
