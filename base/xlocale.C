/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/xlocale.H"
#include "x/locale.H"
#include "x/exception.H"
#include "x/globlocale.H"

#include <cstdlib>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

thread_locale::thread_locale(const localeObj &obj)
	: xlocale::thread_locale(obj.x)
{
}

thread_locale::~thread_locale()
{
}

xlocale::xlocale(const char *localeName)
	: xlocale(std::string(localeName ? localeName:""))
{
}

xlocale::xlocale(const std::string &localeName)
	: h(newlocale(LC_ALL_MASK, localeName.c_str(), NULL)), n(localeName)
{
	if (h == NULL)
		throw EXCEPTION("Cannot instantiate locale "
				<< (!n.empty() ? n : "(global)"));
}

xlocale::xlocale() : h(NULL)
{
}

xlocale::~xlocale()
{
	if (h)
		freelocale(h);
}

xlocale::xlocale(const xlocale &o)
	: h(duplocale(o.h))
{
}

xlocale &xlocale::operator=(const xlocale &o)
{
	if (h)
		freelocale(h);
	h=NULL;

	if (o.h)
		h=duplocale(o.h);

	return *this;
}

xlocale::thread_locale::thread_locale(const xlocale &x)
	: oldLocale(uselocale(x.h))
{
}

xlocale::thread_locale::~thread_locale()
{
	uselocale(oldLocale);
}

#if HAVE_XLOCALE
#else

globlocale::globlocale(const localeObj &localeArg)
	: thread_locale(localeArg)
{
}

globlocale::~globlocale()
{
}

#endif

#if 0
{
#endif
}
