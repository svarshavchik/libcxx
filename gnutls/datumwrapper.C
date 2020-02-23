/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/gnutls/datumwrapper.H"
#include "x/sysexception.H"

#include <algorithm>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

gnutls::datumwrapper::datumwrapper(const gnutls_datum_t &valueRef) noexcept
	: datum(valueRef)
{
}

gnutls::datumwrapper::~datumwrapper()
{
	if (datum.data && datum.size)
		gnutls_free(datum.data);
}

gnutls::datumwrapper::datumwrapper(datum_t importRef)

{
	datum.data=0;
	datum.size=0;

	try {
		datum.data=(unsigned char *)gnutls_malloc(importRef->size());
	} catch (...)
	{
		datum.data=0;
	}
	if (!datum.data)
		throw SYSEXCEPTION("gnutls_malloc");

	datum.size=importRef->size();
	std::copy(importRef->begin(), importRef->end(), datum.data);
}

gnutls::datumwrapper::operator datum_t() const
{
	return convert(datum);
}

gnutls::datum_t gnutls::datumwrapper::convert(const gnutls_datum_t &datum)

{
	datum_t r(datum_t::create());

	if (datum.data && datum.size)
	{
		r->reserve(datum.size);
		r->resize(datum.size);
		std::copy(datum.data, datum.data+datum.size, r->begin());
	}
	return r;
}

gnutls::tempdatum::tempdatum(const const_datum_t &valueRef) noexcept
{
	datum.data=const_cast<unsigned char *>( &(*valueRef)[0] );
	datum.size=valueRef->size();
}

#if 0
{
#endif
};
