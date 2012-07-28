/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "gnutls/rsaparamsobj.H"
#include "gnutls/init.H"
#include "sysexception.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

gnutls::rsaparamsObj::rsaparamsObj()
{
	gnutls::init::gnutls_init();

	chkerr(gnutls_rsa_params_init(&rsa),
	       "gnutls_rsa_params_init");
}

gnutls::rsaparamsObj::rsaparamsObj::operator gnutls_rsa_params_t()

{
	std::lock_guard<std::mutex> lock(objmutex); // Let's make this thread-safe

	gnutls_rsa_params_t cpy(rsa);

	try {
		chkerr(gnutls_rsa_params_init(&rsa),
		       "gnutls_rsa_params_init");
	} catch (...) {
		rsa=cpy;
		throw;
	}
	return cpy;
}

gnutls::rsaparamsObj::~rsaparamsObj() noexcept
{
	gnutls_rsa_params_deinit(rsa);
}

void gnutls::rsaparamsObj::generate(//! Number of bits
				    unsigned int bits)
{
	chkerr(gnutls_rsa_params_generate2(rsa, bits),
	       "gnutls_rsa_params_generate2");
}

gnutls::datum_t gnutls::rsaparamsObj::export_pk(gnutls_x509_crt_fmt_t format)

{
	// gnutls_rsa_params_export_pkcs1 is not thread safe

	std::lock_guard<std::mutex> lock(objmutex);

	size_t buf_s=0;

	int rc=gnutls_rsa_params_export_pkcs1(rsa, format, NULL, &buf_s);

	if (rc != GNUTLS_E_SHORT_MEMORY_BUFFER)
		chkerr(rc, "gnutls_rsa_params_export_pkcs1");

	datum_t ret(datum_t::create());

	ret->reserve(buf_s);
	ret->resize(buf_s);
	chkerr(gnutls_rsa_params_export_pkcs1(rsa, format,
					      &*ret->begin(), &buf_s),
	       "gnutls_rsa_params_export_pkcs1");
	ret->resize(buf_s);
	return ret;
}

void gnutls::rsaparamsObj::import_pk(datum_t rsaparams,
				     gnutls_x509_crt_fmt_t format)

{
	gnutls_datum_t datum;

	datum.data=&(*rsaparams)[0];
	datum.size=rsaparams->size();

	chkerr(gnutls_rsa_params_import_pkcs1(rsa, &datum, format),
	       "gnutls_rsa_params_import_pkcs1");
}


void gnutls::rsaparamsObj::export_raw(std::vector<datum_t> &raw_datum,
				      unsigned int &bits)
{
	// gnutls_rsa_params_export_pkcs1 is not thread safe, too tired to check
	// if this one is.

	std::lock_guard<std::mutex> lock(objmutex);

	gnutls_datum_t m, e, d, p, q, u;

	chkerr(gnutls_rsa_params_export_raw(rsa, &m, &e, &d, &p, &q, &u,
					    &bits),
	       "gnutls_rsa_params_export_raw");

	datumwrapper w_m(m), w_e(e), w_d(d), w_p(p), w_q(q), w_u(u);

	raw_datum.clear();
	raw_datum.reserve(6);

	raw_datum.push_back(w_m);
	raw_datum.push_back(w_e);
	raw_datum.push_back(w_d);
	raw_datum.push_back(w_p);
	raw_datum.push_back(w_q);
	raw_datum.push_back(w_u);
}

void gnutls::rsaparamsObj::import_raw(const std::vector<datum_t> &raw_datum)

{
	if (raw_datum.size() != 6)
	{
		errno=EINVAL;
		throw SYSEXCEPTION("gnutls::rsaparamsObj::import_raw");
	}

	gnutls_datum_t m, e, d, p, q, u;

	m.data=const_cast<unsigned char *>(&(*raw_datum[0])[0]);
	m.size=raw_datum[0]->size();

	e.data=const_cast<unsigned char *>(&(*raw_datum[1])[0]);
	e.size=raw_datum[1]->size();

	d.data=const_cast<unsigned char *>(&(*raw_datum[2])[0]);
	d.size=raw_datum[2]->size();

	p.data=const_cast<unsigned char *>(&(*raw_datum[3])[0]);
	p.size=raw_datum[3]->size();

	q.data=const_cast<unsigned char *>(&(*raw_datum[4])[0]);
	q.size=raw_datum[4]->size();

	u.data=const_cast<unsigned char *>(&(*raw_datum[5])[0]);
	u.size=raw_datum[5]->size();

	chkerr(gnutls_rsa_params_import_raw(rsa, &m, &e, &d, &p, &q, &u),
	       "gnutls_rsa_params_import_raw");
}

gnutls_pk_algorithm_t gnutls::rsaparamsObj::get_pk_algorithm()
{
	return pk_algorithm;
}


void gnutls::rsaparamsObj::import()
{
	import_pk(open_default("rsa"), GNUTLS_X509_FMT_PEM);
}

#if 0
{
#endif
};

