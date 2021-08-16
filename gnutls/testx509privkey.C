/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/property_properties.H"
#include "x/gnutls/x509_privkey.H"
#include "x/gnutls/dhparams.H"
#include "x/exception.H"

#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>

void foo()
{
	std::vector<std::string> labels;

	LIBCXX_NAMESPACE::gnutls::sec_param::enumerate(labels);
}

static LIBCXX_NAMESPACE::gnutls::x509::privkey testrsa()
{
	std::string weak="medium";
	LIBCXX_NAMESPACE::gnutls::sec_param
		weak_param(LIBCXX_NAMESPACE::gnutls::sec_param
			   ::from_string(weak.begin(), weak.end()));

	LIBCXX_NAMESPACE::gnutls::x509::privkey
		pk=LIBCXX_NAMESPACE::gnutls::x509::privkey::create();

	std::cout << "Generating RSA key, this could take a while..." << std::endl << std::flush;

	{
		LIBCXX_NAMESPACE::gnutls::progress_notifier notifier;

		pk->generate("rsa", weak);
	}
	pk->fix();

	std::cout << "Key algorithm: " << pk->get_pk_algorithm() << std::endl;

	LIBCXX_NAMESPACE::gnutls::datum_t key=pk->export_pkcs1(GNUTLS_X509_FMT_DER),
		origpem=pk->export_pkcs1(GNUTLS_X509_FMT_PEM);

	origpem->save("testrsa1.key");

	{
		LIBCXX_NAMESPACE::gnutls::x509::privkey pk(LIBCXX_NAMESPACE::gnutls::x509::privkey::create());

		std::cout << "Generating 2nd RSA key, this could take a while..."
			  << std::endl << std::flush;

		LIBCXX_NAMESPACE::gnutls::progress_notifier notifier;

		pk->generate(GNUTLS_PK_RSA, weak_param);
		pk->fix();

		pk->export_pkcs1("testrsa2.key", GNUTLS_X509_FMT_PEM);
	}
	{
		LIBCXX_NAMESPACE::gnutls::x509::privkey pk(LIBCXX_NAMESPACE::gnutls::x509::privkey::create());

		std::cout << "Generating 3rd RSA key, this could take a while..."
			  << std::endl << std::flush;

		LIBCXX_NAMESPACE::gnutls::progress_notifier notifier;

		pk->generate(GNUTLS_PK_RSA, x::to_string(weak_param));
		pk->fix();

		LIBCXX_NAMESPACE::gnutls::datum_t
			origpem=pk->export_pkcs1(GNUTLS_X509_FMT_PEM);

		origpem->save("testrsa3.key");
	}

	LIBCXX_NAMESPACE::gnutls::x509::privkey pk2(LIBCXX_NAMESPACE::gnutls::x509::privkey::create());

	pk2->import_pkcs1(key, GNUTLS_X509_FMT_DER);

	key=pk2->export_pkcs1(GNUTLS_X509_FMT_PEM);

	std::cout.write((const char *)&(*key)[0], key->size());

	key=pk2->export_pkcs8(GNUTLS_X509_FMT_DER, "foo",
			      GNUTLS_PKCS8_USE_PKCS12_ARCFOUR);

	LIBCXX_NAMESPACE::gnutls::x509::privkey pk3(LIBCXX_NAMESPACE::gnutls::x509::privkey::create());

	pk3->import_pkcs8(key,GNUTLS_X509_FMT_DER, "foo",
			      GNUTLS_PKCS8_USE_PKCS12_ARCFOUR);

	LIBCXX_NAMESPACE::gnutls::privkey rawprivkey=pk3->export_raw();

	std::cout << "Key algorithm: " << rawprivkey->get_pk_algorithm()
		  << std::endl;

	LIBCXX_NAMESPACE::gnutls::x509::privkey pk4(LIBCXX_NAMESPACE::gnutls::x509::privkey::create());

	pk4->import_raw(rawprivkey);

	key=pk4->export_pkcs8(GNUTLS_X509_FMT_PEM, "foo",
			      GNUTLS_PKCS8_USE_PKCS12_ARCFOUR);

	std::cout.write((const char *)&(*key)[0], key->size());

	key=pk4->export_pkcs1(GNUTLS_X509_FMT_PEM);

	if (key->size() != origpem->size() ||
	    !std::equal(key->begin(), key->end(), origpem->begin()))
		throw EXCEPTION("Key got corrupted");

	return pk;
}

