/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/requestimpl.H"
#include "x/http/responseimpl.H"
#include "x/http/cgiimpl.H"
#include "x/chrcasecmp.H"
#include "x/sysexception.H"
#include "x/base64.H"
#include "gettext_in.h"

#include <cctype>
#include <iterator>
#include <algorithm>

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
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
				     method(GET), httpver(http11)
				     
{
}

requestimpl::requestimpl(const cgiimpl &cgi)
	: messageimpl(cgi.headers), proxyFormat(false),
	  method(cgi.method),
	  uri(cgi.getURI(cgi.uri_query)), httpver(http11)
{
	erase("Host");
}

requestimpl::~requestimpl() noexcept
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
	httpver=httpnone;

	std::string::const_iterator b(start_line.begin()),
		e(start_line.end()), p;

	p=std::find_if(b, e, std::ptr_fun(isspace));

	if ((method=methodnum(std::string(b, p))) == CONNECT)
	    responseimpl::throw_not_implemented();

	b=std::find_if(p, e, std::not1(std::ptr_fun(isspace)));
	p=std::find_if(b, e, std::ptr_fun(isspace));

	uri.parse(b, p);

	b=std::find_if(p, e, std::not1(std::ptr_fun(isspace)));

	b=httpver_parse(b, e, httpver);

	b=std::find_if(b, e, std::not1(std::ptr_fun(isspace)));

	if (b != e)
		bad_message();

	std::pair<const_iterator, const_iterator>
		hosthdr(equal_range("Host"));

	const uriimpl::authority_t &auth=uri.getAuthority();

	if (hosthdr.first != hosthdr.second)
	{
		uriimpl::authority_t newauth=auth;

		newauth.hostport=hosthdr.first->second.value();

		if (auth.hostport.size() == 0)
			uri.setAuthority(newauth);

		if (++hosthdr.first != hosthdr.second)
			bad_message();
	}
	else if (httpver == http11 && auth.hostport.size() == 0)
		bad_message(); // Host header required for HTTP/1.1 messages

	if (uri.getScheme().size() == 0)
		uri.setScheme("http");

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

bool requestimpl::hasMessageBody() const
{
	const_iterator e=end();

	return find(content_length) != e || find(transfer_encoding) != e;
}

bool requestimpl::shouldHaveMessageBody() const
{
	return method == POST || method == PUT;
}

bool requestimpl::responseHasMessageBody(const responseimpl &resp) const
{
	int code=resp.getStatusCode();

	if (method == HEAD || (method == CONNECT && code == 200) ||
	    (code / 100) == 1 || code == 204 || code == 304)
		return false;

	return true;
}

std::string requestimpl::hostheader() const
{
	std::ostringstream o;

	std::pair<std::string, int> hostport(uri.getHostPort());

	o << "Host: " << hostport.first;

	if (hostport.second != uri.getSchemePort())
		o << ":" << hostport.second;

	o << "\r\n";

	return o.str();
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

template std::istreambuf_iterator<char>
requestimpl::parse(std::istreambuf_iterator<char>,
		   std::istreambuf_iterator<char>, size_t);

template std::ostreambuf_iterator<char>
requestimpl::toString(std::ostreambuf_iterator<char>) const;

#if 0
{
	{
#endif
	}
}
