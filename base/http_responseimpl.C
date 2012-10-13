/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ymdhms.H"
#include "x/http/responseimpl.H"
#include "x/http/exception.H"
#include "x/logger.H"
#include "x/tokens.H"
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

const responseimpl::auth_param responseimpl::auth_realm={"realm", true};

std::string responseimpl::auth_param::quote_value(const std::string &value) const
{
	std::ostringstream o;

	if (always_quoted)
	{
		headersbase
			::emit_quoted_string(std::ostreambuf_iterator<char>(o),
					     value.begin(), value.end());
	}
	else
	{
		tokenizer<is_http_token>
			::emit_token_or_quoted_word(std::ostreambuf_iterator
						    <char>(o),
						    value.begin(), value.end());
	}

	return o.str();
}

responseimpl::responseimpl() noexcept : httpver(httpver_t::http11)
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
	httpver=httpver_t::httpnone;
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
const char responseimpl::authentication_info[]="Authentication-Info";
const char responseimpl::proxy_authentication_info[]="Proxy-Authentication-Info";

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::http::responseimpl, respLogger);

responseimpl::challenge_info::challenge_info()
	: scheme(auth::unknown)
{
}

void responseimpl::challenge_info::add(std::list<challenge_info> &list)
{
	LOG_FUNC_SCOPE(respLogger);

	if (scheme == auth::unknown)
		return;

	auto realm_iters=params.equal_range("realm");

	if (realm_iters.first == realm_iters.second)
	{
		LOG_WARNING("Authentication scheme realm not found");
		return;
	}

	realm=realm_iters.first->second;
	params.erase(realm_iters.first, realm_iters.second);

	list.push_back(*this);
}

void responseimpl::challenges(std::string::const_iterator b,
			      std::string::const_iterator e,
			      std::list<challenge_info> &list)
{
	LOG_FUNC_SCOPE(respLogger);

	challenge_info current;

	LOG_DEBUG("Processing authentication challenge header");

	parse_structured_header([]
				(char c)
				{
					return c == ',' ||
						c == ' ' ||
						c == '\r' ||
						c == '\n' ||
						c == '\t';
				},
				[&]
				(bool ignore,
				 const std::string &word)
				{
					// RFC 2617 requires = for auth-param
					// If there's no =, this must be
					// an auth-scheme.

					if (word.find('=') != std::string::npos)
					{
						LOG_DEBUG("Parameter: "
							  << word);

						current.params.insert
							(name_and_value(word));
						return;
					}

					current.add(list);

					current=challenge_info();

					if ((current.scheme=
					     auth_fromstring(word))
					    == auth::unknown)
					{
						LOG_WARNING("Unknown HTTP authentication scheme: \""
							    << word << "\"");
					}

				}, b, e);

	current.add(list);
}

bool responseimpl::get_authentication_info(const char *header,
					   scheme_parameters_t &params) const
{
	LOG_FUNC_SCOPE(respLogger);

	params.clear();

	bool found_header=false;
	bool toomany=false;

	process(header,
		[&]
		(std::string::const_iterator b,
		 std::string::const_iterator e)
		{
			if (found_header)
			{
				toomany=true;
				return;
			}
			found_header=true;

			parse_structured_header
				([]
				 (char c)
				 {
					 return c == ',';
				 },
				 [&]
				 (bool dummy, const std::string &word)
				 {
					 params.insert(name_and_value(word));
				 },
				 b, e);
		});

	if (toomany)
	{
		LOG_WARNING("Multiple " << header << " headers received");
		return false;
	}

	return found_header;
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
