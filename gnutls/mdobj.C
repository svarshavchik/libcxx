/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/vector.H"
#include "x/gnutls/init.H"
#include "x/gcrypt/md.H"
#include "x/gcrypt/errors.H"
#include "x/exception.H"
#include "x/messages.H"
#include "gettext_in.h"
#include <algorithm>

namespace LIBCXX_NAMESPACE {
	namespace gcrypt {
#if 0
	};
};
#endif

#include "gcrypt_md.h"

static const gcry_md_algos gcry_algos[]={ GNUTLS_MD_ENUM };

mdBase::iterator::~iterator()
{
}

void mdBase::enumerate(std::set<std::string> &algos)
{
	gnutls::init::gnutls_init();

	algos.clear();

	for (auto a:gcry_algos)
	{
		if (gcry_md_test_algo(a) == 0)
			algos.insert(name(a));
	}
}

std::string mdBase::name(gcry_md_algos algo)
{
	gnutls::init::gnutls_init();

	return gcry_md_algo_name(algo);
}

gcry_md_algos mdBase::number(const std::string &name)
{
	gnutls::init::gnutls_init();

	auto n=(gcry_md_algos)gcry_md_map_name(name.c_str());

	if (n == GCRY_MD_NONE)
		throw EXCEPTION(gettextmsg(libmsg()->
					   get(_txt("Unknown message digest algorithm \"%1%\"")), name));

	return n;
}

vector<unsigned char> mdBase::iterator::digest(gcry_md_algos algorithm)
{
	return ( (*this)->digest(algorithm));
}

std::string mdBase::iterator::hexdigest(gcry_md_algos algorithm)
{
	return ( (*this)->hexdigest(algorithm));
}

mdObj::mdObj(gcry_md_algos algorithmArg, unsigned int flags)
	: h(0), algorithm(algorithmArg)
{
	gnutls::init::gnutls_init();

	chkerr(gcry_md_open(&h, algorithm, flags));
}

mdObj::mdObj(const std::string &algorithmArg, unsigned int flags)
	: h(0), algorithm(md::base::number(algorithmArg))
{
	gnutls::init::gnutls_init();

	chkerr(gcry_md_open(&h, algorithm, flags));
}

mdObj::mdObj(const const_md &src) : h(0), algorithm(src->algorithm)
{
	chkerr(gcry_md_copy(&h, src->h));
}

mdObj::~mdObj()
{
	gcry_md_close(h);
}

void mdObj::enable(gcry_md_algos extraalgorithm)
{
	chkerr(gcry_md_enable(h, extraalgorithm));
}
void mdObj::enable(const std::string &extraalgorithm)
{
	enable(md::base::number(extraalgorithm));
}

void mdObj::setkey(const void *key, size_t keylen)
{
	chkerr(gcry_md_setkey(h, key, keylen));
}

void mdObj::write(const void *key, size_t keylen)
{
	gcry_md_write(h, key, keylen);
}

void mdObj::digest(std::vector<unsigned char> &buffer,
		   gcry_md_algos explicitalgorithm)
{
	unsigned char *p=gcry_md_read(h, explicitalgorithm);

	if (!p)
		throw EXCEPTION(libmsg()->
				get(_txt("Requested message digest not available")));

	if (explicitalgorithm == GCRY_MD_NONE)
		explicitalgorithm=algorithm;

	if (explicitalgorithm == GCRY_MD_NONE)
		throw EXCEPTION(libmsg()->
				get(_txt("Message digest number must be explicitly given to digest()")));

	buffer.resize(gcry_md_get_algo_dlen(explicitalgorithm));

	std::copy(p, p+buffer.size(), buffer.begin());
}

vector<unsigned char> mdObj::digest(gcry_md_algos algorithm)
{
	auto v=vector<unsigned char>::create();

	digest(*v, algorithm);

	return v;
}

std::string mdObj::hexdigest(gcry_md_algos algorithm)
{
	std::vector<unsigned char> tmp_buf;

	static const char hex[]="0123456789abcdef";

	digest(tmp_buf, algorithm);

	std::string buffer;

	buffer.clear();
	buffer.reserve(tmp_buf.size()*2);

	for (auto byte:tmp_buf)
	{
		buffer.push_back(hex[(unsigned char)byte >> 4]);
		buffer.push_back(hex[byte & 0x0f]);
	}

	return buffer;
}

#if 0
{
	{
#endif
	};
};

