/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "gettext_in.h"
#include "x/exception.H"
#include "x/messages.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

void numerical_overflow(const char *ftype,
			const char *ttype)
{
	std::string f, t;

	obj::demangle(ftype, f);
	obj::demangle(ttype, t);
	throw EXCEPTION(gettextmsg(libmsg(_txt("Numerical overflow during conversion from %1% to %2%")),
				   f, t));
}

void input_numerical_overflow(const char *ttype)
{
	std::string t;

	obj::demangle(ttype, t);
	throw EXCEPTION(gettextmsg(libmsg(_txt("A parsing error or a numerical overflow occured during input conversion to %1%")),
				   t));
}

#if 0
{
#endif
}
