/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ymdhms.H"
#include "x/http/responseimpl.H"
#include "x/http/exception.H"
#include "x/http/cookie.H"
#include "x/logger.H"
#include "x/tokens.H"
#include "x/join.H"
#include "x/tzfile.H"
#include "x/locale.H"
#include "x/uriimpl.H"
#include "x/tokens.H"
#include "x/to_string.H"
#include "x/xml/escape.H"
#include "gettext_in.h"

#include <sstream>
#include <functional>
#include <cctype>
#include <algorithm>
#include <type_traits>
#include <courier-unicode.h>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
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
	return always_quoted ?
		headersbase::quoted_string(value.begin(), value.end())
		: tokenizer<is_http_token>::
		token_or_quoted_word(value.begin(), value.end());
}

responseimpl::responseimpl() noexcept : httpver(httpver_t::http11)
{
	set_status_code(200);
	set_reason_phrase("Ok");
	set_current_date();
}

responseimpl::responseimpl(int statuscodeArg,
			   const std::string &reasonphraseArg,
			   httpver_t httpverArg)
	: httpver(httpverArg)
{
	set_status_code(statuscodeArg);
	set_reason_phrase(reasonphraseArg);
	set_current_date();
}

void responseimpl::bad_message()
{
	throwResponseException(500, "Invalid HTTP response");
}

responseimpl::responseimpl(int statuscodeArg,
			   const char *reasonphraseArg,
			   httpver_t httpverArg)
	: httpver(httpverArg)
{
	set_status_code(statuscodeArg);
	set_reason_phrase(reasonphraseArg);
	set_current_date();
}

responseimpl::~responseimpl()
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


void responseimpl::throw_redirect(const uriimpl &uri, int status_code)
{
	std::string uri_str=LIBCXX_NAMESPACE::to_string(uri);

	std::string msg="Moved";

	response_exception resp(status_code, msg);

	resp.append("Location", uri_str);

	uri_str=xml::escapestr(uri_str, true);

	resp.set_canned_body_str( "<p>Moved to <a href=\""
				  + uri_str
				  + "\">" + uri_str + "</a></p>");
	throw resp;
}

void responseimpl::set_current_date()
{
	replace("Date",
		LIBCXX_NAMESPACE::to_string
		(ymdhms(time(NULL),
			tzfile::base::utc())
		 .format("%a, %d %b %Y %H:%M:%S GMT"),
		 locale::create("C")));
}

ymdhms responseimpl::get_current_date() const
{
	auto date=equal_range("Date");

	try {
		if (date.first != date.second)
			return ymdhms(std::string(date.first->second.begin(),
						  date.first->second.end()),
				      tzfile::base::utc(),
				      locale::create("C"));
	} catch (...)
	{
	}

	// If no header, if there was a parsing error

	return ymdhms();
}

void responseimpl::set_no_cache()
{
	append("Cache-Control", "no-cache");
	append("Pragma", "no-cache");
}

void responseimpl::set_status_code(int statuscodeArg)
{
	if (statuscodeArg < 100 || statuscodeArg > 999)
		throw EXCEPTION(_("Invalid status code"));
	statuscode=statuscodeArg;
}

void responseimpl::set_reason_phrase(const std::string &reasonphraseArg)

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

responseimpl::challenge_info::challenge_info(auth schemeArg,
					     const std::string &realmArg,
					     const scheme_parameters_t
					     &paramsArg)
	: scheme(schemeArg), realm(realmArg), params(paramsArg)
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

void responseimpl::add_auth_challenge(std::list<std::string> &wordlist,
				      const responseimpl::challenge_info
				      &challenge)
{
	wordlist.push_back(auth_tostring(challenge.scheme) + " "
			   + responseimpl::auth_realm.name + "="
			   + responseimpl::auth_realm.quote_value(challenge
								  .realm));

	for (const auto &param:challenge.params)
		wordlist.push_back(param.first + "=" + param.second);
}

