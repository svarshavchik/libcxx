#ifndef x_http_pathcookies_H
#define x_http_pathcookies_H

#include "http_storedcookie.H"
#include <x/namespace.h>
#include <set>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

//! All cookies with the same path.

class pathcookiesObj : public cookie, virtual public obj {

	//! Comparator class for the set
	class cmp {

	public:

		//! Comparator function
		bool operator()(const ref<storedcookieObj> &a,
				const ref<storedcookieObj> &b) LIBCXX_HIDDEN;
	};

 public:
	//! Constructor
	pathcookiesObj() LIBCXX_HIDDEN;

	//! Destructor
	~pathcookiesObj() noexcept LIBCXX_HIDDEN;

	//! Container for the stored cookies
	typedef std::set<ref<storedcookieObj>, cmp> cookies_t;

	//! Stored cookies

	//! store() acquires a write lock on domaincookiesObj's allcookies
	//! container, where this set lives. find() acquires a read lock,
	//! and thus gets blocked, while the write lock is held, so it's
	//! safe to access this container without any additional locking.

	cookies_t cookies;
};

#if 0
{
	{
#endif
	}
}
#endif
