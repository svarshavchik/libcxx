/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/option_definitionpropertiesobj.H"
#include "x/option_parserobj.H"
#include "x/property_properties.H"
#include "x/tostring.H"
#include "x/messages.H"
#include "gettext_in.h"
#include <string>
#include <iostream>
#include <cwctype>
#include <cwchar>

namespace LIBCXX_NAMESPACE {
	namespace option {
#if 0
	};
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

int definitionPropertiesObj::set(parserObj &parserArg) const noexcept
{
	return set(parserArg, "");
}

int definitionPropertiesObj::set(parserObj &parserArg,
				 const std::string &valueArg) const noexcept
{
	bool all=valueArg == libmsg(_txt("all"));

	std::map<std::string, std::string> properties;

	property::enumerate_properties(properties);

	std::map<std::string, std::string>::iterator b, e;

	for (b=properties.begin(), e=properties.end(); b != e; ++b)
	{
		if (!all && b->first.find('@') != b->first.npos)
			continue;
		outputStream << b->first << "=" << b->second << std::endl;
	}
	return option::parser::base::err_builtin;
}

bool definitionPropertiesObj::isSet() const noexcept
{
	return false;
}

void definitionPropertiesObj::reset() noexcept
{
}

#if 0
{
	{
#endif
	}
}
