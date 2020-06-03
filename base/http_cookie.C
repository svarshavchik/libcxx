/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/cookie.H"
#include "x/uriimpl.H"
#include "x/chrcasecmp.H"
#include "x/strtok.H"
#include "x/join.H"
#include "x/tzfile.H"
#include "x/locale.H"
#include "x/ymdhms.H"
#include "x/strftime.H"
#include <algorithm>
#include <type_traits>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

cookie::cookie() : expiration((time_t)-1), secure(false), httponly(false)
{
}

cookie::cookie(const std::string &nameArg,
	       const std::string &valueArg) : cookie()
{
	name=nameArg;
	value=valueArg;
}

cookie::~cookie()
{
}

std::string cookie::getValidatedPath(const uriimpl &request_uri) const
{
	if (path.empty())
	{
		std::string path_str=request_uri.getPath();

		auto b=path_str.begin(), e=path_str.end();

		while (b != e)
		{
			if (*--e == '/')
				break;
		}

		path_str=std::string(b, e);

		if (path_str.empty()) path_str="/";
		return path_str;
	}

	if (path.substr(0, 1) != "/")
		return "";

	std::string p=path;

	if (p.size() > 1 && p.substr(p.size()-1) == "/")
		p=p.substr(0, p.size()-1);
	return p;
}

std::string cookie::getValidatedDomain(const uriimpl &request_uri) const
{
	bool domain_given=false;

	std::string domain_str;

	if (domain.empty() &&
	    request_uri.getAuthority())
		// Should always be the case
		domain_str=request_uri.getHostPort().first;
	else
	{
		std::list<std::string> domain_labels;

		strtok_str(domain, ".", domain_labels);

		if (domain_labels.empty())
			return "";

		domain_str="." + join(domain_labels, ".");
		domain_given=true;
	}

	std::transform(domain_str.begin(),
		       domain_str.end(),
		       domain_str.begin(),
		       chrcasecmp::tolower);

	if (!domain_given)
		return domain_str; // Default one

	if (!request_uri.getAuthority())
		// Should not happen.
	{
		return "";
	}

	// Expecting:
	//
	// domain_str=.example.com
	// request_host=www.example.com

	std::string request_host="." + request_uri.getHostPort().first;

	std::transform(request_host.begin(),
		       request_host.end(),
		       request_host.begin(),
		       chrcasecmp::tolower);

	if (domain_str.size() > request_host.size() ||
	    request_host.substr(request_host.size()-domain_str.size())
	    != domain_str)
	{
		return "";
	}

	std::string h=
		request_host.substr(0, request_host.size()-domain_str.size());

	if (!h.empty() && std::find(++h.begin(), h.end(), '.') != h.end())
		return "";

	return domain_str;
}

std::string cookie::expires_as_string() const
{
	auto utc=tzfile::base::utc();

	ymdhms time(ymd::max(), hms(23, 59, 59), utc);

	try {
		time=ymdhms(expiration, utc);
	} catch (...)
	{
	}

	return strftime(time, locale::create("C"))
		("%a, %d %b %Y %H:%M:%S GMT");
}

time_t cookie::expires_from_str(const std::string &value_str)
{
	auto expires=ymdhms(value_str,tzfile::base::utc(),
			    locale::create("C"));

	auto epoch=ymdhms((time_t)0, tzfile::base::utc());

	if ((ymd)expires < (ymd)epoch)
		expires=epoch;

	// If we can't make the
	// conversion, assume overflow
	// and take the epoch end.
	
	time_t expires_given=
		((std::make_unsigned<time_t>::type)-1) >> 1;

	try {
		expires_given=expires;
	} catch (...)
	{
	}

	return expires_given;
}

cookie &cookie::setDomain(const std::string &domain)
{
	std::string prefix;

	if (domain.substr(0, 1) != ".")
		prefix=".";

	this->domain=prefix+domain;
	return *this;
}

cookie &cookie::setPath(const std::string &path)
{
	this->path=path;
	return *this;
}

cookie &cookie::setExpiresIn(time_t expiration)
{
	this->expiration=time(NULL)+expiration;
	return *this;
}

cookie &cookie::setCancel()
{
	this->expiration=0;
	return *this;
}

cookie &cookie::setSecure(bool flag)
{
	secure=flag;
	return *this;
}

cookie &cookie::setHttpOnly(bool flag)
{
	httponly=flag;
	return *this;
}

#if 0
{
#endif
}
