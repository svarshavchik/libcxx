#ifndef x_http_storedcookie_H
#define x_http_storedcookie_H

#include "x/http/cookie.H"
#include "x/obj.H"
#include "x/ref.H"
#include <list>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

class domaincookiesObj;
class pathcookiesObj;

//! A cookie that's stored in a cookie jar

//! This is just a reference-counted cookie.
//!
//! \note
//! The cookie's domain and path must be set from getValidatedPath() and
//! getValidatedDomain().

class storedcookieObj : public cookie, virtual public obj {

 public:
	//! Constructor
	storedcookieObj(const cookie &cookieArg) LIBCXX_HIDDEN;

	//! Destructor
	~storedcookieObj() noexcept LIBCXX_HIDDEN;

	//! This cookie in the MRU-list of all cookies for a domain

	std::list<ref<storedcookieObj> >::iterator domain_cookie;

	//! Using a raw pointer is safe here. This pathcookiesObj holds a
	//! reference on this object. When the reference gets removed, this
	//! storedcookie object gets destroyed. When the pathcookiesObj
	//! gets destroyed, the reference gets destroyed.
	//!
	//! This raw pointer gets accessed only while holding a write lock
	//! on the top level cookie jar object.

	pathcookiesObj *path_owner;

	//! This cookie in the MRU-list of all cookies in the cookie jar

	std::list<ref<storedcookieObj> >::iterator all_cookie;
};

#if 0
{
	{
#endif
	}
}
#endif
