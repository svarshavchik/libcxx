/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/option_definitionhelpobj.H"
#include "x/option_parserobj.H"
#include <string>
#include <iostream>
#include <cwctype>
#include <cwchar>

namespace LIBCXX_NAMESPACE::option {
#if 0
};
#endif
definitionHelpObj::definitionHelpObj(std::ostream &helpStreamArg) noexcept
	: definitionbaseObj(0, "help", 0, "", ""), helpStream(helpStreamArg)
{
}

definitionHelpObj::~definitionHelpObj()
{
}

int definitionHelpObj::set(parserObj &parserArg) noexcept
{
	parserArg.help(helpStream);
	return option::parser::base::err_builtin;
}

int definitionHelpObj::set(parserObj &parserArg,
			   const std::string &valueArg) noexcept
{
	size_t w=0;

	std::istringstream(valueArg) >> w;

	parserArg.help(helpStream, w);
	return option::parser::base::err_builtin;
}

bool definitionHelpObj::is_set() const noexcept
{
	return false;
}

void definitionHelpObj::reset() noexcept
{
}

#if 0
{
#endif
}
