/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/exception.H"
#include "x/http/serverimpl.H"
#include "x/http/clientauthimpl.H"
#include "x/xml/escape.H"
#include "x/tokens.H"
#include "x/join.H"
#include "gettext_in.h"
#include <sstream>
#include <cstring>

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

response_exception::response_exception(int statuscodeArg,
				       const std::string &reasonphraseArg,
				       httpver_t httpverArg)
	: responseimpl(statuscodeArg, reasonphraseArg, httpverArg)
{
	(*this) << getStatusCode() << ' ' << getReasonPhrase();

	setCurrentDate();
	append("Content-Type: text/html");
	done();
}

response_exception::~response_exception() noexcept
{
}

std::string response_exception::body() const
{
	std::string reasonstresc=xml::escapestr(getReasonPhrase());

	return "<html><head><title>" + reasonstresc + "</title></head>"
		"<body><h1>" + reasonstresc + "</h1></body></html>\n";
}

void response_exception::addChallenge(std::list<std::string> &wordlist,
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

void response_exception::build_version_set(const char *header,
					   const std::list<responseimpl::
					   challenge_info> &challenges)
{
	std::list<std::string> word_list;

	for (const auto &challenge:challenges)
	{
		addChallenge(word_list, challenge);
	}

	emit(header, word_list);
}

void response_exception::emit(const char *header,
			      const std::list<std::string> &word_list)
{
	append(header, join(word_list, ", "));
}


#if 0
{
	{
#endif
	}
}
