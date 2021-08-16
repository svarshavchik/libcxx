/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/requestimpl.H"
#include "x/http/responseimpl.H"
#include "x/http/cgiimpl.H"
#include "x/chrcasecmp.H"
#include "x/sysexception.H"
#include "x/base64.H"
#include "x/to_string.H"
#include "gettext_in.h"

#include <cctype>
#include <iterator>
#include <algorithm>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

const requestimpl::method_list_t requestimpl::method_list[]={
	{OPTIONS, "OPTIONS"},
	{GET, "GET"},
	{HEAD, "HEAD"},
	{POST, "POST"},
	{PUT, "PUT"},
	{DELETE, "DELETE"},
	{TRACE, "TRACE"},
	{CONNECT, "CONNECT"},
	{(method_t)-1, NULL}
};

const char requestimpl::www_authorization[] = "Authorization";

const char requestimpl::proxy_authorization[] = "Proxy-Authorization";


requestimpl::requestimpl() noexcept : proxyFormat(false),
				     method(GET), httpver(httpver_t::http11)

{
}

requestimpl::requestimpl(const cgiimpl &cgi)
	: messageimpl(cgi.headers), proxyFormat(false),
	  method(cgi.method),
	  uri(cgi.get_URI(cgi.uri_query)), httpver(httpver_t::http11)
{
	erase("Host");
}

requestimpl::~requestimpl()
{
}

void requestimpl::bad_message()
{
	responseimpl::throw_bad_request();
}

method_t requestimpl::methodnum(const std::string &name)

{
	for (size_t i=0; method_list[i].name; ++i)
		if (strlen(method_list[i].name) == name.size() &&
		    std::equal(name.begin(), name.end(), method_list[i].name,
			       chrcasecmp::char_equal_to()))
			return method_list[i].method;

	responseimpl::throw_not_implemented();
}

bool requestimpl::parse_method_str(std::string::const_iterator p,
				   size_t plen,
				   const char *str) noexcept
{
	size_t l=strlen(str);

	return (l == plen && std::equal(p, p+plen, str,
					chrcasecmp::char_equal_to()));
}

void requestimpl::parse_start_line()
{
	uri=uriimpl();
	httpver=httpver_t::httpnone;

	std::string::const_iterator b(start_line.begin()),
		e(start_line.end()), p;

	p=std::find_if(b, e, std::ptr_fun(isspace));

	if ((method=methodnum(std::string(b, p))) == CONNECT)
	    responseimpl::throw_not_implemented();

	b=std::find_if(p, e, std::not1(std::ptr_fun(isspace)));
	p=std::find_if(b, e, std::ptr_fun(isspace));

	uri.parse(b, p, 0);

	b=std::find_if(p, e, std::not1(std::ptr_fun(isspace)));

	b=httpver_parse(b, e, httpver);

	b=std::find_if(b, e, std::not1(std::ptr_fun(isspace)));

	if (b != e)
		bad_message();

	std::pair<const_iterator, const_iterator>
		hosthdr(equal_range("Host"));

	const uriimpl::authority_t &auth=uri.get_authority();

	if (hosthdr.first != hosthdr.second)
	{
		uriimpl::authority_t newauth=auth;

		newauth.hostport=hosthdr.first->second.value();

		if (auth.hostport.size() == 0)
			uri.set_authority(newauth);

		if (++hosthdr.first != hosthdr.second)
			bad_message();
	}
	else if (httpver == httpver_t::http11 && auth.hostport.size() == 0)
		bad_message(); // Host header required for HTTP/1.1 messages

	if (uri.get_scheme().size() == 0)
		uri.set_scheme("http");

	erase("Host");
}

const char *requestimpl::methodstr(method_t method) noexcept
{
	size_t i;

	for (i=0; method_list[i].name; ++i)
		if (method_list[i].method == method)
			return method_list[i].name;

	return "";
}

bool requestimpl::has_message_body() const
{
	const_iterator e=end();

	return find(content_length) != e || find(transfer_encoding) != e;
}

bool requestimpl::should_have_message_body() const
{
	return method == POST || method == PUT;
}

bool requestimpl::response_has_message_body(const responseimpl &resp) const
{
	int code=resp.get_status_code();

	if (method == HEAD || (method == CONNECT && code == 200) ||
	    (code / 100) == 1 || code == 204 || code == 304)
		return false;

	return true;
}

std::string requestimpl::hostheader() const
{
	std::pair<std::string, int> hostport(uri.get_host_port());

	std::string o="Host: ";

	o += hostport.first;

	if (hostport.second != uri.get_scheme_port())
	{
		o += ":";
		o += LIBCXX_NAMESPACE::to_string(hostport.second,
						 locale::base::c());
	}

	o += "\r\n";

	return o;
}

void requestimpl::tmpfile_error()
{
	throw SYSEXCEPTION("Temporary file");
}

bool requestimpl::is_basic_auth(std::string::const_iterator b,
				std::string::const_iterator e,
				std::string &basic_auth)
{
	auto p=std::find(b, e, ' ');

	if (chrcasecmp::compare(std::string(b, p), "basic"))
		return false; // This is not a basic authentication scheme

	//! base64 tolerates spaces

	basic_auth.clear();

	base64<>::decode(p, e,
			 std::back_insert_iterator<std::string>(basic_auth));

	return true;
}

cookies_t requestimpl::get_cookies() const
{
	cookies_t cookies;

	for (auto header=equal_range("Cookie");
	     header.first != header.second; ++header.first)
	{
		parse_structured_header([]
					(char c)
					{
						return c == ';';
					},
					[this, &cookies]
					(bool dummy,
					 const std::string namevalue)
					{
						cookies.insert(name_and_value
							       (namevalue));
					},
					header.first->second.begin(),
					header.first->second.end());
	}

	return cookies;
}

template std::istreambuf_iterator<char>
requestimpl::parse(std::istreambuf_iterator<char>,
		   std::istreambuf_iterator<char>, size_t);

template std::ostreambuf_iterator<char>
requestimpl::to_string(std::ostreambuf_iterator<char>) const;

#if 0
{
#endif
}
