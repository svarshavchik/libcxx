#ifndef x_http_cookiemrulist_H
#define x_http_cookiemrulist_H

#include <x/mpobj.H>
#include <x/ptrfwd.H>

#include <list>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

class storedcookieObj;

//! A most-recently-used cookie list.

//! This ends up being protected by a mutex. The mutex protection implements
//! thread-safety, the actual methods are not.

struct cookiemrulist {

	//! Constructor
	cookiemrulist() LIBCXX_HIDDEN;

	//! Destructor
	~cookiemrulist() noexcept LIBCXX_HIDDEN;

	//! The size of the list.
	size_t count;

	//! The list
	std::list< ref<storedcookieObj> > list;

	//! Insert
	std::list< ref<storedcookieObj> >::iterator
		insert(const ref<storedcookieObj> &cookie) LIBCXX_HIDDEN;

	//! Remove

	void remove(std::list< ref<storedcookieObj> >::iterator iter)
		noexcept LIBCXX_HIDDEN;

	//! A refresh

	std::list< ref<storedcookieObj> >::iterator
		refresh(std::list< ref<storedcookieObj> >::iterator iter)
		LIBCXX_HIDDEN;

};

//! A mutex-protected container for cookiemrulist;

typedef mpobj<cookiemrulist> cookiemrulist_t;

#if 0
{
	{
#endif
	}
}
#endif
