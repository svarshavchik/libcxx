/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "gnutls/rsapubkey.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

gnutls::rsapubkeyobj::rsapubkeyobj() :
	m(datum_t::create()),
	e(datum_t::create())
{
}

gnutls::rsapubkeyobj::~rsapubkeyobj() noexcept
{
}

gnutls_pk_algorithm_t gnutls::rsapubkeyobj::get_pk_algorithm()
	const
{
	return GNUTLS_PK_RSA;
}

#if 0
{
#endif
};

