/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/gnutls/session.H"
#include "x/gnutls/credentials.H"
#include "x/gnutls/dhparams.H"
#include "x/gnutls/x509_crt.H"
#include "x/gnutls/init.H"
#include "x/logger.H"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

property::value<std::string> gnutls::credentials::certificateObj
::calist(LIBCXX_NAMESPACE_STR "::gnutls::calist", LIBCXX_CACERTS);

gnutls::credentials::certificateObj::certificateObj()
{
	init();
}

gnutls::credentials::certificateObj
::certificateObj(const std::string &certfile,
		 const std::string &keyfile,
		 gnutls_x509_crt_fmt_t format)
{
	init();

	std::list<gnutls::x509::crt> certlist;

	gnutls::x509::crt::base::import_cert_list(certlist, certfile, format);

	gnutls::x509::privkey pk(gnutls::x509::privkey::create());

	pk->import_pkcs1(keyfile, format);
	set_key(certlist, pk);
}


gnutls::credentials::certificateObj
::certificateObj(const std::list<gnutls::x509::crt> &certlist,
		 const gnutls::x509::privkey &keyfile)
{
	init();
	set_key(certlist, keyfile);
}

void gnutls::credentials::certificateObj::init()
{
	gnutls::init::gnutls_init();

	chkerr(gnutls_certificate_allocate_credentials(&cred),
	       "gnutls_certificate_allocate_credentials");

	gnutls_certificate_set_params_function(cred, &get_params_cb);
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::gnutls::credentials::certificateObj
		    ::get_params, get_paramsLog);

int gnutls::credentials::get_params_cb(gnutls_session_t session,
				       gnutls_params_type_t type,
				       gnutls_params_st *st)
{
	return gnutls::credentials::certificateObj
		::get_params_cb(session, type, st);
}


int gnutls::credentials::certificateObj::
get_params_cb(gnutls_session_t session,
	      gnutls_params_type_t type,
	      gnutls_params_st *st) noexcept
{
	LOG_FUNC_SCOPE(get_paramsLog);

	gnutls::session sess((gnutls::sessionObj *)
			     gnutls_session_get_ptr(session));

	int rc=-1;

	try {
		rc=sess->certificateCred->get_params(type, st);
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		LOG_ERROR(e);
	}
	return rc;
}

int gnutls::credentials::certificateObj::get_params(gnutls_params_type_t type,
						    gnutls_params_st *st)
{
	LOG_FUNC_SCOPE(get_paramsLog);

	switch (type) {
	case GNUTLS_PARAMS_RSA_EXPORT:
		break;
	case GNUTLS_PARAMS_DH:
		{
			dhparamsptr d=dh;

			if (d.null())
			{
				d=dhparamsptr::create();
				d->import();
			}

			dhparams dh_cpy(dhparams::create());

			dh_cpy->import_pk(d->export_pk(GNUTLS_X509_FMT_DER),
					  GNUTLS_X509_FMT_DER);

			st->type=GNUTLS_PARAMS_DH;
			st->params.dh = *dh_cpy;
			st->deinit=1;
		}
		return 0;
	default:
		LOG_ERROR(libmsg()->
			  get(_txt("Unknown temporary key parameter")));
		break;
	}
	return -1;
}

gnutls::credentials::certificateObj::~certificateObj()
{
	gnutls_certificate_free_credentials(cred);
}

void gnutls::credentials::certificateObj
::set_key(const std::list<gnutls::x509::crt> &certlistArg,
	  const gnutls::x509::privkey &certkeyArg)
{
	if (certlistArg.empty())
		throw EXCEPTION("Empty certificate list");

	std::vector<gnutls_x509_crt_t> crt_vec;

	crt_vec.reserve(certlistArg.size());

	std::list<gnutls::x509::crt>::const_iterator
		b=certlistArg.begin(),
		e=certlistArg.end();

	while (b != e)
	{
		crt_vec.push_back((*b)->crt);
		++b;
	}

	privatekeyList.push_back(certkeyArg);

	chkerr(gnutls_certificate_set_x509_key(cred,
					       &crt_vec[0],
					       crt_vec.size(),
					       certkeyArg->privkey),
	       "gnutls_certificate_set_x509_key");
}

