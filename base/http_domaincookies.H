#ifndef x_http_domaincookies_H
#define x_http_domaincookies_H

#include "http_pathcookies.H"
#include "x/http/cookiemrulist.H"
#include "x/hier.H"

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

class allcookiesObj;
class cookiejarObj;
class storedcookieObj;

//! All cookies with the same domain.

class domaincookiesObj : virtual public obj {

 public:
	friend class cookiejarObj;

	//! A hierarchical container, by path component, for cookies.

	typedef hier<std::string, ref<pathcookiesObj> > cookiepathhier_t;

	//! The cookies, by path hierarchy

	cookiepathhier_t cookiepathhier;

	//! All cookies in this domain

	cookiemrulist_t allcookies;

	//! Constructor

	domaincookiesObj() LIBCXX_HIDDEN;

	//! Destructor

	~domaincookiesObj() noexcept LIBCXX_HIDDEN;

	//! Store a cookie

	void store(//! The lock on all.allcookies

		   cookiemrulist_t::lock &all_lock,

		   //! The cookie to store

		   //! The cookie's domain and path must be validated values.

		   const ref<storedcookieObj> &cookie,

		   //! The configured setting for max # of cookies per domain

		   size_t maxdomaincookies) LIBCXX_HIDDEN;

	//! Remove a given cookie in this domain

	void remove(cookiemrulist_t::lock &all_lock,
		    const ref<storedcookieObj> &cookie) LIBCXX_HIDDEN;

 private:

	//! Remove a given cookie in this domain

	//! \internal
	//!
	void remove(cookiemrulist_t::lock &all_lock,
		    cookiemrulist_t::lock &domaincookies_lock,
		    const ref<storedcookieObj> &cookie) LIBCXX_HIDDEN;

	//! Remove a cookie from cookiepathhier and allcookies

	//! \internal

	void drop(cookiemrulist_t::lock &all_lock,
		  cookiemrulist_t::lock &domain_cookies_lock,
		  const ref<storedcookieObj> &oldest_cookie);

	//! Remove a cookie from cookiepathhier and allcookies

	//! \internal
	static void drop(cookiemrulist_t::lock &all_lock,
			 cookiemrulist_t::lock &domaincookies_lock,
			 const ref<pathcookiesObj> &p,
			 pathcookiesObj::cookies_t::iterator old) LIBCXX_HIDDEN;
};

#if 0
{
	{
#endif
	}
}
#endif