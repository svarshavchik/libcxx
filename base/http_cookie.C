/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/cookie.H"
#include "x/uriimpl.H"
#include "x/chrcasecmp.H"
#include "x/strtok.H"
#include "x/join.H"
#include <algorithm>

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
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

cookie::~cookie() noexcept
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

#if 0
{
	{
#endif
	}
}