void  gnutls::credentials::certificateObj
::set_x509_keyfile(const std::string &certfile,
		   const std::string &keyfile,
		   gnutls_x509_crt_fmt_t format)
{
	chkerr(gnutls_certificate_set_x509_key_file(cred,
						    certfile.c_str(),
						    keyfile.c_str(),
						    format),
	       "gnutls_certificate_set_x509_key_file");
}

void  gnutls::credentials::certificateObj
::set_x509_key_mem(gnutls::datum_t cert,
		   gnutls::datum_t key,
		   gnutls_x509_crt_fmt_t format)

{
	gnutls_datum_t certArg;
	gnutls_datum_t keyArg;

	certArg.data=&(*cert)[0];
	certArg.size=cert->size();

	keyArg.data=&(*key)[0];
	keyArg.size=key->size();

	chkerr(gnutls_certificate_set_x509_key_mem(cred, &certArg,
						   &keyArg, format),
	       "gnutls_certificate_set_x509_key_mem");
}

void gnutls::credentials::certificateObj
::set_x509_trust(const std::list<gnutls::x509::crt> &caList)

{
	std::vector<gnutls_x509_crt_t> caVec;

	caVec.reserve(caList.size());
	std::list<gnutls::x509::crt>::const_iterator
		b=caList.begin(), e=caList.end();

	while (b != e)
		caVec.push_back((*b++)->crt);

	chkerr(gnutls_certificate_set_x509_trust(cred, &caVec[0],
						 caVec.size()),
	       "gnutls_certificate_set_x509_trust");
}

void gnutls::credentials::certificateObj
::set_x509_trust(gnutls::datum_t datum,
		 gnutls_x509_crt_fmt_t format)
{
	gnutls_datum_t d;

	d.data=&*datum->begin();
	d.size=datum->size();

	chkerr(gnutls_certificate_set_x509_trust_mem(cred, &d, format),
	       "gnutls_certificate_set_x509_trust_mem");
}

void gnutls::credentials::certificateObj::set_x509_trust_default()

{
	set_x509_trust_file(calist.getValue(), GNUTLS_X509_FMT_PEM);
}


void gnutls::credentials::certificateObj
::set_x509_trust_file(std::string filename,
		      gnutls_x509_crt_fmt_t format)
{
	chkerr(gnutls_certificate_set_x509_trust_file(cred,
						      filename.c_str(),
						      format),
	       "gnutls_certificate_set_x509_trust_file");
}

void gnutls::credentials::certificateObj::set_verify_flags(unsigned int flags)

{
	gnutls_certificate_set_verify_flags(cred, flags);
}

void gnutls::credentials::certificateObj
::set_verify_limits(unsigned int max_bits,
		    unsigned int max_depth
		    )
{
	gnutls_certificate_set_verify_limits(cred, max_bits, max_depth);
}

void gnutls::credentials::certificateObj
::set_dh_params(const dhparams &dhArg)
{
	gnutls_certificate_set_dh_params(cred, dhArg->dh);
	dh=dhArg;
}

void gnutls::credentials::certificateObj
::set_pk_params(const pkparams &pkArg)
{
	switch (pkArg->get_pk_algorithm()) {
	case GNUTLS_PK_DSA:
		set_dh_params(pkArg);
		return;
	default:
		break;
	}

	chkerr(GNUTLS_E_UNKNOWN_PK_ALGORITHM,
	       "gnutls::credentials::certificateObj::set_pk_params");
}


void gnutls::credentials::certificateObj
::set_callback(const credentials::callback &callbackArg)

{
	certificateCallback=callbackArg;
	gnutls_certificate_set_retrieve_function(cred,
						 &gnutls::credentials::callbackObj::callback);
}

#if 0
{
#endif
};
