/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/vipobjdebug.H"
#include "x/messages.H"
#include "x/property_value.H"
#include "gettext_in.h"
#include <cstdlib>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::vipobjdebug_base);

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

property::value<bool> vipobjdebug_base::abort_prop(LIBCXX_NAMESPACE_STR
						   "::vipobjdebugbase::abort",
						   false);

std::string vipobjdebug_base::deadlock_detected_msg()
{
	return libmsg(_txt("Potential deadlock detected in locking order"));
}

void vipobjdebug_base::deadlock_detected(const std::string &lock1,
					 const std::string &lock2)

{
	LOG_FATAL(deadlock_detected_msg());

	LOG_TRACE(gettextmsg(libmsg(_txt("First lock:\n%1%\n"
					 "\nSecond lock:\n%2%\n")),
			     lock1, lock2));

	if (abort_prop.getValue())
		abort();

	throw EXCEPTION(deadlock_detected_msg());
}

#if 0
{
#endif
}
