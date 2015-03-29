/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/option_definitionusageobj.H"
#include "x/option_parserobj.H"
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
definitionUsageObj::definitionUsageObj(std::wostream &helpStreamArg) noexcept
	: definitionbaseObj(0, L"usage", 0, L"", L""), helpStream(helpStreamArg)
{
}

definitionUsageObj::~definitionUsageObj() noexcept
{
}

int definitionUsageObj::set(parserObj &parserArg) const noexcept
{
	parserArg.usage(helpStream);
	return option::parser::base::err_builtin;
}

int definitionUsageObj::set(parserObj &parserArg,
			    const std::string &valueArg,
			    const const_locale &localeArg) const noexcept
{
	size_t w=0;

	std::istringstream(valueArg) >> w;

	parserArg.usage(helpStream, w);
	return option::parser::base::err_builtin;
}

bool definitionUsageObj::isSet() const noexcept
{
	return false;
}

void definitionUsageObj::reset() noexcept
{
}

bool definitionUsageObj::usage(std::wostream &o,
			       size_t indentlevel,
			       size_t width)
{
	return false;
}

void definitionUsageObj::help(std::wostream &o,
			      size_t indentlevel,
			      size_t width)
{
}

#if 0
{
	{
#endif
	}
}
