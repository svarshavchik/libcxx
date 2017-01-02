/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/gnutls/dsaprivkeyobj.H"
#include "x/gnutls/x509_privkey.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

gnutls::dsaprivkeyobj::dsaprivkeyobj()
	: x(datum_t::create())
{
}

gnutls::dsaprivkeyobj::~dsaprivkeyobj()
{
}

gnutls::x509::privkey gnutls::dsaprivkeyobj::import_raw() const
{
	gnutls::x509::privkey newKey(gnutls::x509::privkey::create());

	tempdatum pwrap(p),
		qwrap(q),
		gwrap(g),
		ywrap(y),
		xwrap(x);

	chkerr(gnutls_x509_privkey_import_dsa_raw(newKey->privkey,
						  &pwrap.datum,
						  &qwrap.datum,
						  &gwrap.datum,
						  &ywrap.datum,
						  &xwrap.datum),
	       "gnutls_x509_privjey_import_dsa_raw");
	return newKey;
}

#if 0
{
#endif
};
