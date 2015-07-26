/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "gettext_in.h"
#include "x/uriimpl.H"
#include "x/http/form.H"
#include "x/chrcasecmp.H"
#include "x/messages.H"
#include "x/servent.H"
#include "x/ctype.H"
#include <sstream>

#include <arpa/inet.h>
#include <netinet/in.h>
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

const char uriimpl::gen_delims[]=":/?#[]@";

void uriimpl::invalid_url_char()
{
	throw EXCEPTION(_("Invalid character in a URI"));
}

void uriimpl::invalid_authority()
{
	throw EXCEPTION(_("Invalid URI authority format"));
}

uriimpl::authority_t::authority_t() noexcept : has_userinfo(false)
{
}

uriimpl::authority_t::authority_t(const std::string &authorityStr)
	: authority_t(authorityStr, locale::base::environment(), 0)
{
}

uriimpl::authority_t::authority_t(const std::string &authorityStr,
				  const const_locale &localeArg,
				  int flags)
	: has_userinfo(false)
{
	if (parse(authorityStr.begin(), authorityStr.end(), flags)
	    != authorityStr.end())
		invalid_authority();
}

uriimpl::authority_t::~authority_t() noexcept
{
}

std::string uriimpl::authority_t::toString() const
{
	std::ostringstream o;

	toString(std::ostreambuf_iterator<char>(o.rdbuf()));

	return o.str();
}

int uriimpl::authority_t::compare(const authority_t &o) const noexcept
{
	int n=userinfo.compare(o.userinfo);

	if (n == 0)
	{
		if (has_userinfo != o.has_userinfo)
		{
			n=has_userinfo ? 1:-1;
		}
	}

	if (n == 0)
		n=chrcasecmp::compare(hostport, o.hostport);

	return n;
}

void uriimpl::authority_t::validate_port()
{
	size_t p=hostport.rfind(':');

	if (p == std::string::npos)
		return;

	if (++p == hostport.size())
		invalid_authority();

	if (std::find_if(hostport.begin()+p, hostport.end(),
			 []
			 (char c)
			 {
				 return c < '0' || c > '9';
				 // Not a numeric port
			 }) != hostport.end())
		invalid_authority();
}

template std::string::const_iterator
uriimpl::authority_t::parse(std::string::const_iterator,
			    std::string::const_iterator,
			    int);

template std::ostreambuf_iterator<char>
uriimpl::authority_t::emitUserinfo(std::ostreambuf_iterator<char>)
	const;

template std::ostreambuf_iterator<char>
uriimpl::authority_t::toString(std::ostreambuf_iterator<char>)
	const;

template std::ostreambuf_iterator<char>
uriimpl::authority_t::toString(std::ostreambuf_iterator<char>,
			       const const_locale &, int)
	const;



uriimpl::uriimpl() noexcept
{
}

uriimpl::uriimpl(const std::string &urlString,
		 int flags)
{
	parse(urlString.begin(), urlString.end(), flags);
}

uriimpl::uriimpl(const char *urlString) : uriimpl(std::string(urlString))
{
}

uriimpl::~uriimpl() noexcept
{
}

uriimpl &uriimpl::operator+=(const uriimpl &r)
{
	if (r.scheme.size() && r.scheme != scheme)
	{
		// Not strict

		*this=r;
	}
	else
	{
		if (r.authority)
		{
			authority=r.authority;
			path=r.path;
			query=r.query;
		}
		else
		{
			if (r.path.size() == 0)
			{
				if (r.query.size() > 0)
				{
					query=r.query;
				}
			}
			else
			{
				if (*r.path.c_str() == '/')
				{
					path=r.path;
				}
				else
				{
					if (authority > 0 &&
					    path.size() == 0)
					{
						path="/" + r.path;
					}
					else
					{
						size_t p=path.rfind('/');

						if (p == std::string::npos)
							p=0;
						else
							++p;
						path=path.substr(0, p)
							+ r.path;
					}
				}
				query=r.query;
			}
		}
		fragment=r.fragment;
	}

	std::string opath;
	size_t oslash=0;

	for (std::string::iterator b=path.begin(), e=path.end(); b != e;)
	{
		std::string::iterator p=b;

		if (*p == '.')
		{
			++p;
			if (p == e || *p == '/')
			{
				if (p != e) ++p;

				b=p;
				continue;
			}

			if (*p == '.')
			{
				++p;
				if (p == e || *p == '/')
				{
					if (p != e) ++p;
					b=p;
					continue;
				}
			}
		}

		p=b;

		if (*p == '/')
		{
			++p;
			if (p != e && *p == '.')
			{
				++p;
				if (p == e || *p == '/')
				{
					if (p == e) --p;
					*p='/';
					b=p;
					continue;
				}

				if (*p == '.')
				{
					++p;
					if (p == e || *p == '/')
					{
						if (p == e) --p;
						*p='/';
						b=p;

						opath=opath.substr(0, oslash);

						oslash=opath.rfind('/');
						if (oslash == std::string::npos)
							oslash=0;

						continue;
					}
				}
			}
		}

		p=b;

		if (*b == '/')
		{
			++b;
			oslash=opath.size();
		}

		b=std::find(b, e, '/');

		opath += std::string(p, b);
	}

	path=opath;
	return *this;
}

