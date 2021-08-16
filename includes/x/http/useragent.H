/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_useragent_H
#define x_http_useragent_H

#include <algorithm>

#include <x/http/useragentfwd.H>
#include <x/http/useragentobj.H>
#include <x/ptr.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

//! Refer to this class as \c http::useragent::base or \c http::useragentptr::base

class useragentBase : public ptrref_base {

public:
	//! A challenge factory
	typedef useragentObj::challenge challenge;

	//! A nullable pointer reference to a challenge factory
	typedef useragentObj::challengeptr challengeptr;

	//! A reference to a response object.

	//! useragent's request() returns this reference to a
	//! \ref useragentObj::responseObj "response object".

	typedef useragentObj::response response;

	//! A nullable pointer to a response object.

	//! \see response

	typedef useragentObj::responseptr responseptr;

	//! An iterator for the contents of a response

	//! This object is returned by response->begin() and response->end().

	typedef useragentObj::response::base::iterator body_iterator;

	//! The container for server's authentication challenges

	typedef useragentObj::responseObj::challenges_t challenges_t;

	//! https support linkage

	static void https_enable() noexcept;

	//! Construct a body parameter for useragent->request(), given two iterators

	template<typename iter_type>
	static std::pair<iter_type, iter_type> body(iter_type begin_iter,
						    iter_type end_iter)

	{
		return std::make_pair(begin_iter, end_iter);
	}

	//! Construct a body parameter for useragent->request(), given a container

	template<typename container_type>
	static std::pair<typename container_type::const_iterator,
			 typename container_type::const_iterator>
		body(const container_type &container)

	{
		return body(container.begin(), container.end());
	}

	//! Return a global, default useragent

	//! Use this to return a useragent that's shared with the
	//! \ref httportmap "portmapper".

	static useragent global();
};

#if 0
{
#endif
}
#endif
