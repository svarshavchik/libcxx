/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_serverauthfwd_H
#define x_http_serverauthfwd_H

#include <x/ptrfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

class serverauthObj;
class serverauthBase;

//! Collection of authorizations used by an HTTP request.

typedef ref<serverauthObj, serverauthBase> serverauth;

#if 0
{
	{
#endif
	}
}

#endif