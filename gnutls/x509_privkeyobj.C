/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/gnutls/init.H"
#include "x/gnutls/x509_privkeyobj.H"
#include "x/gnutls/x509_privkey.H"
#include "x/gnutls/dhparams.H"
#include "x/gnutls/datumwrapper.H"
#include "x/exception.H"
#include <gnutls/gnutls.h>
#include <cstring>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

void gnutls::x509::privkeyBase
::get_algorithm_list(std::list<gnutls_pk_algorithm_t> &listRet) noexcept
{
	listRet.clear();

	const gnutls_pk_algorithm_t *pk=gnutls_pk_list();

	while (*pk)
		listRet.push_back(*pk++);
}

std::string gnutls::x509::privkeyBase::
get_algorithm_name(gnutls_pk_algorithm_t a)
{
	const char *str=gnutls_pk_get_name(a);

	if (!str)
		throw EXCEPTION("No such public key algorithm");

	return str;
}

gnutls_pk_algorithm_t gnutls::x509::privkeyBase::
get_algorithm_id(const std::string &name)
{
	std::string name_upper=name;

	std::transform(name_upper.begin(), name_upper.end(),
		       name_upper.begin(),
		       [](char c) noexcept->char{
			       if (c >= 'a' && c <= 'z')
				       c += 'A'-'a';
			       return c;
		       });

	gnutls_pk_algorithm_t pk=gnutls_pk_get_id(name_upper.c_str());

	if (pk == GNUTLS_PK_UNKNOWN)
		throw EXCEPTION("No such public key algorithm: " + name);

	return pk;
}


gnutls::x509::privkeyObj::privkeyObj()
{
	gnutls::init::gnutls_init();

	chkerr(gnutls_x509_privkey_init(&privkey), "gnutls_x509_privkey_init");
}

gnutls::x509::privkeyObj::~privkeyObj()
{
	gnutls_x509_privkey_deinit(privkey);
}

void gnutls::x509::privkeyObj::generate(gnutls_pk_algorithm_t algo,
					const sec_param &strength)
{
	chkerr(gnutls_x509_privkey_generate(privkey, algo,
					    gnutls_sec_param_to_pk_bits(algo,
									strength
									),
					    0),
	       "gnutls_x509_privkey_generate");	       
}

void gnutls::x509::privkeyObj::generate(const std::string &algo,
					const sec_param &strength)
{
	generate(gnutls::x509::privkey::base::get_algorithm_id(algo),
		 strength);
}

void gnutls::x509::privkeyObj::fix()
{
	chkerr(gnutls_x509_privkey_fix(privkey),
	       "gnutls_x509_privkey_fix");
}

gnutls::datum_t gnutls::x509::privkeyObj
::export_pkcs1(gnutls_x509_crt_fmt_t format)
	const
{
	datum_t buf=datum_t::create();

	size_t n=0;

	while (1)
	{
		n += 512;

		size_t s_cpy=n;

		char tmp_buf[n];

		int rc=gnutls_x509_privkey_export(privkey, format, tmp_buf,
						  &s_cpy);

		if (rc == 0)
		{
			buf->resize(s_cpy);
			memcpy(&(*buf)[0], tmp_buf, s_cpy);
			break;
		}

		if (rc != GNUTLS_E_SHORT_MEMORY_BUFFER)
			chkerr(rc, "gnutls_x509_privkey_export");
	}

	return buf;
}	       

void gnutls::x509::privkeyObj::export_pkcs1(const std::string &filename,
					    gnutls_x509_crt_fmt_t format)
	const
{
	export_pkcs1(format)->save(filename);
}

void gnutls::x509::privkeyObj::import_pkcs1(datum_t keyref,
					    gnutls_x509_crt_fmt_t format)

{
	gnutls_datum_t d;

	d.data=(unsigned char *)&(*keyref)[0];
	d.size=keyref->size();

	chkerr(gnutls_x509_privkey_import(privkey, &d, format),
	       "gnutls_x509_privkey_import");
}

void gnutls::x509::privkeyObj::import_pkcs1(const std::string &filename,
					    gnutls_x509_crt_fmt_t format)

{
	gnutls::datum_t key(gnutls::datum_t::create());

	key->load(filename);
	import_pkcs1(key, format);
}

gnutls::datum_t gnutls::x509::privkeyObj
::export_pkcs8(gnutls_x509_crt_fmt_t format,
	       const std::string &password,
	       gnutls_pkcs_encrypt_flags_t flags)
	const
{
	datum_t buf=datum_t::create();

	size_t n=0;

	while (1)
	{
		n += 512;

		size_t s_cpy=n;

		char tmp_buf[n];

		int rc=gnutls_x509_privkey_export_pkcs8(privkey, format,
							password.c_str(),
							flags,
							tmp_buf,
							&s_cpy);

		if (rc == 0)
		{
			buf->resize(s_cpy);
			memcpy(&(*buf)[0], tmp_buf, s_cpy);
			break;
		}

		if (rc != GNUTLS_E_SHORT_MEMORY_BUFFER)
			chkerr(rc, "gnutls_x509_privkey_export_pkcs8");
	}

	return buf;
}	       

