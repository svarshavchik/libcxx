/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/messages.H"
#include "gettext_in.h"
#include <libintl.h>

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

messagesObj::messagesObj(const const_locale &lArg,
			 const std::string &domainArg)
	: l(lArg), domain(domainArg)
{
}

messagesObj::messagesObj(const const_locale &lArg,
			 const std::string &domainArg,
			 const std::string &directory)
	: messagesObj(lArg, domainArg)
{
	bindtextdomain(domainArg.c_str(), directory.c_str());
}

messagesObj::~messagesObj() noexcept
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

#if 0
{
#endif
}
