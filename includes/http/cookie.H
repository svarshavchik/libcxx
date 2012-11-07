/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_cookie_H
#define x_http_cookie_H

#include <string>
#include <time.h>

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	class uriimpl;

	namespace http {
#if 0
	};
};
#endif

class httpresponseimpl;

//! An HTTP cookie.

class cookie {

public:
	std::string name;		//!< Cookie's name

	std::string value;		//!< Cookie's value

	//! Cookie's expiration or (time_t)-1 if not given.

	//! Populated from Expires or Max-Age attribute.
	//!

	time_t expiration;

	//! Whether the cookie expires

	bool has_expiration() const
	{
		return expiration != (time_t)-1;
	}

	//! Convert the expiration to a string, in HTTP date+time GMT format

	std::string expires_as_string() const;

	//! The opposite conversion.

	//! Times before epoch are converted to (time_t)0. Times past the
	//! epoch are converted to the largest possible time_t.

	static time_t expires_from_str(const std::string &value_str);

	//! Given domain, in lowercase.

	//! Begins with a dot, if attribute present in the cookie, otherwise
	//! defaults to the URI's host, without the leading dot.

	std::string domain;

	//! Path, if given. If not, defaults to URI's path's "directory".

	std::string path;

	//! Secure flag, if given

	bool secure;

	//! httponly flag, if given

	bool httponly;

	//! Default constructor
	cookie();

	//! Simple case
	cookie(const std::string &nameArg,
	       const std::string &valueArg);

	//! Destructor
	~cookie() noexcept;

	//! Check path attribute of the cookie per section 4.1.2.4 of RFC 6265

	//! If no path attribute was specified, returns the default path
	//! derived from request_uri.
	//!
	//! "/" gets returned for the root path.
	//!
	//! If the path attribute was specified, it must begin with a "/".
	//! If the path attribute ends with "/" (and it is not "/"), the
	//! trailing "/" gets removed.
	//!
	//! An empty string gets returned if these checks fail.

	std::string getValidatedPath(const uriimpl &request_uri) const;

	//! Check domain attribute of the cookie per section 4.1.2.3 of RFC 6265

	//! If no domain attribute was specified, returns the default domain
	//! derived from request_uri.
	//!
	//! Otherwise returns the domain attribute gets returned, converted to
	//! lowercase, and it will always have a leading dot.
	//!
	//! An empty string gets returned if domain fails checks described
	//! in section 4.1.2.3 of RFC 6265.

	std::string getValidatedDomain(const uriimpl &request_uri) const;
};

#if 0
{
	{
#endif
	}
}
#endif
