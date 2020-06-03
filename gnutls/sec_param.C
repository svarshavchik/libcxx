/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/gnutls/sec_param.H"
#include "gettext_in.h"
#include "x/messages.H"

#include "gnutls_defs.h"

#include "x/param/getenumerationarr_int.H"

namespace LIBCXX_NAMESPACE::param {
#if 0
}
#endif

template class onlygetname<gnutls_sec_param_t>;
template class enumeration<gnutls_sec_param_t>;
template class getenumerationarr<onlygetname<gnutls_sec_param_t> >;

template class settable<gnutls_sec_param_t,
			getenumerationarr<
				onlygetname<gnutls_sec_param_t> >,
			GNUTLS_SEC_PARAM_UNKNOWN>;

template<>
const gnutls_sec_param_t
enumeration<gnutls_sec_param_t>::manual_enumeration[] = {
	GNUTLS_SEC_ENUM
};

template<> const size_t
enumeration<gnutls_sec_param_t>::manual_enumeration_cnt
= sizeof(manual_enumeration)/sizeof(manual_enumeration[0]);

template<> std::string
onlygetname<gnutls_sec_param_t>::get_name(gnutls_sec_param_t value,
					  const const_locale &localeRef)
{
	return gnutls_sec_param_get_name(value);
}

template<> void onlygetname<gnutls_sec_param_t>
::nosuchname(const std::string &name,
	     const const_locale &localeRef)
{
	throw EXCEPTION(gettextmsg(_("Unknown security strength: %1%"), name));
}

template<>
bool getenumerationarr<onlygetname<gnutls_sec_param_t> >
::donotenumerate(gnutls_sec_param_t value)
{
	return value == GNUTLS_SEC_PARAM_UNKNOWN;
}

#if 0
{
#endif
}

#define LIBCXX_TEMPLATE_DECL
#define LIBCXX_SETTABLE_CLASS LIBCXX_NAMESPACE::gnutls::sec_param
#define LIBCXX_TEMPLATE_DECLARE
#include "x/param/settable_t.H"
#undef LIBCXX_TEMPLATE_DECLARE
#undef LIBCXX_SETTABLE_CLASS
#undef LIBCXX_TEMPLATE_DECL
