/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/option_valuebaseobj.H"
#include "x/option_mutexobj.H"
#include "x/option_parserfwd.H"
#include "x/option_valueobj.H"
#include <cctype>
#include <algorithm>
#include <sstream>
#include <x/exception.H>

namespace LIBCXX_NAMESPACE {
	namespace option {
#if 0
	};
};
#endif

valuebaseObj::valuebaseObj() noexcept : specified(false)
{
}

valuebaseObj::~valuebaseObj() noexcept
{
}

int valuebaseObj::pubset() noexcept
{
	if (!value_mutex.null() &&
	    value_mutex->value)
		return option::parser::base::err_mutexoption;

	int rc=set();

	if (rc == 0)
	{
		specified=true;
		if (!value_mutex.null())
			value_mutex->value=true;
	}

	return rc;
}

int valuebaseObj::pubset(const std::string &valueArg,
			 const const_locale &localeArg) noexcept
{
	if (!value_mutex.null() &&
	    value_mutex->value)
		return option::parser::base::err_mutexoption;

	int rc=set(valueArg, localeArg);

	if (rc == 0)
	{
		specified=true;
		if (!value_mutex.null())
			value_mutex->value=true;
	}
	return rc;
}

void valuebaseObj::pubreset() noexcept
{
	reset();
	specified=false;
	if (!value_mutex.null())
		value_mutex->value=false;
}

#if 0
{
	{
#endif
	}
}
