/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "option_definitionsetpropertyobj.H"
#include "option_parserobj.H"
#include "property_properties.H"
#include "logger.H"
#include "messages.H"
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
definitionSetPropertyObj::definitionSetPropertyObj()
	noexcept
	: definitionbaseObj(0, L"set-property",
			    option::list::base::hasvalue,
			    wlibmsg(_txt("Set application property \"name\" to \"value\"")),
			    wlibmsg(_txt("\"name=value\"")))
{
}

definitionSetPropertyObj::~definitionSetPropertyObj() noexcept
{
}

int definitionSetPropertyObj::set(parserObj &parserArg) const noexcept
{
	return option::parser::base::err_invalidoption;
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::property::definitionSetPropertyObj,
		    setLogger);

int definitionSetPropertyObj::set(parserObj &parserArg,
				  const std::string &valueArg,
				  const const_locale &localeArg) const noexcept
{
	LOG_FUNC_SCOPE(setLogger);

	try {
		property::load_properties(stringize<property::propvalue,
					  std::string>
					  ::tostr(valueArg + "\n", localeArg),
					  true, true,
					  property::errhandler::errlog(),
					  localeArg);
	} catch (exception &e)
	{
		LOG_ERROR(valueArg << ": " << e);
		return option::parser::base::err_invalidoption;
	}

	return option::parser::base::err_ok;
}

bool definitionSetPropertyObj::isSet() const noexcept
{
	return false;
}

void definitionSetPropertyObj::reset() noexcept
{
}

#if 0
{
	{
#endif
	}
}
