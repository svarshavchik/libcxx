/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_cookiejar_H
#define x_http_cookiejar_H

#include <algorithm>

#include <x/http/cookie.H>
#include <x/http/cookiejarfwd.H>
#include <x/http/cookiejarobj.H>
#include <x/ref.H>
#include <x/ptr.H>
#include <x/namespace.h>

#include <iterator>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

//! Refer to this class as \c http::cookiejar::base or \c http::cookiejarptr::base

class cookiejarBase : public ptrref_base {

public:

	//! Convenience typedef

	typedef cookiejarObj::iterator iterator;

	//! Return the public suffix of a domain

	//! This basically returns the top-level domain, given a subdomain.
	//! Passing a top-level domain just returns it. In all cases, the
	//! domain gets converted to lowercase.

	static std::string public_suffix(//! Domain

					 //! An international domain name must
					 //! be converted to ASCII-compatible
					 //! encoding (punicode).
					 const std::string &domain);
};

#if 0
{
	{
#endif
	}
}
#endif
