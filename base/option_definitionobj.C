/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/option_list.H"
#include "x/option_definitionobj.H"

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
definitionObj::definitionObj(ptr<valuebaseObj> valueArg,
			     char32_t shortnameArg,
			     const std::string &longnameArg,
			     int flagsArg,
			     const std::string &descriptionArg,
			     const std::string &argDescriptionArg) noexcept
	: definitionbaseObj(shortnameArg, longnameArg, flagsArg,
			    descriptionArg,
			    argDescriptionArg), value(valueArg)
{
}

definitionObj::~definitionObj()
{
}

int definitionObj::set(parserObj &parserArg) const
{
	return value->pubset();
}

int definitionObj::set(parserObj &parserArg,
		       const std::string &valueArg) const
{
	return value->pubset(valueArg, locale::base::utf8());
}

bool definitionObj::isSet() const
{
	return value->isSet();
}

bool definitionObj::multiple() const
{
	return value->multiple();
}

void definitionObj::reset()
{
	value->pubreset();
}

#if 0
{
	{
#endif
	}
}