static LIBCXX_NAMESPACE::gnutls::x509::privkey testdsa()
{
	LIBCXX_NAMESPACE::gnutls::x509::privkey
		pk=LIBCXX_NAMESPACE::gnutls::x509::privkey::create();

	std::cout << "Generating DSA key...\n";

	{
		LIBCXX_NAMESPACE::gnutls::progress_notifier notifier;

		pk->generate("dsa", "weak");
	}
	pk->fix();
	std::cout << "Key algorithm: " << pk->get_pk_algorithm() << std::endl;

	LIBCXX_NAMESPACE::gnutls::datum_t key=pk->export_pkcs1(GNUTLS_X509_FMT_DER),
		origpem=pk->export_pkcs1(GNUTLS_X509_FMT_PEM);

	LIBCXX_NAMESPACE::gnutls::x509::privkey pk2(LIBCXX_NAMESPACE::gnutls::x509::privkey::create());

	pk2->import_pkcs1(key, GNUTLS_X509_FMT_DER);

	key=pk2->export_pkcs1(GNUTLS_X509_FMT_PEM);

	std::cout.write((const char *)&(*key)[0], key->size());
	LIBCXX_NAMESPACE::gnutls::x509::privkey pk3(pk2);

	LIBCXX_NAMESPACE::gnutls::privkey rawprivkey=pk3->export_raw();

	std::cout << "Key algorithm: " << rawprivkey->get_pk_algorithm()
		  << std::endl;

	LIBCXX_NAMESPACE::gnutls::x509::privkey pk4(LIBCXX_NAMESPACE::gnutls::x509::privkey::create());

	pk4->import_raw(rawprivkey);

	key=pk4->export_pkcs1(GNUTLS_X509_FMT_PEM);

	if (key->size() != origpem->size() ||
	    !std::equal(key->begin(), key->end(), origpem->begin()))
		throw EXCEPTION("Key got corrupted");

	std::cout << "DSA key ID size: " << pk4->get_key_id()->size()
		  << std::endl;

	return pk;
}

static void createparams(LIBCXX_NAMESPACE::gnutls::x509::privkey &rsakey,
			 LIBCXX_NAMESPACE::gnutls::x509::privkey &dsakey)
{

	LIBCXX_NAMESPACE::gnutls::pkparams dh=dsakey->get_pkparams();

	std::cout << "Generating DH parameters, this could take a while..." << std::endl;

	{
		LIBCXX_NAMESPACE::gnutls::progress_notifier notifier;

		dh->generate(2048);
	}

	{
		LIBCXX_NAMESPACE::gnutls::dhparams dh2(LIBCXX_NAMESPACE::gnutls::dhparams::create());

		dh2->import_pk(dh->export_pk(GNUTLS_X509_FMT_DER),
			       GNUTLS_X509_FMT_DER);

		dh2->export_pk(GNUTLS_X509_FMT_PEM)->save("dhparams.dat");

		LIBCXX_NAMESPACE::gnutls::pkparams
			dh3(LIBCXX_NAMESPACE::gnutls::pkparams
			    ::create(dh2->get_pk_algorithm()));
		std::vector<LIBCXX_NAMESPACE::gnutls::datum_t> datums;
		unsigned int nbits;

		dh2->export_raw(datums, nbits);
		dh3->import_raw(datums);

		LIBCXX_NAMESPACE::gnutls::pkparams
			dh4(LIBCXX_NAMESPACE::gnutls::pkparams
			    ::create(dh3->get_pk_algorithm()));

		dh3->export_raw(datums, nbits);
		dh4->import_raw(datums);

		LIBCXX_NAMESPACE::gnutls::datum_t a(dh3->export_pk(GNUTLS_X509_FMT_DER)),
			b(dh4->export_pk(GNUTLS_X509_FMT_DER));

		if (a->size() != b->size() ||
		    !std::equal(a->begin(), a->end(), b->begin()))
			throw EXCEPTION("dh export/import raw() test failed");
	}
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::gnutls::x509::privkey rsakey=testrsa(),
			dsakey=testdsa();

		rsakey=rsakey->export_raw()->import_raw();
		dsakey=dsakey->export_raw()->import_raw();
		createparams(rsakey, dsakey);
	} catch (LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}

