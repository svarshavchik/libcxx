/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/localeobj.H"
#include "x/locale.H"
#include "x/to_string.H"
#include "x/singleton.H"
#include "x/exception.H"
#include "x/ymd.H"
#include "gettext_in.h"

#include <courier-unicode.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

namespace{
#if 0
}
#endif

class singleton_localeObj {

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
			return instance.get();
		}
	};

	static constexpr const char *utf8_locale() noexcept
	{
		return "C.UTF-8";
	}

	static constexpr const char *c_locale() noexcept
	{
		return "C";
	}

	static constexpr const char *sysenviron_locale() noexcept
	{
		return "";
	}
};

template<const char *(*init_func)()>
singleton<singleton_localeObj::singletonObj<init_func>>
singleton_localeObj::singletonObj<init_func>::instance;

class global_localeObj : virtual public obj {

 public:

	mpobj<const_localeptr> global_locale;
};

singleton<global_localeObj> global_locale_singleton;

#if 0
{
#endif
}

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

const_locale localeBase::c()
{
	return singleton_localeObj::singletonObj<singleton_localeObj::c_locale>
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
		throw EXCEPTION("Cannot instantiate locale "
				<< (explicit_name ? explicit_name:"<default>"));
		/* NOTREACHED */
	}
#else
	return std::locale();
#endif
}

localeObj::localeObj(const std::string &localeName)
	: localeObj{localeName.c_str()}
{
}

static bool check_utf8(const std::string &charset)
{
	const char *p=charset.c_str();

	if ((p[0] & ~0x20) == 'U' &&
	    (p[1] & ~0x20) == 'T' &&
	    (p[2] & ~0x20) == 'F')
	{
		p += 3;

		if (*p == '-') ++p;

		return *p == '8';
	}
	return false;
}

void localeObj::deserialized(const std::string &localeName)
{
	locale=create_locale(localeName.c_str());
	x=xlocale(localeName);
	locale_charset=unicode_locale_chset_l(x.h);

	utf8=check_utf8(locale_charset);
}

localeObj::localeObj(const char *localeName)
	: locale{create_locale(localeName)},
	  x{localeName},
	  locale_charset{unicode_locale_chset_l(x.h)},
	  utf8{check_utf8(locale_charset)}
{
}

localeObj::~localeObj()
{
}

void localeObj::throw_unknown_facet()
{
	throw EXCEPTION("Unknown facet");
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

bool localeObj::right_to_left() const
{
	x::ymd d{2021, 1, 1};

	auto s=d.format_date(U"%B\n", const_ref{this});

	return unicode::bidi_get_direction(s).direction != UNICODE_BIDI_LR;
}

std::u32string localeObj::tou32(const std::string &text) const
{
	auto [string, error]=unicode::iconvert::tou::convert(text, charset());

	if (error)
		throw EXCEPTION("Unicode encoding error");

	return string;
}

std::string localeObj::fromu32(const std::u32string &text) const
{
	auto [string, error]=unicode::iconvert::fromu::convert(text, charset());

	if (error)
		throw EXCEPTION("Unicode encoding error");

	return string;
}

namespace {
#if 0
}
#endif

struct converted_string : public unicode::iconvert {

	std::string convstr;

	using unicode::iconvert::operator();

	int converted(const char *p, size_t n) override
	{
		convstr.insert(convstr.end(), p, p+n);
		return 0;
	}
};

#if 0
{
#endif
}

std::string localeObj::toutf8(const std::string_view &text) const
{
	if (utf8)
		return {text.begin(), text.end()}; // Shortcut

	converted_string convert;

	bool errflag=false;

	convert.convstr.reserve(text.size() + text.size()/3);
	if (!convert.begin(charset(), unicode::utf_8)
	    || !convert(text.data(), text.size())
	    || !convert.end(errflag)
	    || errflag)
		throw EXCEPTION(libmsg(_txt("UTF-8 encoding error")));

	return std::move(convert.convstr);
}

std::string localeObj::fromutf8(const std::string_view &text) const
{
	if (utf8)
		return {text.begin(), text.end()}; // Shortcut

	converted_string convert;

	bool errflag=false;

	convert.convstr.reserve(text.size());
	if (!convert.begin(unicode::utf_8, charset())
	    || !convert(text.data(), text.size())
	    || !convert.end(errflag)
	    || errflag)
		throw EXCEPTION("UTF-8 encoding error");

	return std::move(convert.convstr);
}

#if 0
{
#endif
}