void gnutls::x509::privkeyObj::import_pkcs8(datum_t keyref,
					    gnutls_x509_crt_fmt_t format,
					    const std::string &password,
					    gnutls_pkcs_encrypt_flags_t flags)

{
	gnutls_datum_t d;

	d.data=(unsigned char *)&(*keyref)[0];
	d.size=keyref->size();

	chkerr(gnutls_x509_privkey_import_pkcs8(privkey, &d, format,
						password.c_str(),
						flags),
	       "gnutls_x509_privkey_import_pkcs8");
}

gnutls::datum_t gnutls::x509::privkeyObj::get_key_id() const
{
	size_t n=128;

	while(1)
	{
		unsigned char buffer[n];
		size_t nsize=n;

		int rc=gnutls_x509_privkey_get_key_id(privkey, 0, buffer,
						      &nsize);

		n += 128;

		if (rc == GNUTLS_E_SHORT_MEMORY_BUFFER)
			continue;

		chkerr(rc, "gnutls_x509_privkey_get_key_id");

		gnutls::datum_t id(gnutls::datum_t::create());

		id->reserve(nsize);
		id->resize(nsize);
		std::copy(buffer, buffer+nsize, id->begin());
		return id;
	}
}

gnutls_pk_algorithm_t gnutls::x509::privkeyObj::get_pk_algorithm()
	const
{
	int rc=gnutls_x509_privkey_get_pk_algorithm(privkey);

	chkerr(rc, "gnutls_x509_privkey_get_pk_algorithm");
	return (gnutls_pk_algorithm_t) rc;
}

gnutls::pkparams gnutls::x509::privkeyObj::get_pkparams()
	const
{
	return pkparams::create(get_pk_algorithm());
}

gnutls::privkey gnutls::x509::privkeyObj::export_raw() const
{
	switch (get_pk_algorithm()) {
	case GNUTLS_PK_RSA:
		return export_rsa_raw();
	case GNUTLS_PK_DSA:
		return export_dsa_raw();
	default:
		break;
	}

	throw EXCEPTION("Unknown public key algorithm");
}

void gnutls::x509::privkeyObj::import_raw(gnutls::privkey privkey)

{
	switch (privkey->get_pk_algorithm()) {
	case GNUTLS_PK_RSA:
		import_rsa_raw(privkey);
		return;
	case GNUTLS_PK_DSA:
		import_dsa_raw(privkey);
		return;
	default:
		break;
	}

	throw EXCEPTION("Unknown public key algorithm");
}


gnutls::rsaprivkey gnutls::x509::privkeyObj::export_rsa_raw()
	const
{
	gnutls_datum_t m, e, d, p, q, u;

	chkerr(gnutls_x509_privkey_export_rsa_raw(privkey, &m, &e, &d, &p,
						  &q, &u),
	       "gnutls_x509_privkey_export_rsa_raw");


	gnutls::datumwrapper mw(m), ew(e), dw(d), pw(p), qw(q), uw(u);

	gnutls::rsaprivkey raw(gnutls::rsaprivkey::create());

	raw->m=mw;
	raw->e=ew;
	raw->d=dw;
	raw->p=pw;
	raw->q=qw;
	raw->u=uw;

	return raw;
}

void gnutls::x509::privkeyObj::import_rsa_raw(gnutls::rsaprivkey rawprivkey)

{
	gnutls::datumwrapper m(rawprivkey->m),
		e(rawprivkey->e),
		d(rawprivkey->d),
		p(rawprivkey->p),
		q(rawprivkey->q),
		u(rawprivkey->u);

	chkerr(gnutls_x509_privkey_import_rsa_raw(privkey,
						  &m.datum,
						  &e.datum,
						  &d.datum,
						  &p.datum,
						  &q.datum,
						  &u.datum),
	       "gnutls_x509_privkey_import_rsa_raw");
}

gnutls::dsaprivkey gnutls::x509::privkeyObj::export_dsa_raw()
	const
{
	gnutls_datum_t p, q, g, y, x;

	chkerr(gnutls_x509_privkey_export_dsa_raw(privkey, &p, &q, &g, &y, &x),
	       "gnutls_x509_privkey_export_dsa_raw");


	gnutls::datumwrapper pw(p), qw(q), gw(g), yw(y), xw(x);

	gnutls::dsaprivkey raw(gnutls::dsaprivkey::create());

	raw->p=pw;
	raw->q=qw;
	raw->g=gw;
	raw->y=yw;
	raw->x=xw;

	return raw;
}

void gnutls::x509::privkeyObj::import_dsa_raw(gnutls::dsaprivkey rawprivkey)

{
	gnutls::datumwrapper p(rawprivkey->p),
		q(rawprivkey->q),
		g(rawprivkey->g),
		y(rawprivkey->y),
		x(rawprivkey->x);

	chkerr(gnutls_x509_privkey_import_dsa_raw(privkey,
						  &p.datum,
						  &q.datum,
						  &g.datum,
						  &y.datum,
						  &x.datum),
	       "gnutls_x509_privkey_import_dsa_raw");
}

#if 0
{
#endif
};
