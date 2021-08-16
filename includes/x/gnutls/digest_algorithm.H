/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_digest_algorithm_H
#define x_tls_digest_algorithm_H

#include <x/namespace.h>
#include <x/param/getnameenum.H>
#include <x/param/getenumerationntl.H>
#include <x/param/settable.H>
#include <gnutls/gnutls.h>

namespace LIBCXX_NAMESPACE::gnutls {
#if 0
}
#endif

//! Base class for \ref param::getenumerationntl "param::getenumerationntl" that implements get_ntl() and donotenumerate()

class digest_algorithm_base :
	public LIBCXX_NAMESPACE::param::getnameenum<gnutls_digest_algorithm_t>
{
public:

	//! Call gnutls_mac_list()
	static const gnutls_mac_algorithm_t *get_ntl();

	//! Always returns false
	static bool donotenumerate(gnutls_mac_algorithm_t);
};

//! A gnutls_digest_algorithm_t that can be specified by value, or a name.

//! \code
//! INSERT_LIBX_NAMESPACE::gnutls::digest_algorithm param(GNUTLS_DIG_SHA1);
//!
//! // ... ...
//!
//! INSERT_LIBX_NAMESPACE::gnutls::digest_algorithm param("sha1");
//!
//! std::vector<std::string> labels;
//!
//! INSERT_LIBX_NAMESPACE::gnutls::digest_algorithm::enumerate(labels);
//! \endcode
//!
//! This helper object specifies a \c gnutls_digest_algorithm_t setting,
//! by either a
//! an explicit value or a string.
//!
//! enumerate() initializes the passed vector with the list of all known labels.
//!
//! \code
//! std::cout << (gnutls_digest_algorithm_t)param;
//!
//! std::cout << (std::string)param;
//!
//! std::cout << INSERT_LIBX_NAMESPACE::tostring(param);
//! \endcode
//!
//! The digest_algorithm object is convertible to a raw
//! \c gnutls_digest_algorithm_t or a
//! string. \c INSERT_LIBX_NAMESPACE::tostring() also knows
//! what to do with it.

typedef LIBCXX_NAMESPACE::param::settable<
	gnutls_digest_algorithm_t,
	LIBCXX_NAMESPACE::param
	::getenumerationntl<digest_algorithm_base>,
	GNUTLS_DIG_UNKNOWN> digest_algorithm;

#if 0
{
#endif
}

namespace LIBCXX_NAMESPACE::param {
#if 0
}
#endif

//! Internal specialization.

template<>
std::string
getnameenum<gnutls_digest_algorithm_t>::
get_name(gnutls_digest_algorithm_t value, const const_locale &localeRef);

//! Internal specialization.

template<>
gnutls_digest_algorithm_t
getnameenum<gnutls_digest_algorithm_t>::get_enum(const std::string &name,
						 const const_locale &localeRef);
#if 0
{
#endif
}

extern template class LIBCXX_NAMESPACE::param
::getnameenum<gnutls_digest_algorithm_t>;

extern template class LIBCXX_NAMESPACE::param
::getenumerationntl<LIBCXX_NAMESPACE::gnutls::digest_algorithm_base>;

extern template class LIBCXX_NAMESPACE::param
::settable<gnutls_digest_algorithm_t,
	   LIBCXX_NAMESPACE::param::getenumerationntl<
		   LIBCXX_NAMESPACE::gnutls::digest_algorithm_base>,
	   GNUTLS_DIG_UNKNOWN>;

namespace LIBCXX_NAMESPACE::param {
#define LIBCXX_TEMPLATE_DECL extern
#define LIBCXX_SETTABLE_CLASS LIBCXX_NAMESPACE::gnutls::digest_algorithm
#include <x/param/settable_t.H>
#undef LIBCXX_SETTABLE_CLASS
#undef LIBCXX_TEMPLATE_DECL
}

#endif
