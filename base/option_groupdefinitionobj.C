/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/option_groupdefinitionobj.H"
#include "x/option_parser.H"
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <cwctype>
#include <cwchar>

namespace LIBCXX_NAMESPACE {
	namespace option {
#if 0
	};
};
#endif
groupdefinitionObj::groupdefinitionObj(ptr<valuebaseObj> valueArg,
				       char32_t shortnameArg,
				       std::string longnameArg,
				       list groupOptionsArg,
				       std::string descriptionArg,
				       std::string argDescriptionArg) noexcept
	: definitionObj(valueArg, shortnameArg, longnameArg, 0,
			descriptionArg, argDescriptionArg),
	  groupOptions(groupOptionsArg)
{
}

groupdefinitionObj::~groupdefinitionObj()
{
}

int groupdefinitionObj::set(parserObj &parserArg) const
{
	int rc=value->pubset();

	if (rc == 0)
		parserArg.current_options=groupOptions;

	return rc;
}

int groupdefinitionObj::set(parserObj &parserArg,
			    std::string valueArg,
			    const const_locale &localeArg)
	const
{
	return option::parser::base::err_invalidoption; // Values not allowed
}

void groupdefinitionObj::reset()
{
	definitionObj::reset();
	groupOptions->reset();
}

bool groupdefinitionObj::multiple() const noexcept
{
	return false;
}

bool groupdefinitionObj::usage(std::ostream &o,
			       size_t indentlevel,
			       size_t width) const
{
	std::ostringstream tmpO;

	definitionObj::usage(tmpO, indentlevel, width);

	o << tmpO.str() << std::endl;

	groupOptions->usage_internal(o, "", width, indentlevel+2);

	return true;
}

void groupdefinitionObj::help(std::ostream &o,
			      size_t indentlevel,
			      size_t width) const
{
	definitionObj::help(o, indentlevel, width);
	groupOptions->help_internal(o, width, indentlevel+2);
}

#if 0
{
	{
#endif
	}
}
