/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "option_list.H"
#include "option_definitionobj.H"

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
			     wchar_t shortnameArg,
			     const std::wstring &longnameArg,
			     int flagsArg,
			     const std::wstring &descriptionArg,
			     const std::wstring &argDescriptionArg) noexcept
	: definitionbaseObj(shortnameArg, longnameArg, flagsArg,
			    descriptionArg,
			    argDescriptionArg), value(valueArg)
{
}

definitionObj::~definitionObj() noexcept
{
}

int definitionObj::set(parserObj &parserArg) const
{
	return value->pubset();
}

int definitionObj::set(parserObj &parserArg,
		       const std::string &valueArg,
		       const const_locale &localeArg) const
{
	return value->pubset(valueArg, localeArg);
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
