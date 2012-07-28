/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "gnutls/dsapubkey.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

gnutls::dsapubkeyobj::dsapubkeyobj() :
	p(datum_t::create()),
	q(datum_t::create()),
	g(datum_t::create()),
	y(datum_t::create())
{
}

gnutls::dsapubkeyobj::~dsapubkeyobj() noexcept
{
}

gnutls_pk_algorithm_t gnutls::dsapubkeyobj::get_pk_algorithm()
	const
{
	return GNUTLS_PK_DSA;
}

#if 0
{
#endif
};
