/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_exception_H
#define x_http_exception_H

#include <exception>
#include <x/http/responseimpl.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

//! A thrown %exception that can formatted as an HTTP message.

//! Instantiate a response_exception and throw it.
//! When a response_exception gets caught, use it as a responseimpl, also
//! invoke body() to get a short, brief body to use for the response.

class response_exception : public exception, public responseimpl {

	//! No more challanges

	void buildChallenges(std::list<std::string> &word_list)
	{
	}

	//! Start collecting the next challenge from the variadic parameters.

	template<typename ...Args>
	void buildChallenges(std::list<std::string> &word_list,
			     auth next_scheme,
			     const std::string &realm,
			     Args && ...args)
	{
		responseimpl::challenge_info challenge;

		challenge.scheme=next_scheme;
		challenge.realm=realm;

		collectChallenge(word_list, challenge,
				 std::forward<Args>(args)...);
	}

	//! The next challenge is already prepared for us.

	template<typename ...Args>
	void buildChallenges(std::list<std::string> &word_list,
			     const responseimpl::challenge_info &nextChallenge,
			     Args && ...args)
	{
		addChallenge(word_list, nextChallenge);
		buildChallenges(word_list, std::forward<Args>(args)...);
	}

	//! Another parameter for the current challenge.

	template<typename ...Args>
	void collectChallenge(std::list<std::string> &word_list,
			      responseimpl::challenge_info &challenge,
			      const responseimpl::auth_param &param,
			      const std::string &value,
			      Args && ...args)
	{
		challenge.params
			.insert(std::make_pair(param.name, param.quote_value(value)));

		collectChallenge(word_list, challenge,
				 std::forward<Args>(args)...);
	}

	//! End of current challenge, start of next one.

	template<typename ...Args>
	void collectChallenge(std::list<std::string> &word_list,
			      responseimpl::challenge_info &challenge,
			      auth nextArg,
			      Args && ...args)
	{
		addChallenge(word_list, challenge);
		buildChallenges(word_list, nextArg,
				 std::forward<Args>(args)...);
	}

	//! End of current challenge, start of next one.

	template<typename ...Args>
	void collectChallenge(std::list<std::string> &word_list,
			      responseimpl::challenge_info &challenge,
			      const responseimpl::challenge_info &nextArg,
			      Args && ...args)
	{
		addChallenge(word_list, challenge);
		buildChallenges(word_list, nextArg,
				 std::forward<Args>(args)...);
	}

	//! Last challenge

	void collectChallenge(std::list<std::string> &word_list,
			      responseimpl::challenge_info &challenge)
	{
		addChallenge(word_list, challenge);
	}

	//! Save a complete challenge, collected from variadic parameters.
	void addChallenge(std::list<std::string> &word_list,
			  const responseimpl::challenge_info &challenge);
public:

	using exception::operator->;
	using exception::operator*;

	friend class responseimpl;

	//! Constructor
	response_exception(int statuscodeArg,
			   const std::string &reasonphraseArg,
			   httpver_t httpverArg=httpver_t::http11);

	//! Destructor
	~response_exception() noexcept;

	//! Return the HTML body for this exception

	std::string body() const;

private:

	//! Finish constructing the rest of the object, used by responseimpl

	//! \internal
	template<typename ...Args>
	void build(const char *header,
		   httpver_t httpverArg,
		   Args && ...args)
	{
		setVersion(httpverArg);
		build_version_set(header, std::forward<Args>(args)...);
	}

	//! Finish constructing the rest of the object, used by responseimpl

	//! \internal

	template<typename ...Args>
	void build(const char *header,
		   auth first_scheme,
		   Args && ...args)
	{
		build_version_set(header, first_scheme,
				  std::forward<Args>(args)...);
	}

	//! Finish constructing the rest of the object, used by responseimpl

	//! \internal

	void build(const char *header,
		   const std::list<responseimpl::challenge_info> &challenges)
	{
		build_version_set(header, challenges);
	}

	//! Finish constructing the rest of the object, used by responseimpl

	//! \internal

	template<typename ...Args>
	void build_version_set(const char *header,
			       auth firstArg,
			       Args && ...args)
	{
		std::list<std::string> word_list;

		buildChallenges(word_list, firstArg,
				std::forward<Args>(args)...);
		emit(header, word_list);
	}

	//! Finish constructing the rest of the object, used by responseimpl

	//! \internal

	template<typename ...Args>
	void build_version_set(const char *header,
			       const responseimpl::challenge_info &firstArg,
			       Args && ...args)
	{
		std::list<std::string> word_list;

		buildChallenges(word_list, firstArg,
				std::forward<Args>(args)...);
		emit(header, word_list);
	}


	//! Finish constructing the rest of the object, used by responseimpl

	//! \internal
	void build_version_set(const char *header,
			       const std::list<responseimpl::challenge_info>
			       &challenges);

	//! Finished collecting the parameters, emit the header

	void emit(const char *header,
		  const std::list<std::string> &word_list);

};

template<typename ...Args>
void responseimpl::throwAuthenticateException(int code,
					      const char *msg,
					      const char *header,
					      Args && ...args)
{
	response_exception resp(code, msg);

	resp.build(header, std::forward<Args>(args)...);

	throw resp;
}

#if 0
{
	{
#endif
	}
}
#endif
