/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/messages.H"
#include "x/weakmultimap.H"
#include "x/singleton.H"
#include "gettext_in.h"
#include <algorithm>
#include <libintl.h>
#include <courier-unicode.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

messages libmsg()
{
	return messages::create(LIBCXX_DOMAIN, locale::base::environment());
}

std::string libmsg(const char *str)
{
	return libmsg()->get(str);
}

class LIBCXX_HIDDEN bound_domainsObj;

class bound_domainsObj : virtual public obj {

 public:
	const std::string domain;
	const std::string directory;

 public:
	bound_domainsObj(const std::string &domain,
			 const std::string_view &directory) noexcept
		: domain{domain},
		  directory{directory}
	{
		bindtextdomain(this->domain.c_str(),
			       this->directory.c_str());
	}

	~bound_domainsObj()=default;
};

class LIBCXX_HIDDEN bound_messagesObj;

class bound_messagesObj : public messagesObj {

public:
	const ref<bound_domainsObj> bound_domain;

	bound_messagesObj(const const_locale &lArg,
			  const std::string_view &domainArg,
			  const ref<bound_domainsObj> &bound_domain) noexcept
		: messagesObj{lArg, domainArg},
		  bound_domain{bound_domain}
	{
	}

	~bound_messagesObj()=default;
};

class LIBCXX_HIDDEN all_bound_domainsObj;


class all_bound_domainsObj : virtual public obj {

public:

	typedef weakmultimap<std::string, bound_domainsObj> all_bounded_t;

	const all_bounded_t all_bounded=all_bounded_t::create();
};

static singleton<all_bound_domainsObj> all_bound_domains_singleton;

messages messagesBase::create(const std::string_view &domain,
			      const constructor_args &args)
{
	auto directory=optional_arg<std::string>(args);

	std::optional<const_locale> default_locale;

	auto l=optional_arg_or<explicit_refptr<const_locale>>
		(args, default_locale,
		 locale::base::global());

	if (!directory)
		return ptrref_base::objfactory<messages>::create(l, domain);

	auto &d=*directory;

	std::string s{domain.begin(), domain.end()};

	auto bounded_domain=
		all_bound_domains_singleton.get()->all_bounded
		->find_or_create(s,
				 [&]
				 {
					 return ref<bound_domainsObj>
						 ::create(s, d);
				 });

	if (bounded_domain->directory != d)
		throw EXCEPTION(gettextmsg(libmsg(_("Domain %1% already bound"
						    " to %2%")),
					   domain,
					   bounded_domain->directory));

	return ref<bound_messagesObj>::create(l, domain,
					      bounded_domain);
}

messagesObj::messagesObj(const const_locale &lArg,
			 const std::string_view &domainArg) noexcept
			: l{lArg}, domain{domainArg}
{
}

messagesObj::~messagesObj()
{
}

std::string messagesObj::call_dngettext(const std::string_view &sArg,
					const std::string_view &pArg,
					unsigned long n) const noexcept
{
	char s[sArg.size()+1];
	char p[pArg.size()+1];

	*std::copy(sArg.begin(), sArg.end(), s)=0;
	*std::copy(pArg.begin(), pArg.end(), p)=0;

	thread_locale thread_locale(*l);

	const char *msg=
		const_cast<const char*>(dngettext(domain.c_str(),
						  s, p, n));

	// msg can be one of the above VLAs, so we must construct a std::string
	// here, in scope.

	return msg;
}

std::string messagesObj::get(const std::string_view &sArg) const noexcept
{
	char s[sArg.size()+1];
	*std::copy(sArg.begin(), sArg.end(), s)=0;

	thread_locale thread_locale(*l);
	return const_cast<const char*>(dgettext(domain.c_str(), s));
}

void gettext_fmt_advance(std::ostream &stream,
			 std::string_view::const_iterator &b,
			 std::string_view::const_iterator &e)
{
	auto start=b;

	while (b != e && *b != '%')
		++b;

	stream.write(&*start, b-start);
}

//! Parse parameter number in a formatting string in gettextmsg()

//! \internal
//!
size_t gettext_fmt_getn(std::string_view::const_iterator &b,
			std::string_view::const_iterator &e) noexcept
{
	size_t n=0;

	while (++b != e && *b >= '0' && *b <= '9')
	{
		n = n * 10 + (*b - '0');
	}

	if (b != e && *b == '%')
		++b;

	return n;
}

std::u32string messagesObj::to_unicode(const std::string &s) const
{
	std::u32string uc;

	unicode::iconvert::convert(s, l->charset(), uc);

	return uc;
}

#if 0
{
#endif
}
