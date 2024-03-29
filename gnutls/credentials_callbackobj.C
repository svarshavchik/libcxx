/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/gnutls/sessionobj.H"
#include "x/gnutls/credentials_callbackobj.H"
#include "x/gnutls/datumwrapper.H"
#include "x/gnutls/init.H"
#include "x/sysexception.H"
#include <iostream>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

gnutls::credentials::callbackObj::keycertObj
::keycertObj(const x509::privkey &keyArg) : key(keyArg)
{
}

gnutls::credentials::callbackObj::keycertObj
::~keycertObj()
{
}

gnutls::credentials::callbackObj::callbackObj() noexcept
: captured_exception(false)
{
}

gnutls::credentials::callbackObj::~callbackObj()
{
}

int gnutls::credentials::callbackObj::callback(gnutls_session_t sess,
					       const gnutls_datum_t *req_ca_dn,
					       int nreqs,
					       const gnutls_pk_algorithm_t
					       *pk_algos,
					       int pk_algos_length,
					       gnutls_retr2_st* st)
{
	std::list<std::string> hostname_list;

	st->cert_type=GNUTLS_CRT_UNKNOWN;
	st->key_type=GNUTLS_PRIVKEY_X509;
	st->ncerts=0;
	st->deinit_all=0;

	credentials::callbackptr cb;

	try {
		gnutls::sessionObj *sessObj=(gnutls::sessionObj *)
			gnutls_session_get_ptr(sess);

		if (sessObj->mode == GNUTLS_SERVER)
			sessionObj::get_server_names(sess, hostname_list);

		cb=sessObj->certificateCred->certificateCallback;

		std::vector<gnutls_pk_algorithm_t> algos;

		algos.reserve(pk_algos_length);
		if (pk_algos)
			algos.insert(algos.end(), pk_algos,
				     pk_algos+pk_algos_length);


		auto certkeyptr=
			cb->get(sessObj, hostname_list, algos, req_ca_dn,
				nreqs);

		if (!certkeyptr.null())
		{
			int rc;

			gnutls::datum_t keyDatumObj=certkeyptr->key
				->export_pkcs1(GNUTLS_X509_FMT_DER);

			gnutls_datum_t keyDatum;

			keyDatum.data=&*keyDatumObj->begin();
			keyDatum.size=keyDatumObj->size();

			gnutls_x509_privkey_t privkey;

			chkerr(gnutls_x509_privkey_init(&privkey),
			       "gnutls_x509_privkey_init");

			rc=gnutls_x509_privkey_import(privkey, &keyDatum,
						      GNUTLS_X509_FMT_DER);
			if (rc)
			{
				gnutls_x509_privkey_deinit(privkey);
				chkerr(rc, "gnutls_x509_privkey_import");
			}

			try {
				st->cert_type=GNUTLS_CRT_X509;
				st->cert.x509=init_certs(certkeyptr->cert);
			}
			catch (...)
			{
				gnutls_x509_privkey_deinit(privkey);
				throw;
			}

			st->ncerts=certkeyptr->cert.size();
			st->key.x509=privkey;
			st->deinit_all=1;
		}

	} catch (const sysexception &e)
	{
		if (!cb.null())
		{
			auto &cbref= *cb;

			cbref.saved_exception=e;
			cbref.captured_exception=true;
		}
	} catch (const exception &e)
	{
		if (!cb.null())
		{
			auto &cbref= *cb;

			cbref.saved_exception=e;
			cbref.captured_exception=true;
		}
		return -1;
	}

	return 0;
}

gnutls_x509_crt_t *gnutls::credentials::callbackObj
::init_certs(std::list<x509::crt> &x509_cert)

{
	gnutls_x509_crt_t *certs=(gnutls_x509_crt_t *)
		gnutls_malloc(sizeof(gnutls_x509_crt_t) *
			      x509_cert.size());

	size_t n=0;

	try {
		std::list<x509::crt>::iterator
			ccb=x509_cert.begin(),
			cce=x509_cert.end();

		while (ccb != cce)
		{
			datum_t d=(*ccb)->export_cert(GNUTLS_X509_FMT_DER);
			++ccb;

			gnutls_datum_t dd;

			dd.data=&*d->begin();
			dd.size=d->size();

			chkerr(gnutls_x509_crt_init(&certs[n]),
			       "gnutls_x509_crt_init");
			++n;
			chkerr(gnutls_x509_crt_import(certs[n-1], &dd,
						      GNUTLS_X509_FMT_DER),
			       "gnutls_x509_crt_import");
		}
	}
	catch (...)
	{
		while (n)
		{
			gnutls_x509_crt_deinit(certs[--n]);
		}
		gnutls_free(certs);
		throw;
	}
	return certs;
}

#if 0
{
#endif
};
