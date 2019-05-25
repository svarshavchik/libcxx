/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/messages.H"
#include "x/weakmultimap.H"
#include "x/singleton.H"
#include "gettext_in.h"
#include <libintl.h>
#include <courier-unicode.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

messages libmsg()
{
	return messages::create(locale::base::environment(), LIBCXX_DOMAIN);
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
	bound_domainsObj(const std::string_view &domain,
			 const std::string_view &directory)
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
			  const ref<bound_domainsObj> &bound_domain)
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

messages messagesBase::create(const const_locale &lArg,
			      const std::string_view &domainArg,
			      const std::string_view &directory)
{
	// TODO: C++20
	std::string s{domainArg.begin(), domainArg.end()};

	auto bounded_domain=
		all_bound_domains_singleton.get()->all_bounded
		->find_or_create(s,
				 [&]
				 {
					 return ref<bound_domainsObj>
						 ::create(s,
							  directory);
				 });

	if (bounded_domain->directory != directory)
		throw EXCEPTION(gettextmsg(libmsg(_("Domain %1% already bound"
						    " to %2%")),
					   domainArg,
					   bounded_domain->directory));

	return ref<bound_messagesObj>::create(lArg, domainArg,
					      bounded_domain);
}

messagesObj::messagesObj(const const_locale &lArg,
			 const std::string_view &domainArg)
	: l{lArg}, domain{domainArg}
{
}

messagesObj::~messagesObj()
{
}

const char *messagesObj::call_dngettext(const char *s,
					const char *p,
					unsigned long n) const
{
	thread_locale thread_locale(*l);

	const char *msg=
		const_cast<const char*>(dngettext(domain.c_str(),
						  s, p, n));
	return msg;
}

std::string messagesObj::get(const char *s) const
{
	thread_locale thread_locale(*l);
	return const_cast<const char*>(dgettext(domain.c_str(), s));
}

std::string messagesObj::get(const std::string &s) const
{
	return get(s.c_str());
}

void gettext_fmt_advance(std::ostream &stream,
			 const char *&str)
{
	const char *start=str;

	while (*str && *str != '%')
		++str;
	stream.write(start, str-start);
}

//! Parse parameter number in a formatting string in gettextmsg()

//! \internal
//!
size_t gettext_fmt_getn(const char *&str)
{
	size_t n=0;

	while (++str, *str >= '0' && *str <= '9')
	{
		n = n * 10 + (*str - '0');
	}

	if (*str == '%')
		++str;
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
