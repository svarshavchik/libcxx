/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/localeobj.H"
#include "x/locale.H"
#include "x/tostring.H"
#include "x/singleton.H"
#include "x/exception.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class global_localeObj {

public:
	template<const char *(*init_func)()>
		class singletonObj : public localeObj {
	public:
		singletonObj() LIBCXX_HIDDEN : localeObj(init_func())
		{
		}
		~singletonObj() noexcept LIBCXX_HIDDEN
		{
		}

		static singleton<singletonObj<init_func>> instance LIBCXX_HIDDEN;

		static LIBCXX_NAMESPACE::locale get() LIBCXX_HIDDEN
		{
			localeptr p=instance.get();

			if (p.null()) //! Application shutdown, perhaps
				p=ptr<singletonObj<init_func> >::create();
			return p;
		}
	};

	static const char *utf8_locale() noexcept LIBCXX_HIDDEN
	{
		return "en_US.UTF-8";
	}

	static const char *sysenviron_locale() noexcept LIBCXX_HIDDEN
	{
		return "";
	}

	static const char *glob_locale() noexcept LIBCXX_HIDDEN
	{
		return 0;
	}
};

template<const char *(*init_func)()>
singleton<global_localeObj::singletonObj<init_func>>
global_localeObj::singletonObj<init_func>::instance LIBCXX_HIDDEN ;


locale localeBase::global()
{
	return global_localeObj::singletonObj<global_localeObj::glob_locale>
		::get();
}

locale localeBase::utf8()
{
	return global_localeObj::singletonObj<global_localeObj::utf8_locale>
		::get();
}

locale localeBase::environment()
{
	return global_localeObj::singletonObj<global_localeObj::sysenviron_locale>
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

localeObj::localeObj(const char *localeName)
	: locale(create_locale(localeName)),
	  x(localeName)
{
}

localeObj::localeObj(const std::string &localeName)
	: locale(create_locale(localeName.c_str())),
	  x(localeName)
{
}

localeObj::~localeObj() noexcept
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
#if HAVE_XLOCALE
	std::locale::global(locale);
#else
	setlocale(LC_ALL, x.h.c_str());
#endif
}

std::string localeObj::name() const noexcept
{
#if HAVE_XLOCALE
	return locale.name();
#else
	return x.h;
#endif
}

template std::string fromutf8string(const std::string &,
				    const const_locale &);
template std::string fromutf8string(std::string &&,
				    const const_locale &);
template std::string fromutf8string(const std::string &);
template std::string fromutf8string(std::string &&);

template std::string toutf8string(const std::string &,
				  const const_locale &);
template std::string toutf8string(std::string &&,
				  const const_locale &);
template std::string toutf8string(const std::string &);
template std::string toutf8string(std::string &&);

#if 0
{
#endif
}
