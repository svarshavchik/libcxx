/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "ymdhms.H"
#include "http/responseimpl.H"
#include "http/exception.H"
#include "gettext_in.h"

#include <sstream>
#include <algorithm>
#include <functional>
#include <cctype>

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

const char responseimpl::msg_400[]="Bad Request";
const char responseimpl::msg_401[]="Unauthorized";
const char responseimpl::msg_402[]="Payment Required";
const char responseimpl::msg_403[]="Forbidden";
const char responseimpl::msg_404[]="Not Found";
const char responseimpl::msg_405[]="Method Not Allowed";
const char responseimpl::msg_406[]="Not Acceptable";
const char responseimpl::msg_407[]="Proxy Authentication Required";
const char responseimpl::msg_408[]="Request Timeout";
const char responseimpl::msg_409[]="Conflict";
const char responseimpl::msg_410[]="Gone";
const char responseimpl::msg_411[]="Length Required";
const char responseimpl::msg_412[]="Precondition Failed";
const char responseimpl::msg_413[]="Request Entity Too Large";
const char responseimpl::msg_414[]="Request-URI Too Long";
const char responseimpl::msg_415[]="Unsupported Media Type";
const char responseimpl::msg_416[]="Requested Range Not Satisfiable";
const char responseimpl::msg_417[]="Expectation Failed";
const char responseimpl::msg_500[]="Internal Server Error";
const char responseimpl::msg_501[]="Not Implemented";
const char responseimpl::msg_502[]="Bad Gateway";
const char responseimpl::msg_503[]="Service Unavailable";
const char responseimpl::msg_504[]="Gateway Timeout";
const char responseimpl::msg_505[]="HTTP Version Not Supported";

responseimpl::responseimpl() noexcept : httpver(http11)
{
	setStatusCode(200);
	setReasonPhrase("Ok");
	setCurrentDate();
}

responseimpl::responseimpl(int statuscodeArg,
			   const std::string &reasonphraseArg,
			   httpver_t httpverArg)
	: httpver(httpverArg)
{
	setStatusCode(statuscodeArg);
	setReasonPhrase(reasonphraseArg);
	setCurrentDate();
}

responseimpl::responseimpl(const response_exception &exception)
	: httpver(http11), statuscode(exception.getStatusCode()),
	  reasonphrase(exception.getReasonPhrase())
{
	setCurrentDate();
	append("Content-Type: text/html");
}

void responseimpl::bad_message()
{
	responseimpl::throw_internal_server_error();
}

responseimpl::responseimpl(int statuscodeArg,
			   const char *reasonphraseArg,
			   httpver_t httpverArg)
	: httpver(httpverArg)
{
	setStatusCode(statuscodeArg);
	setReasonPhrase(reasonphraseArg);
	setCurrentDate();
}

responseimpl::~responseimpl() noexcept
{
}

void responseimpl::throwResponseException(int err_code, const char *msg)

{
	throw response_exception(err_code, msg);
}

void responseimpl::throwResponseException(int err_code, const std::string &msg)

{
	throw response_exception(err_code, msg);
}

void responseimpl::setCurrentDate()
{
	replace("Date",
		ymdhms(time(NULL), tzfile::base::utc())
		.toString(locale::create("C"),
			  "%a, %d %b %Y %H:%M:%S GMT")
		);
}

void responseimpl::setNoCache()
{
	append("Cache-Control", "no-cache");
	append("Pragma", "no-cache");
}

void responseimpl::setStatusCode(int statuscodeArg)
{
	if (statuscodeArg < 100 || statuscodeArg > 999)
		throw EXCEPTION(_("Invalid status code"));
	statuscode=statuscodeArg;
}

void responseimpl::setReasonPhrase(const std::string &reasonphraseArg)

{
	for (std::string::const_iterator b(reasonphraseArg.begin()),
		     e(reasonphraseArg.end()); b != e; ++b)
		if (*b == '\r' || *b == '\n' || *b == 0)
			throw EXCEPTION(_("Invalid character in reason phrase")
					);
	reasonphrase=reasonphraseArg;
}

void responseimpl::parse_start_line()
{
	httpver=httpnone;
	statuscode=0;
	reasonphrase.clear();

	std::string::const_iterator b(start_line.begin()),
		e(start_line.end());

	b=httpver_parse(b, e, httpver);

	b=std::find_if(b, e, std::not1(std::ptr_fun(isspace)));

	if (b == e || *b < '0' || *b > '9')
		throw_bad_request();

	while (*b >= '0' && *b <= '9')
	{
		statuscode=statuscode * 10 + (*b++ - '0');

		if (statuscode > 999)
			throw_bad_request();
	}

	if (statuscode < 100)
		throw_bad_request();

	b=std::find_if(b, e, std::not1(std::ptr_fun(isspace)));

	reasonphrase=std::string(b, e);
}

template std::istreambuf_iterator<char>
responseimpl::parse(std::istreambuf_iterator<char>,
		    std::istreambuf_iterator<char>, size_t);

template std::ostreambuf_iterator<char>
responseimpl::toString(std::ostreambuf_iterator<char>) const;

#if 0
{
	{
#endif
	}
}
