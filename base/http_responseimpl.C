/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ymdhms.H"
#include "x/http/responseimpl.H"
#include "x/http/exception.H"
#include "x/logger.H"
#include "gettext_in.h"

#include <sstream>
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

//////////////////////////////////////////////////////////////////////////////

const char responseimpl::www_authenticate[]="WWW-Authenticate";
const char responseimpl::proxy_authenticate[]="Proxy-Authenticate";

responseimpl::authschemeparser::authschemeparser(const char *headername,
						 const headersbase &headers)
	: tokenhdrparser(headername, headers)
{
	// Peek ahead to the first word.
	current_char=tokenhdrparser::operator()();
	current_word=value;
}

responseimpl::authschemeparser::~authschemeparser() noexcept
{
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::http::responseimpl, respLogger);

auth responseimpl::authschemeparser::operator()(void)
{
	LOG_FUNC_SCOPE(respLogger);

 again:

	scheme_parameters.clear();
	realm.clear();

	std::string scheme;
	bool first=true;

	while (current_char)
	{
		std::string::iterator b=current_word.begin(),
			e=current_word.end();

		std::string::iterator p;

		if (first)
		{
			first=false;

			// The challenge should start with
			// 'scheme firstparameter=value,'
			//
			// tokenhdrparser stops scanning when it seens a comma,
			// and not a space. So, we find the space ourselves,
			// and extra the scheme's name from it.

			p=std::find(b, e, ' ');

			scheme=std::string(b, p); // Here it is.

			while (p < e && *p == ' ')
				++p;
			b=p;

			// Set up the iterator to take the first scheme
			// parameter.
			p=std::find(b, e, '=');
		}
		else
		{
			// If this is not the first word, well, if there's
			// a space somewhere before the =, this must be the
			// first parameter of the next authentication scheme.
			p=std::find(b, e, '=');

			if (std::find(b, p, ' ') < p)
				break;
		}

		std::string key=std::string(b, p);

		if (p < e) ++p;

		scheme_parameters.insert(std::make_pair(key,
							std::string(p, e)));

		// Move to the next word.
		current_char=tokenhdrparser::operator()();
		current_word=value;
	}

	if (scheme.empty())
		return auth::unknown;

	// Extract the realm parameter.

	auto realm_param=scheme_parameters.equal_range("realm");

	if (realm_param.first != realm_param.second)
		realm=realm_param.first->second;

	scheme_parameters.erase(realm_param.first, realm_param.second);

	auto scheme_val=auth_fromstring(scheme);

	if (scheme_val == auth::unknown)
	{
		LOG_WARNING("Unknown HTTP authentication scheme: \""
			    << scheme << "\"");
		goto again;
	}
	return scheme_val;
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
