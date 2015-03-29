/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/gnutls/init.H"
#include "x/param/enumeration.H"
#include "x/gnutls/digest_algorithm.H"
#include <gnutls/gnutls.h>

#include "gnutls_defs.h"

namespace LIBCXX_NAMESPACE {
	namespace gnutls {
#if 0
	}
}
#endif

const gnutls_mac_algorithm_t *digest_algorithm_base::get_ntl()
{
	return gnutls_mac_list();
}

bool digest_algorithm_base::donotenumerate(gnutls_mac_algorithm_t ignore)
{
	return false;
}

#if 0
{
	{
#endif
	}
}

namespace LIBCXX_NAMESPACE {
	namespace param {
#if 0
	}
}
#endif

template class getnameenum<gnutls_digest_algorithm_t>;

template<>
std::string
getnameenum<gnutls_digest_algorithm_t>::
get_name(gnutls_digest_algorithm_t value, const const_locale &localeRef)
{
	auto p = gnutls_mac_get_name( (gnutls_mac_algorithm_t)value );

	if (!p)
		return "(unknown digest)";

	return ctype(localeRef).toupper(p);
}


template<>
gnutls_digest_algorithm_t
getnameenum<gnutls_digest_algorithm_t>::get_enum(const std::string &name,
						 const const_locale &localeRef)
{
	return (gnutls_digest_algorithm_t)
		gnutls_mac_get_id(ctype(localeRef).toupper(name).c_str());
}

template class getenumerationntl<LIBCXX_NAMESPACE::gnutls::
				 digest_algorithm_base>;

template class settable<gnutls_digest_algorithm_t,
			getenumerationntl<LIBCXX_NAMESPACE::gnutls
					  ::digest_algorithm_base>,
			GNUTLS_DIG_UNKNOWN,
			LIBCXX_NAMESPACE::class_tostring_utf8>;

#if 0
{
	{
#endif
	}
}

#define LIBCXX_TEMPLATE_DECL
#define LIBCXX_SETTABLE_CLASS LIBCXX_NAMESPACE::gnutls::digest_algorithm
#define LIBCXX_TEMPLATE_DECLARE
#include "x/param/settable_t.H"
#undef LIBCXX_TEMPLATE_DECLARE
#undef LIBCXX_SETTABLE_CLASS
#undef LIBCXX_TEMPLATE_DECL
