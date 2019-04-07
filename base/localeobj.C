/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/localeobj.H"
#include "x/locale.H"
#include "x/tostring.H"
#include "x/singleton.H"
#include "x/exception.H"

#include <courier-unicode.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class LIBCXX_HIDDEN singleton_localeObj {

public:
	template<const char *(*init_func)()>
		class singletonObj : public localeObj {
	public:
		singletonObj() : localeObj(init_func())
		{
		}

		~singletonObj()=default;


		static singleton<singletonObj<init_func>> instance;

		static LIBCXX_NAMESPACE::locale get()
		{
			localeptr p=instance.get();

			if (!p) //! Application shutdown, perhaps
				p=ptr<singletonObj<init_func> >::create();
			return p;
		}
	};

	static const char *utf8_locale() noexcept
	{
		return "en_US.UTF-8";
	}

	static const char *sysenviron_locale() noexcept
	{
		return "";
	}
};

template<const char *(*init_func)()>
singleton<singleton_localeObj::singletonObj<init_func>>
singleton_localeObj::singletonObj<init_func>::instance LIBCXX_HIDDEN ;

class LIBCXX_HIDDEN global_localeObj : virtual public obj {

 public:

	mpobj<const_localeptr> global_locale;
};

singleton<global_localeObj> global_locale_singleton LIBCXX_HIDDEN;

const_locale localeBase::global()
{
	auto p=global_locale_singleton.get();

	mpobj<const_localeptr>::lock lock{p->global_locale};

	if (!*lock)
		*lock=locale::create(nullptr);

	return *lock;
}

const_locale localeBase::utf8()
{
	return singleton_localeObj::singletonObj<singleton_localeObj::utf8_locale>
		::get();
}

const_locale localeBase::environment()
{
	return singleton_localeObj::singletonObj<singleton_localeObj::sysenviron_locale>
		::get();
}

std::locale localeObj::create_locale(const char *explicit_name)

{
#if HAVE_XLOCALE
	try {
		if (!explicit_name)
			return std::locale();

		try {
			return std::locale(explicit_name);
		} catch (const std::exception &e)
		{
			if (*explicit_name)
				throw;

			return std::locale("C");
		}
	} catch(const std::exception &e)
	{
		throw_locale_exception(std::string("Cannot instantiate locale "
						   ) + (explicit_name ?
							explicit_name:
							"<default>"));
		/* NOTREACHED */
	}
#else
	return std::locale();
#endif
}

localeObj::localeObj(const std::string &localeName)
	: localeObj(localeName.c_str())
{
}

void localeObj::deserialized(const std::string &localeName)
{
	locale=create_locale(localeName.c_str());
	x=xlocale(localeName);
	locale_charset=unicode_locale_chset_l(x.h);
}

localeObj::localeObj(const char *localeName)
	: locale(create_locale(localeName)),
	  x(localeName),
	  locale_charset(unicode_locale_chset_l(x.h))
{
}

localeObj::~localeObj()
{
}

void localeObj::throw_unknown_facet()

{
	throw_locale_exception("Unknown facet");
}

void localeObj::throw_locale_exception(const std::string &what)

{
	throw EXCEPTION(what);
}

void localeObj::global() const noexcept
{
	auto p=global_locale_singleton.get();

	mpobj<const_localeptr>::lock lock{p->global_locale};

	std::locale::global(locale);
	*lock=const_localeptr(this);
}

std::string localeObj::tolower(const std::string &text) const
{
	return unicode::tolower(text, charset());
}

std::string localeObj::toupper(const std::string &text) const
{
	return unicode::toupper(text, charset());
}

#if 0
{
#endif
}
