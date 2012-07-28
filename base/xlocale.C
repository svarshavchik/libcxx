/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "xlocale.H"
#include "locale.H"
#include "exception.H"
#include "globlocale.H"

#include <cstdlib>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

thread_locale::thread_locale(const localeObj &obj)
	: xlocale::thread_locale(obj.x)
{
}

thread_locale::~thread_locale() noexcept
{
}

xlocale::xlocale(const char *localeName)
{
	init(localeName);
}

xlocale::xlocale(const std::string &localeName)
{
	init(localeName.c_str());
}


#if HAVE_XLOCALE
xlocale::xlocale() : h(NULL)
{
}

xlocale::~xlocale() noexcept
{
	if (h)
		freelocale(h);
}

void xlocale::init(const char *n)
{
	if ((h=n ? newlocale(LC_ALL_MASK, n, NULL)
	     : newlocale(0, "C", NULL)) == NULL)
		localeObj::throw_locale_exception(std::string("Cannot instantiate locale "
							      ) + (n ? n
								   : "(global)"
								   ));


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

xlocale::thread_locale::~thread_locale() noexcept
{
	uselocale(oldLocale);
}

#else

xlocale::xlocale()
{
}

xlocale::~xlocale() noexcept
{
}

void xlocale::init(const char *n)
{
	globlock lock;

	const char *prev_str=setlocale(LC_ALL, NULL);

	if (prev_str == NULL)
		localeObj::throw_locale_exception("Cannot determine current locale");

	std::string prev=prev_str;

	if (n && setlocale(LC_ALL, n) == NULL)
	{
		localeObj::throw_locale_exception(std::string("Cannot instantiate locale "
							      )
						  + (*n ? n:"(system)"));
	}

	setlocale(LC_ALL, prev.c_str());
	h = n ? n:prev;
}


xlocale::xlocale(const xlocale &o) : h(o.h)
{
}

xlocale &xlocale::operator=(const xlocale &o)
{
	h=o.h;
	return *this;
}

xlocale::thread_locale::thread_locale(const xlocale &x)
{
	const char *p=setlocale(LC_ALL, NULL);

	if (!p)
		localeObj::throw_locale_exception("Cannot determine current locale");
	oldLocale=p;

	p=getenv("LC_ALL");

	oldenv=p ? p:"";

	if (setlocale(LC_ALL, x.h.c_str()) == NULL)
		localeObj::throw_locale_exception("Cannot instantiate locale "
						  + x.h);
	setenv("LC_ALL", x.h.c_str(), 1);
}

xlocale::thread_locale::~thread_locale() noexcept
{
	if (oldLocale.size() > 0)
		setlocale(LC_ALL, oldLocale.c_str());

	if (oldenv.size())
		setenv("LC_ALL", oldenv.c_str(), 1);
	else
		unsetenv("LC_ALL");
}

globlocale::globlocale(const const_locale &localeArg)
	: thread_locale(*localeArg)
{
}

globlocale::~globlocale() noexcept
{
}

#endif

#if 0
{
#endif
}
