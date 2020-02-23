/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sysexception.H"
#include "x/gnutls/dhparamsobj.H"
#include "x/gnutls/init.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

gnutls::dhparamsObj::dhparamsObj()
{
	gnutls::init::gnutls_init();

	chkerr(gnutls_dh_params_init(&dh),
	       "gnutls_dh_params_init");
}

gnutls::dhparamsObj::dhparamsObj::operator gnutls_dh_params_t()

{
	std::lock_guard<std::mutex> lock(objmutex); // Let's make this thread-safe

	gnutls_dh_params_t cpy(dh);

	try {
		chkerr(gnutls_dh_params_init(&dh),
		       "gnutls_dh_params_init");
	} catch (...) {
		dh=cpy;
		throw;
	}
	return cpy;
}

gnutls::dhparamsObj::~dhparamsObj()
{
	gnutls_dh_params_deinit(dh);
}

void gnutls::dhparamsObj::generate(//! Number of bits
				   unsigned int bits)
{
	chkerr(gnutls_dh_params_generate2(dh, bits),
	       "gnutls_dh_params_generate2");
}

gnutls::datum_t gnutls::dhparamsObj::export_pk(gnutls_x509_crt_fmt_t format)
	const
{
	// gnutls_rsa_params_export_pkcs1 is not thread safe, too tired to check
	// if this one is.
	std::lock_guard<std::mutex> lock(objmutex);

	size_t buf_s=0;

	int rc=gnutls_dh_params_export_pkcs3(dh, format, NULL, &buf_s);

	if (rc != GNUTLS_E_SHORT_MEMORY_BUFFER)
		chkerr(rc, "gnutls_dh_params_export_pkcs3");

	datum_t ret(datum_t::create());

	ret->reserve(buf_s);
	ret->resize(buf_s);
	chkerr(gnutls_dh_params_export_pkcs3(dh, format, &*ret->begin(),
					     &buf_s),
	       "gnutls_dh_params_export_pkcs3");
	ret->resize(buf_s);
	return ret;
}

void gnutls::dhparamsObj::import_pk(datum_t dhparams,
				    gnutls_x509_crt_fmt_t format)
{
	gnutls_datum_t datum;

	datum.data=&(*dhparams)[0];
	datum.size=dhparams->size();

	chkerr(gnutls_dh_params_import_pkcs3(dh, &datum, format),
	       "gnutls_dh_params_import_pkcs3");
}

void gnutls::dhparamsObj::export_raw(std::vector<datum_t> &raw_datum,
				     unsigned int &bits) const
{
	// gnutls_rsa_params_export_pkcs1 is not thread safe, too tired to check
	// if this one is.

	std::lock_guard<std::mutex> lock(objmutex);

	gnutls_datum_t prime, generator;

	chkerr(gnutls_dh_params_export_raw(dh, &prime, &generator, &bits),
	       "gnutls_dh_params_export_raw");

	datumwrapper w_prime(prime), w_generator(generator);

	raw_datum.clear();
	raw_datum.reserve(2);

	raw_datum.push_back(w_prime);
	raw_datum.push_back(w_generator);
}

void gnutls::dhparamsObj::import_raw(const std::vector<datum_t> &raw_datum)

{
	if (raw_datum.size() != 2)
	{
		errno=EINVAL;
		throw SYSEXCEPTION("gnutls::dhparamsObj::import_raw");
	}

	gnutls_datum_t prime, generator;

	prime.data=const_cast<unsigned char *>(&(*raw_datum[0])[0]);
	prime.size=raw_datum[0]->size();

	generator.data=const_cast<unsigned char *>(&(*raw_datum[1])[0]);
	generator.size=raw_datum[1]->size();

	chkerr(gnutls_dh_params_import_raw(dh, &prime, &generator),
	       "gnutls_dh_params_import_raw");
}

gnutls_pk_algorithm_t gnutls::dhparamsObj::get_pk_algorithm() const
{
	return pk_algorithm;
}

void gnutls::dhparamsObj::import()
{
	import_pk(open_default("dh"), GNUTLS_X509_FMT_PEM);
}

#if 0
{
#endif
};