void responseimpl::add_auth_set(const char *header,
				const std::list<responseimpl::
				challenge_info> &challenges)
{
	// https://bugzilla.mozilla.org/show_bug.cgi?id=281851#c31

	for (const auto &challenge:challenges)
	{
		std::list<std::string> word_list;

		add_auth_challenge(word_list, challenge);
		save_auth_header(header, word_list);
	}
}

void responseimpl::save_auth_header(const char *header,
				    const std::list<std::string> &word_list)
{
	append(header, join(word_list, ", "));
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::http::getCookies, cookiesLogger);

static const char set_cookie[]="Set-Cookie";

std::list<cookie> responseimpl::get_cookies() const
{
	std::list<cookie> cookies;

	LOG_FUNC_SCOPE(cookiesLogger);

	auto current_date=get_current_date();

	for (auto cookie_headers=equal_range(set_cookie);
	     cookie_headers.first != cookie_headers.second;
	     ++cookie_headers.first)
	{
		std::u32string u;

		if (!unicode::iconvert::convert(cookie_headers.first->second
						.value(),
						unicode::utf_8, u))
		{
			LOG_WARNING("Set-Cookie: header cannot be parsed as a UTF-8 string");
			continue;
		}

		try {
			cookie c;
			bool first=true;
			time_t expires_given=(time_t)-1;
			int max_age=0;

			headersbase::parse_structured_header
				([](char c)
				 {
					 return c == ';';
				 },
				 [&c, &first, &expires_given, &max_age, &logger]
				 (bool ignore, const std::string &word)
				 {
					 auto b=word.begin();
					 auto e=word.end();
					 auto p=std::find(b, e, '=');

					 std::string name_str(b, p);
					 if (p != e)
						 ++p;

					 std::string value_str(p, e);

					 if (first)
					 {
						 first=false;

						 c.name=name_str;
						 c.value=value_str;
						 return;
					 }

					 std::transform(name_str.begin(),
							name_str.end(),
							name_str.begin(),
							chrcasecmp::tolower);

					 if (name_str == "max-age")
					 {
						 std::istringstream
							 i(value_str);

						 i >> max_age;
						 return;
					 }

					 if (name_str == "domain")
					 {
						 c.domain=value_str;
						 return;
					 }

					 if (name_str == "path")
					 {
						 c.path=value_str;
						 return;
					 }

					 if (name_str == "secure")
					 {
						 c.secure=true;
						 return;
					 }

					 if (name_str == "httponly")
					 {
						 c.httponly=true;
						 return;
					 }

					 if (name_str != "expires")
						 return;

					 try {
						 expires_given=cookie
							 ::expires_from_str
							 (value_str);
					 } catch (const exception &e)
					 {
						 LOG_WARNING(e);
						 LOG_TRACE(e->backtrace);
					 }
					 return;
				 }, cookie_headers.first->second.begin(),
				 cookie_headers.first->second.end());

			if (max_age > 0)
			{
				c.expiration=max_age+current_date;
			}
			else if (expires_given != (time_t)-1)
			{
				c.expiration=expires_given;
			}
			cookies.push_back(c);
		} catch (const exception &e)
		{
			LOG_WARNING(e);
			LOG_TRACE(e->backtrace);
		}
	}

	return cookies;
}

void responseimpl::add_cookie(const cookie &c)
{
	std::ostringstream o;

	o << c.name << "="
	  << tokenizer<is_http_token>::token_or_quoted_word(c.value);

	if (c.expiration != (time_t)-1)
		o << "; Expires="
		  << tokenizer<is_http_token>
			::token_or_quoted_word(c.expires_as_string());

	if (!c.domain.empty())
		o << "; Domain=" << c.domain;

	if (!c.path.empty())
		o << "; Path=" << c.path;

	if (c.secure)
		o << "; Secure";

	if (c.httponly)
		o << "; HttpOnly";

	append(set_cookie, o.str());
}

template std::istreambuf_iterator<char>
responseimpl::parse(std::istreambuf_iterator<char>,
		    std::istreambuf_iterator<char>, size_t);

template std::ostreambuf_iterator<char>
responseimpl::to_string(std::ostreambuf_iterator<char>) const;

#if 0
{
#endif
}
