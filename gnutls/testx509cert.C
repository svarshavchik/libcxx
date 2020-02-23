/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/gnutls/x509_crt.H"
#include "x/gnutls/datumwrapper.H"
#include "x/property_properties.H"
#include "x/exception.H"

#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <iomanip>

#include <ctime>

static void testcert()
{
	LIBCXX_NAMESPACE::gnutls::x509::privkey cakey(LIBCXX_NAMESPACE::gnutls::x509::privkey::create());

	{
		LIBCXX_NAMESPACE::gnutls::datum_t key(LIBCXX_NAMESPACE::gnutls::datum_t::create());

		key->load("testrsa1.key");
		cakey->import_pkcs1(key, GNUTLS_X509_FMT_PEM);
	}

	time_t current_time=time(NULL);

	LIBCXX_NAMESPACE::gnutls::x509::crt cacert(LIBCXX_NAMESPACE::gnutls::x509::crt::create());

	cacert->set_activation_time(current_time);
	cacert->set_basic_constraints(true);
	cacert->set_dn_by_oid(GNUTLS_OID_X520_ORGANIZATION_NAME,
			      "libx library");
	cacert->set_dn_by_oid(GNUTLS_OID_X520_ORGANIZATIONAL_UNIT_NAME,
			      "GnuTLS wrapper");
	cacert->set_dn_by_oid(GNUTLS_OID_X520_COMMON_NAME,
			      "example.com");
	cacert->set_expiration_time(current_time + 60 * 60 * 24 * 400L);

	cacert->set_key_usage(GNUTLS_KEY_DIGITAL_SIGNATURE|
			      GNUTLS_KEY_NON_REPUDIATION|
			      GNUTLS_KEY_KEY_CERT_SIGN|
			      GNUTLS_KEY_KEY_AGREEMENT|
			      GNUTLS_KEY_KEY_ENCIPHERMENT|
			      GNUTLS_KEY_CRL_SIGN);
	cacert->set_key(cakey);
	cacert->set_key_purpose_oid(GNUTLS_KP_TLS_WWW_SERVER, false);
	cacert->set_key_purpose_oid(GNUTLS_KP_CODE_SIGNING, false);
	cacert->set_key_purpose_oid(GNUTLS_KP_EMAIL_PROTECTION, false);
	cacert->set_serial(1UL);

	cacert->set_subject_alternative_name(GNUTLS_SAN_DNSNAME,
					     "example.com");
	cacert->set_subject_key_id();
	cacert->set_version();
	cacert->sign(cacert, cakey);

	std::cout << cacert->print();

	cacert->export_cert("testrsa1.crt", GNUTLS_X509_FMT_PEM);

	std::cout << std::endl;

	{
		time_t t=cacert->get_activation_time();
		char buf[256];

		strftime(buf, sizeof(buf), "%c", gmtime(&t));

		std::cout << "Activation time: " << buf << std::endl;
	}

	{
		time_t t=cacert->get_expiration_time();
		char buf[256];

		strftime(buf, sizeof(buf), "%c", gmtime(&t));

		std::cout << "Expiration time: " << buf << std::endl;
	}

	{
		bool ca, critical;
		int pathLen;

		cacert->get_basic_constraints(ca, critical, pathLen);

		std::cout << "CA: " << ca << ", len=" << pathLen << std::endl;
	}

	std::cout << "DN: " << cacert->get_dn() << std::endl;

	{
		std::list<LIBCXX_NAMESPACE::gnutls::datum_t> cn;

		cacert->get_dn_by_oid(GNUTLS_OID_X520_COMMON_NAME, cn);

		if (cn.empty())
			throw EXCEPTION("No DN?");

		std::cout << "commonName: ";

		std::copy(cn.front()->begin(),
			  cn.front()->end(),
			  std::ostream_iterator<char>(std::cout));
		std::cout << std::endl;
	}

	{
		std::list<std::string> all_dns;

		cacert->get_all_dns(all_dns);

		std::cout << "DNs: ";

		std::copy(all_dns.begin(), all_dns.end(),
			  std::ostream_iterator<std::string>(std::cout, " "));
		std::cout << std::endl;
	}

	{
		std::list< std::pair<std::string, bool> > extensions;

		cacert->get_all_extensions_list(extensions);

		std::list< std::pair<std::string, bool> >::iterator
			b=extensions.begin(),
			e=extensions.end();

		while (b != e)
		{
			std::cout << "Extension: " << b->first
				  << " (critical=" << b->second << ")"
				  << std::endl;
			++b;
		}
	}

	{
		LIBCXX_NAMESPACE::gnutls::datum_t fingerprint(cacert->get_fingerprint());

		std::cout << "Fingerprint: "
			  << std::hex;

		std::copy(fingerprint->begin(),
			  fingerprint->end(),
			  std::ostream_iterator<int>(std::cout, ":"));
		std::cout << std::dec << std::endl;
	}
	std::cout << "Issuer DN: " << cacert->get_issuer_dn() << std::endl;

	{
		std::list<LIBCXX_NAMESPACE::gnutls::datum_t> cn;

		cacert->get_issuer_dn_by_oid(GNUTLS_OID_X520_COMMON_NAME, cn);

		if (cn.empty())
			throw EXCEPTION("No DN?");

		std::cout << "Issuer commonName: ";

		std::copy(cn.front()->begin(),
			  cn.front()->end(),
			  std::ostream_iterator<char>(std::cout));
		std::cout << std::endl;
	}

	{
		bool critical;

		std::cout << "Key usage: " << std::setw(8) << std::setfill('0')
			  << std::hex << cacert->get_key_usage(critical)
			  << std::dec << std::endl;
	}

	{
		unsigned int bits;

		std::cout << "Algorithm: "
			  << gnutls_pk_algorithm_get_name(cacert
							  ->get_pk_algorithm
							  (bits));
		std::cout << " (" << bits << " bits)/"
			  << gnutls_sign_algorithm_get_name
			(cacert
			 ->get_signature_algorithm()) << std::endl;

	}

	std::cout << "Serial: " << cacert->get_serial() << std::endl;

	cacert->get_signature();

#if 0
	{
		std::list<std::pair<gnutls_x509_subject_alt_name_t,
			LIBCXX_NAMESPACE::gnutls::datum_t> > altlist;
		bool critical;

		cacert->get_subject_alternative_names(altlist, critical);

		std::list<std::pair<gnutls_x509_subject_alt_name_t,
			LIBCXX_NAMESPACE::gnutls::datum_t> >
			::iterator b=altlist.begin(), e=altlist.end();

		while (b != e)
		{
			std::cout << "altname: ";
			std::copy(b->second->begin(),
				  b->second->end(),
				  std::ostream_iterator<char>(std::cout));
			std::cout << std::endl;
			++b;
		}
	}
#endif
	std::cout << "Version: " << cacert->get_version() << std::endl;

	LIBCXX_NAMESPACE::gnutls::x509::privkey icertkey(LIBCXX_NAMESPACE::gnutls::x509::privkey::create());

	{
		LIBCXX_NAMESPACE::gnutls::datum_t key(LIBCXX_NAMESPACE::gnutls::datum_t::create());

		key->load("testrsa2.key");
		icertkey->import_pkcs1(key, GNUTLS_X509_FMT_PEM);
	}

	LIBCXX_NAMESPACE::gnutls::x509::crt icert(LIBCXX_NAMESPACE::gnutls::x509::crt::create());

	icert->set_activation_time(current_time);
	icert->set_basic_constraints(true, 1);
	icert->set_dn_by_oid(GNUTLS_OID_X520_ORGANIZATION_NAME,
			      "libx library");
	icert->set_dn_by_oid(GNUTLS_OID_X520_ORGANIZATIONAL_UNIT_NAME,
			      "GnuTLS wrapper");
	icert->set_dn_by_oid(GNUTLS_OID_X520_COMMON_NAME,
			      "subcert.example.com");
	icert->set_expiration_time(current_time + 60 * 60 * 24 * 400L);

	icert->set_key_usage(GNUTLS_KEY_DIGITAL_SIGNATURE|
			     GNUTLS_KEY_NON_REPUDIATION|
			     GNUTLS_KEY_KEY_CERT_SIGN|
			     GNUTLS_KEY_KEY_AGREEMENT|
			     GNUTLS_KEY_CRL_SIGN);
	icert->set_key(icertkey);
	icert->set_key_purpose_oid(GNUTLS_KP_TLS_WWW_SERVER, false);
	icert->set_key_purpose_oid(GNUTLS_KP_CODE_SIGNING, false);
	icert->set_key_purpose_oid(GNUTLS_KP_EMAIL_PROTECTION, false);
	//icert->set_serial(1UL);

	icert->set_subject_key_id();
	icert->set_version();
	icert->sign(cacert, cakey);
	std::cout << "icert: " << std::endl << icert->print();

	icert->export_cert("testrsa2.crt", GNUTLS_X509_FMT_PEM);

	{
		std::list<LIBCXX_NAMESPACE::gnutls::x509::crt> certlist, calist;

		certlist.push_back(icert);
		calist.push_back(cacert);

		std::cout << "Verification: "
			  << LIBCXX_NAMESPACE::gnutls::x509::crt::base::verify_cert_list(certlist,
								    calist)
			  << std::endl;
	}

	LIBCXX_NAMESPACE::gnutls::x509::privkey hcertkey(LIBCXX_NAMESPACE::gnutls::x509::privkey::create());

	{
		LIBCXX_NAMESPACE::gnutls::datum_t key(LIBCXX_NAMESPACE::gnutls::datum_t::create());

		key->load("testrsa3.key");
		hcertkey->import_pkcs1(key, GNUTLS_X509_FMT_PEM);
	}

	LIBCXX_NAMESPACE::gnutls::x509::crt hcert(LIBCXX_NAMESPACE::gnutls::x509::crt::create());

	hcert->set_activation_time(current_time);
	hcert->set_basic_constraints(false);
	hcert->set_dn_by_oid(GNUTLS_OID_X520_ORGANIZATION_NAME,
			      "libx library");
	hcert->set_dn_by_oid(GNUTLS_OID_X520_ORGANIZATIONAL_UNIT_NAME,
			      "GnuTLS wrapper");
	hcert->set_dn_by_oid(GNUTLS_OID_X520_COMMON_NAME,
			      "*.domain.com");
	hcert->set_expiration_time(current_time + 60 * 60 * 24 * 400L);

	hcert->set_key_usage(GNUTLS_KEY_DIGITAL_SIGNATURE|
			     GNUTLS_KEY_NON_REPUDIATION|
			     GNUTLS_KEY_KEY_AGREEMENT);

	hcert->set_key(hcertkey);
	hcert->set_key_purpose_oid(GNUTLS_KP_TLS_WWW_SERVER, false);
	hcert->set_key_purpose_oid(GNUTLS_KP_TLS_WWW_CLIENT, false);
	hcert->set_serial(1UL);

	hcert->set_subject_key_id();
	hcert->set_version();
	hcert->sign(icert, icertkey);
	std::cout << "hcert: " << std::endl << hcert->print();

	hcert->export_cert(GNUTLS_X509_FMT_PEM)->save("testrsa3.crt");

	{
		LIBCXX_NAMESPACE::fd
			hcertfile(LIBCXX_NAMESPACE::fd::base
				  ::open("testrsa3i.crt",
					 O_WRONLY | O_CREAT | O_TRUNC));

		icert->export_cert(GNUTLS_X509_FMT_PEM)->save(hcertfile);
		hcert->export_cert(GNUTLS_X509_FMT_PEM)->save(hcertfile);
	}

	if (!hcert->check_hostname("hosta.domain.com") ||
	    hcert->check_hostname("example.com"))
		throw EXCEPTION("check_hostname failed self-test");
}

