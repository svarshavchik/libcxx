#ifndef x_http_cookiemrulist_H
#define x_http_cookiemrulist_H

#include <x/mpobj.H>
#include <x/ptrfwd.H>

#include <list>
#include <map>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

class storedcookieObj;

//! A most-recently-used cookie list.

//! This ends up being protected by a mutex. The mutex protection implements
//! thread-safety, the actual methods are not.

struct cookiemrulist {

	//! Constructor
	cookiemrulist() LIBCXX_HIDDEN;

	//! Destructor
	~cookiemrulist() LIBCXX_HIDDEN;

	//! MRU list for a given public suffix

	struct ps_mrulist {

		//! The size of the list.
		size_t count;

		//! The list
		std::list< ref<storedcookieObj> > list;

		//! The constructor needs only to set count to 0

		ps_mrulist() LIBCXX_HIDDEN;
	};

	//! Container for the MRU list, keyed by public suffix
	typedef std::map<std::string, ps_mrulist> mru_t;

	//! The MRU list

	mru_t mru;

	//! An iterator for a cookie in the MRU list

	//! The iterator consists of two actual iterators, an iterator for
	//! the mru_t container, and an iterator for the std::list in the
	//! ps_mrulist.

	typedef std::pair<mru_t::iterator,
			  std::list<ref<storedcookieObj>>::iterator
			  > cookie_mru_iter_t;

	//! Insert the cookie

	cookie_mru_iter_t insert(const ref<storedcookieObj> &cookie)
		LIBCXX_HIDDEN;

	//! Remove

	void remove(cookie_mru_iter_t iter)
		noexcept LIBCXX_HIDDEN;

	//! A refresh

	cookie_mru_iter_t refresh(cookie_mru_iter_t iter)
		LIBCXX_HIDDEN;

};

//! A mutex-protected container for cookiemrulist;

typedef mpobj<cookiemrulist> cookiemrulist_t;

#if 0
{
#endif
}
#endif
