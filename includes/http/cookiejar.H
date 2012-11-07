/*
** Copyright 2012 Double Precision, Inc.
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

class cookiejarBase : public ptrrefBase {

public:

	//! Convenience typedef

	typedef cookiejarObj::iterator iterator;
};

#if 0
{
	{
#endif
	}
}
#endif
