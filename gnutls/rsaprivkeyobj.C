/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/gnutls/rsaprivkeyobj.H"
#include "x/gnutls/x509_privkey.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

gnutls::rsaprivkeyobj::rsaprivkeyobj() :
	d(datum_t::create()),
	p(datum_t::create()),
	q(datum_t::create()),
	u(datum_t::create())
{
}

gnutls::rsaprivkeyobj::~rsaprivkeyobj()
{
}

gnutls::x509::privkey gnutls::rsaprivkeyobj::import_raw() const
{
	gnutls::x509::privkey newKey(gnutls::x509::privkey::create());

	tempdatum mwrap(m),
		ewrap(e),
		dwrap(d),
		pwrap(p),
		qwrap(q),
		uwrap(u);

	chkerr(gnutls_x509_privkey_import_rsa_raw(newKey->privkey,
						  &mwrap.datum,
						  &ewrap.datum,
						  &dwrap.datum,
						  &pwrap.datum,
						  &qwrap.datum,
						  &uwrap.datum),
	       "gnutls_x509_privjey_import_rsa_raw");
	return newKey;
}


#if 0
{
#endif
};
