/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "option_definitionhelpobj.H"
#include "option_parserobj.H"
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
definitionHelpObj::definitionHelpObj(std::wostream &helpStreamArg) noexcept
	: definitionbaseObj(0, L"help", 0, L"", L""), helpStream(helpStreamArg)
{
}

definitionHelpObj::~definitionHelpObj() noexcept
{
}

int definitionHelpObj::set(parserObj &parserArg) const noexcept
{
	parserArg.help(helpStream);
	return option::parser::base::err_builtin;
}

int definitionHelpObj::set(parserObj &parserArg,
			   const std::string &valueArg,
			   const const_locale &localeArg) const noexcept
{
	size_t w=0;

	std::istringstream(valueArg) >> w;

	parserArg.help(helpStream, w);
	return option::parser::base::err_builtin;
}

bool definitionHelpObj::isSet() const noexcept
{
	return false;
}

bool definitionHelpObj::usage(std::wostream &o,
			      size_t indentlevel,
			      size_t width)
{
	return false;
}

void definitionHelpObj::reset() noexcept
{
}

void definitionHelpObj::help(std::wostream &o,
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