static void testcert2()
{
	std::list<LIBCXX_NAMESPACE::gnutls::x509::crt> calist;

	LIBCXX_NAMESPACE::gnutls::x509::crt::base::import_cert_list(calist, "testrsa1.crt",
					       GNUTLS_X509_FMT_PEM);

	std::list<LIBCXX_NAMESPACE::gnutls::x509::crt> ilist;

	LIBCXX_NAMESPACE::gnutls::x509::crt::base::import_cert_list(ilist, "testrsa2.crt",
					       GNUTLS_X509_FMT_PEM);

	std::list<LIBCXX_NAMESPACE::gnutls::x509::crt> hlist;

	LIBCXX_NAMESPACE::gnutls::x509::crt::base::import_cert_list(hlist, "testrsa3.crt",
					       GNUTLS_X509_FMT_PEM);

	std::list<LIBCXX_NAMESPACE::gnutls::x509::crt> hilist;

	LIBCXX_NAMESPACE::gnutls::x509::crt::base::import_cert_list(hilist, "testrsa3i.crt",
					       GNUTLS_X509_FMT_PEM);

	if (LIBCXX_NAMESPACE::gnutls::x509::crt::base::verify_cert_list(ilist, calist) != 0)
		throw EXCEPTION("ilist failed to verify");

	if (LIBCXX_NAMESPACE::gnutls::x509::crt::base::verify_cert_list(hlist, calist) == 0)
		throw EXCEPTION("hlist improperly verified");

	if (LIBCXX_NAMESPACE::gnutls::x509::crt::base::verify_cert_list(hilist, calist) != 0)
		throw EXCEPTION("hilist failed to verify");
}

int main(int argc, char **argv)
{
	try {
		std::vector<std::string> l;
		testcert();
		testcert2();
	} catch (LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
	}
	return 0;
}

