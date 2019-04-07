/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/option_definitionsetpropertyobj.H"
#include "x/option_parserobj.H"
#include "x/property_properties.H"
#include "x/logger.H"
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
definitionSetPropertyObj::definitionSetPropertyObj()
	noexcept
	: definitionbaseObj(0, "set-property",
			    option::list::base::hasvalue,
			    libmsg(_txt("Set application property \"name\" to \"value\"")),
			    libmsg(_txt("\"name=value\"")))
{
}

definitionSetPropertyObj::~definitionSetPropertyObj()
{
}

int definitionSetPropertyObj::set(parserObj &parserArg) const noexcept
{
	return option::parser::base::err_invalidoption;
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::property::definitionSetPropertyObj,
		    setLogger);

int definitionSetPropertyObj::set(parserObj &parserArg,
				  const std::string &valueArg) const noexcept
{
	LOG_FUNC_SCOPE(setLogger);

	try {
		property::load_properties(valueArg + "\n",
					  true, true,
					  property::errhandler::errlog(),
					  locale::base::utf8());
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
