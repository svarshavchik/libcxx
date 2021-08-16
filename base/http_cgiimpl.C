/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/cgiimpl.H"
#include "x/http/form.H"
#include "x/mime/structured_content_header.H"
#include "x/sockaddr.H"
#include "x/fd.H"

#include <cstdlib>
#include <cctype>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstring>

#include <unistd.h>

#if ENVIRON_EXPLICIT_DECLARE
extern "C" char **environ;
#endif

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

#define LIBCXX_TEMPLATE_DECL
#define LIBCXX_TEMPLATE_CLIENT_INPUT_ITER cgiimpl::iterator
#include "x/http/clientimpl_to.H"
#undef LIBCXX_TEMPLATE_CLIENT_INPUT_ITER

#include "x/http/cgiimpl_t.H"
#undef LIBCXX_TEMPLATE_DECL

std::string get_cgi_query_string() LIBCXX_INTERNAL;

cgiimpl::cgiimpl(bool proxymode)
	: method( ({
				method_t m=GET;

				const char *p;

				if ((p=getenv("REQUEST_METHOD")) != NULL)
					m=requestimpl::methodnum(p);

				m;
			})),
	  parameters( ({
				  std::string s;

				  const char *p=getenv("QUERY_STRING");

				  if (p)
					  s=p;

				  form::parameters::create(s);
			  }))
{
	for (size_t i=0; environ[i]; i++)
	{
		std::string hdr;

		if (strncmp(environ[i], "HTTP_", 5) == 0)
		{
			hdr=environ[i]+5;
		} else if (strncmp(environ[i], "CONTENT_TYPE=", 13) == 0 ||
			   strncmp(environ[i], "CONTENT_LENGTH=", 15) == 0)
		{
			hdr=environ[i];
		} else continue;

		size_t p=hdr.find('=');

		if (p == std::string::npos)
			continue;

		std::replace(&hdr[0], &hdr[p], '_', '-');

		hdr[p]=':';

		headers.new_header(hdr);
	}

	{
		auto host_header=headers.find("Host");

		if (host_header != headers.end())
		{
			host=host_header->second.value();
			headers.erase("Host");
		}
		else
		{
			host="localhost";
		}
	}

	const char *p, *q;

	if (method == POST && !proxymode)
	{
		mime::structured_content_header
			ct(headers,
			   mime::structured_content_header::content_type);

		if (ct == mime::structured_content_header
		    ::application_x_www_form_urlencoded)
		{
			parameters->decode_params(form::limited_iter<iterator>
						  (begin(),
						   form::getformmaxsize()),
						  form::limited_iter<iterator>
						  (end(), 0));
			method=GET;
			headers.erase(messageimpl::content_length);
			headers.erase(messageimpl::transfer_encoding);
		}
	}

	if ((p=getenv("SCRIPT_NAME")) != NULL)
		script_name=p;

	if ((p=getenv("PATH_INFO")) != NULL)
		path_info=p;

	scheme=getenv("SSL_PROTOCOL") ? "https":"http";

	if ((p=getenv("REMOTE_ADDR")) != NULL &&
	    (q=getenv("REMOTE_PORT")) != NULL)
	{
		remote_addr=sockaddr::create(strchr(p, ':') ?
						   AF_INET6:AF_INET,
						   p,
						   atoi(q));
	}

	if ((p=getenv("SERVER_ADDR")) != NULL &&
	    (q=getenv("SERVER_PORT")) != NULL)
	{
		server_addr=sockaddr::create(strchr(p, ':') ?
						   AF_INET6:AF_INET,
						   p,
						   atoi(q));
	}
}

cgiimpl::~cgiimpl()
{
}

bool cgiimpl::has_data()
{
	return method == POST || method == PUT;
}

cgiimpl::iterator cgiimpl::begin()
{
	if (!has_data())
		return end();

	return fdinputiter(fd::base::dup(0));
}

cgiimpl::iterator cgiimpl::end()
{
	return fdinputiter();
}

uriimpl cgiimpl::get_URI(int options) const
{
	uriimpl uri;

	std::string new_path;

	if (options & uri_relative)
	{
		std::string s=script_name + path_info;

		size_t n=s.rfind('/');

		if (n != std::string::npos)
			new_path=s.substr(n+1);
	}
	else
	{
		uri.set_scheme(scheme);
		uri.set_authority(uriimpl::authority_t(host));
		new_path=script_name;

		if (options & uri_path)
			new_path += path_info;
	}

	uri.set_path(new_path);

	if (options & uri_query)
		uri.set_query(parameters);

	return uri;
}

void cgiimpl::send_response_header(responseimpl &resp)

{
	resp.erase(LIBCXX_NAMESPACE::http::messageimpl::content_length);
	resp.erase(LIBCXX_NAMESPACE::http::messageimpl::transfer_encoding);

	std::cout << "Status: " << resp.get_status_code() << ' '
		  << resp.get_reason_phrase() << '\n';

	static_cast<const headersbase &>(resp)
		.to_string(std::ostreambuf_iterator<char>(std::cout), "\n");

	std::cout << std::flush;
}

#if 0
{
#endif
}
