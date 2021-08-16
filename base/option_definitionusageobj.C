/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/option_definitionusageobj.H"
#include "x/option_parserobj.H"
#include <string>
#include <iostream>
#include <cwctype>
#include <cwchar>

namespace LIBCXX_NAMESPACE::option {
#if 0
};
#endif
definitionUsageObj::definitionUsageObj(std::ostream &helpStreamArg) noexcept
	: definitionbaseObj(0, "usage", 0, "", ""), helpStream(helpStreamArg)
{
}

definitionUsageObj::~definitionUsageObj()
{
}

int definitionUsageObj::set(parserObj &parserArg) noexcept
{
	parserArg.usage(helpStream);
	return option::parser::base::err_builtin;
}

int definitionUsageObj::set(parserObj &parserArg,
			    const std::string &valueArg) noexcept
{
	size_t w=0;

	std::istringstream(valueArg) >> w;

	parserArg.usage(helpStream, w);
	return option::parser::base::err_builtin;
}

bool definitionUsageObj::is_set() const noexcept
{
	return false;
}

void definitionUsageObj::reset() noexcept
{
}

bool definitionUsageObj::usage(std::ostream &o,
			       size_t indentlevel,
			       size_t width)
{
	return false;
}

void definitionUsageObj::help(std::ostream &o,
			      size_t indentlevel,
			      size_t width)
{
}

#if 0
{
#endif
}