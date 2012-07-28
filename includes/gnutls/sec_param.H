/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_sec_param_H
#define x_tls_sec_param_H

#include <x/namespace.h>
#include <string>
#include <x/locale.H>
#include <x/param/onlygetname.H>
#include <x/param/settable.H>
#include <x/param/getenumerationarr.H>
#include <gnutls/gnutls.h>

extern template class LIBCXX_NAMESPACE::param::enumeration<gnutls_sec_param_t>;
extern template class LIBCXX_NAMESPACE::param::onlygetname<gnutls_sec_param_t>;

extern template class LIBCXX_NAMESPACE::param
::getenumerationarr<LIBCXX_NAMESPACE::param::onlygetname
		    <gnutls_sec_param_t> >;

extern template class LIBCXX_NAMESPACE::param
::settable<gnutls_sec_param_t,
	   LIBCXX_NAMESPACE::param::getenumerationarr<
		   LIBCXX_NAMESPACE::param::onlygetname<gnutls_sec_param_t>
		   >,
	   GNUTLS_SEC_PARAM_UNKNOWN,
	   LIBCXX_NAMESPACE::class_tostring_utf8>;

namespace LIBCXX_NAMESPACE {
	namespace gnutls {
#if 0
	}
}
#endif

//! A gnutls_sec_param_t that can be specified by value or name

//! \code
//! INSERT_LIBX_NAMESPACE::gnutls::sec_param param(GNUTLS_SEC_PARAM_NORMAL);
//!
//! // ... ...
//!
//! INSERT_LIBX_NAMESPACE::gnutls::sec_param param("normal");
//!
//! std::vector<std::string> labels;
//!
//! INSERT_LIBX_NAMESPACE::gnutls::sec_param::enumerate(labels);
//! \endcode
//!
//! This helper object specifies a \c gnutls_sec_param setting, by either a
//! an explicit value or a string.
//!
//! enumerate() initializes the passed vector with the list of all known labels.
//!
//! \code
//! std::cout << (gnutls_sec_param_t)param;
//!
//! std::cout << (std::string)param;
//!
//! std::cout << INSERT_LIBX_NAMESPACE::tostring(param);
//! \endcode
//!
//! The sec_param object is convertible to a raw \c gnutls_sec_param_t or a
//! string. \ref tostring "INSERT_LIBX_NAMESPACE::tostring"() also knows
//! what to do with it.

typedef LIBCXX_NAMESPACE::param::settable<
	gnutls_sec_param_t,
	LIBCXX_NAMESPACE::param::getenumerationarr<
		LIBCXX_NAMESPACE::param::onlygetname<gnutls_sec_param_t>
		>,
	GNUTLS_SEC_PARAM_UNKNOWN,
	class_tostring_utf8> sec_param;

#if 0
{
	{
#endif
	}
}

namespace LIBCXX_NAMESPACE {
	namespace param {

#define LIBCXX_TEMPLATE_DECL extern
#define LIBCXX_SETTABLE_CLASS LIBCXX_NAMESPACE::gnutls::sec_param
#include <x/param/settable_t.H>
#undef LIBCXX_SETTABLE_CLASS
#undef LIBCXX_TEMPLATE_DECL

	}
}

#endif
