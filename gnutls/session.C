/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "gnutls/session.H"
#include "gnutls/init.H"

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

void gnutls::sessionBase
::get_cipher_list(std::list<gnutls_cipher_algorithm_t> &listRet) noexcept
{
	const gnutls_cipher_algorithm_t *a=gnutls_cipher_list();

	while (*a)
		listRet.push_back(*a++);
}

std::string gnutls::sessionBase::get_cipher_name(gnutls_cipher_algorithm_t a)
{
	const char *n=gnutls_cipher_get_name(a);

	if (!n)
		throw EXCEPTION("Unknown algorithm id");

	return n;
}

gnutls_cipher_algorithm_t gnutls::sessionBase::get_cipher_id(std::string name)
{
	gnutls_cipher_algorithm_t a=gnutls_cipher_get_id(name.c_str());

	if (a == GNUTLS_CIPHER_UNKNOWN)
		throw EXCEPTION("Unknown algorithm name: " + name);

	return a;
}

void gnutls::sessionBase
::get_compression_list(std::list<gnutls_compression_method_t>
		       &listRet) noexcept
{
	const gnutls_compression_method_t *a=gnutls_compression_list();

	while (*a)
		listRet.push_back(*a++);
}


std::string gnutls::sessionBase
::get_compression_name(gnutls_compression_method_t a)
{
	const char *n=gnutls_compression_get_name(a);

	if (!n)
		throw EXCEPTION("Unknown compression method");

	return n;
}

gnutls_compression_method_t
gnutls::sessionBase::get_compression_id(std::string name)
{
	gnutls_compression_method_t n=gnutls_compression_get_id(name.c_str());

	if (n == GNUTLS_COMP_UNKNOWN)
		throw EXCEPTION("Unknown compression name: " + name);
	return n;
}

void gnutls::sessionBase::get_kx_list(std::list<gnutls_kx_algorithm_t>
				      &listRet) noexcept
{
	const gnutls_kx_algorithm_t *a=gnutls_kx_list();

	while (*a)
		listRet.push_back(*a++);
}


std::string gnutls::sessionBase::get_kx_name(gnutls_kx_algorithm_t a)
{
	const char *n=gnutls_kx_get_name(a);

	if (!n)
		throw EXCEPTION("Unknown key exchange algorithm");

	return n;
}

gnutls_kx_algorithm_t gnutls::sessionBase::get_kx_id(std::string name)
{
	gnutls_kx_algorithm_t n=gnutls_kx_get_id(name.c_str());

	if (n == GNUTLS_KX_UNKNOWN)
		throw EXCEPTION("Unknown key exchange algorithm name: " + name);
	return n;
}

void gnutls::sessionBase::get_mac_list(std::list<gnutls_mac_algorithm_t>
				       &listRet) noexcept
{
	const gnutls_mac_algorithm_t *a=gnutls_mac_list();

	while (*a)
		listRet.push_back(*a++);
}

std::string gnutls::sessionBase::get_mac_name(gnutls_mac_algorithm_t a)
{
	const char *n=gnutls_mac_get_name(a);

	if (!n)
		throw EXCEPTION("Unknown key exchange algorithm");

	return n;
}

gnutls_mac_algorithm_t gnutls::sessionBase::get_mac_id(std::string name)
{
	gnutls_mac_algorithm_t n=gnutls_mac_get_id(name.c_str());

	if (n == GNUTLS_MAC_UNKNOWN)
		throw EXCEPTION("Unknown hash function name: " + name);
	return n;
}

void gnutls::sessionBase::get_protocol_list(std::list<gnutls_protocol_t>
					    &listRet) noexcept
{
	const gnutls_protocol_t *a=gnutls_protocol_list();

	while (*a)
		listRet.push_back(*a++);
}

std::string gnutls::sessionBase::get_protocol_name(gnutls_protocol_t a)

{
	const char *n=gnutls_protocol_get_name(a);

	if (!n)
		throw EXCEPTION("Unknown protocol");

	return n;
}

gnutls_protocol_t gnutls::sessionBase::get_protocol_id(std::string name)
{
	gnutls_protocol_t n=gnutls_protocol_get_id(name.c_str());

	if (n == GNUTLS_VERSION_UNKNOWN)
		throw EXCEPTION("Unknown protocol name: " + name);
	return n;
}

void gnutls::sessionBase
::get_certificate_list(std::list<gnutls_certificate_type_t>
		       &listRet) noexcept
{
	const gnutls_certificate_type_t *a=gnutls_certificate_type_list();

	while (*a)
		listRet.push_back(*a++);
}

std::string gnutls::sessionBase
::get_certificate_name(gnutls_certificate_type_t a)

{
	const char *n=gnutls_certificate_type_get_name(a);

	if (!n)
		throw EXCEPTION("Unknown certificate type");

	return n;
}

gnutls_certificate_type_t gnutls::sessionBase::
get_certificate_id(std::string name)

{
	gnutls_certificate_type_t n=
		gnutls_certificate_type_get_id(name.c_str());

	if (n == GNUTLS_CRT_UNKNOWN)
		throw EXCEPTION("Unknown certificate type: " + name);
	return n;
}
#if 0
{
#endif
};
