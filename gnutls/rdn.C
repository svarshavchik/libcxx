/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/gnutls/rdn.H"
#include "x/gnutls/init.H"

#include <gnutls/x509.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

std::string gnutls::rdn::raw_get(const gnutls_datum_t &datum)
{
	std::vector<char> buf;
	size_t buf_size=0;

	int err=gnutls_x509_rdn_get(&datum, NULL, &buf_size);

	if (err == GNUTLS_E_SHORT_MEMORY_BUFFER)
	{
		buf.resize(buf_size+1);
		err=gnutls_x509_rdn_get(&datum, &buf[0], &buf_size);
	}
	chkerr(err, "gnutls_x509_rdn_get_oid");

	buf.resize(buf_size);
	buf.push_back(0);

	return &buf[0];
}

bool gnutls::rdn::raw_get_oid_internal(const gnutls_datum_t &datum,
				       size_t i,
				       std::vector<char> &oid)
{
	size_t oid_s=0;

	int err=gnutls_x509_rdn_get_oid(&datum, i, NULL, &oid_s);

	if (err == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
		return false;

	if (err == GNUTLS_E_SHORT_MEMORY_BUFFER)
	{
		oid.resize(oid_s+1);
		err=gnutls_x509_rdn_get_oid(&datum, i, &oid[0], &oid_s);
	}
	chkerr(err, "gnutls_x509_rdn_get_oid");
	oid.resize(oid_s);
	oid.push_back(0);
	return true;
}

bool gnutls::rdn::raw_get_oid_internal(const gnutls_datum_t &datum,
				       size_t i,
				       const char *oid,
				       std::vector<char> &value,
				       size_t &value_s,
				       bool raw)

{
	value_s=0;

	int err=gnutls_x509_rdn_get_by_oid(&datum, oid, i, raw, NULL, &value_s);

	if (err == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
		return false;

	if (err == GNUTLS_E_SHORT_MEMORY_BUFFER)
	{
		value.resize(value_s+1);
		err=gnutls_x509_rdn_get_by_oid(&datum, oid, i, raw,
					       &value[0], &value_s);
	}

	chkerr(err, "gnutls_x509_rdn_get_by_oid");
	value.resize(value_s);
	return true;
}

template
void gnutls::rdn::raw_get_oid(const gnutls_datum_t &datum,
			      std::set<std::string> &oid);
template
void gnutls::rdn::raw_get_oid(const gnutls_datum_t &datum,
			      std::vector<std::string> &oid);

template
void gnutls::rdn::raw_get_oid(const gnutls_datum_t &datum,
			      std::list<std::string> &oid);

template
void gnutls::rdn::raw_get_oid(const gnutls_datum_t &datum,
			      std::set<std::string> &values,
			      const std::string &oid,
			      bool raw);
template
void gnutls::rdn::raw_get_oid(const gnutls_datum_t &datum,
			      std::vector<std::string> &values,
			      const std::string &oid,
			      bool raw);
template
void gnutls::rdn::raw_get_oid(const gnutls_datum_t &datum,
			      std::list<std::string> &values,
			      const std::string &oid,
			      bool raw);

template
void gnutls::rdn::raw_get_oid(const gnutls_datum_t &datum,
			      std::set<std::string> &values,
			      const char * const &oid,
			      bool raw);
template
void gnutls::rdn::raw_get_oid(const gnutls_datum_t &datum,
			      std::vector<std::string> &values,
			      const char * const &oid,
			      bool raw);
template
void gnutls::rdn::raw_get_oid(const gnutls_datum_t &datum,
			      std::list<std::string> &values,
			      const char * const &oid,
			      bool raw);

#if 0
{
#endif
};