int uriimpl::compare(const uriimpl &o) const noexcept
{
	int n=chrcasecmp::compare(scheme, o.scheme);

	if (n == 0)
		n=authority.compare(o.authority);

	if (n == 0)
		n=path.compare(o.path);

	if (n == 0)
		n=query.compare(o.query);

	if (n == 0)
		n=fragment.compare(o.fragment);

	return n;
}

bool uriimpl::operator<<(const uriimpl &o) const
{
	return ((!scheme.size() || chrcasecmp::compare(scheme, o.scheme) == 0)
		&&
		(!authority || authority.compare(o.authority) == 0)
		&& !authority.has_userinfo
		&& query.size() == 0 && fragment.size() == 0 &&
		(path == o.path ||
		 path + "/" == o.path.substr(0, path.size()+1)));
}

template void uriimpl::parse(std::string::const_iterator,
			     std::string::const_iterator,
			     int);

template std::ostreambuf_iterator<char>
uriimpl::toString(std::ostreambuf_iterator<char>, bool,
		  const const_locale *, int)
	const;

template std::ostreambuf_iterator<char>
uriimpl::toString(std::ostreambuf_iterator<char>, const const_locale &, bool)
	const;

std::string uriimpl::toStringi18n(const const_locale &localeRef, int flags)
	const
{
	std::ostringstream o;

	toString(std::ostreambuf_iterator<char>(o), false, &localeRef, flags);

	return o.str();
}

void uriimpl::setScheme(const std::string &value)
{
	std::string::const_iterator b(value.begin()), e(value.end());

	if (b != e)
	{
		if (!valid_scheme_1st_char(*b))
		{
		bad_scheme:

			throw EXCEPTION(_("Invalid character in a URI scheme"));
		}

		while (++b != e)
			if (!valid_scheme_char(*b))
				goto bad_scheme;
	}
	scheme=value;
}

void uriimpl::setAuthority(const std::string &value)
{
	authority_t newauth(value);

	authority=newauth;
}

void uriimpl::setPath(const std::string &value)
{
	for (std::string::const_iterator b(value.begin()), e(value.end());
	     b != e; ++b)
		switch (*b) {
		case '?':
		case '#':
			invalid_url_char();
		}

	path=value;
}

http::form::parameters uriimpl::getForm() const
{
	return http::form::parameters::create(getQuery());
}

void uriimpl::setQuery(const std::string &value)
{
	for (std::string::const_iterator b(value.begin()), e(value.end());
	     b != e; ++b)
		switch (*b) {
		case '?':
		case '#':
			invalid_url_char();
		}

	query=value;
}

void uriimpl::setQuery(const http::form::const_parameters &value)

{
	setQuery(std::string(value->encode_begin(), value->encode_end()));
}

void uriimpl::setFragment(const std::string &value)
{
	for (std::string::const_iterator b(value.begin()), e(value.end());
	     b != e; ++b)
		switch (*b) {
		case '?':
		case '#':
			invalid_url_char();
		}

	fragment=value;
}

int uriimpl::getSchemePort(const std::string &protocol) const
{
	std::string s=getScheme();

	if (s.size() == 0)
		invalid_authority();

	std::transform(s.begin(), s.end(), s.begin(), chrcasecmp::tolower);

	servent service(s, protocol);

	if (service->s_name == NULL)
		throw EXCEPTION(gettextmsg(libmsg(_txt("%1%: undefined port")),
					   s));
	return ntohs(service->s_port);
}

void uriimpl::fixSchemePort(const std::string &protocol)

{
	std::pair<std::string, int> hostport=getHostPort();

	if (hostport.second == getSchemePort())
		authority.hostport=hostport.first;
}

std::pair<std::string, int> uriimpl::getHostPort(const std::string &protocol)
	const
{
	const std::string &hostport=getAuthority().hostport;

	if (hostport.size() == 0 || getScheme().size() == 0)
		invalid_authority();

	std::string::const_iterator b=hostport.begin(), e=hostport.end(),
		p=e;

	while (1)
	{
		if (p == b)
		{
			p=e;
			break;
		}

		--p;
		if (*p == ']')
		{
			p=e;
			break;
		}

		if (*p == ':')
			break;
	}

	std::pair<std::string, int> retval;

	retval.first=std::string(b, p);

	if (p != e)
	{
		std::istringstream i(std::string(++p, e));

		i >> retval.second;
	}
	else
	{
		retval.second=getSchemePort(protocol);
	}
	return retval;
}

#if 0
{
#endif
}
