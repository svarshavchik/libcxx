/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/gcrypt/md.H"
#include "x/options.H"
#include "x/http/useragent.H"
#include "x/join.H"
#include "x/vector.H"

#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace LIBCXX_NAMESPACE;

void testmd()
{
	std::string message_digest="message digest";

	auto v=std::copy(message_digest.begin(), message_digest.end(),
			 gcrypt::md::base::iterator("md5")).digest();

	if (std::vector<unsigned char>({
				0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d,
					0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0})
		!= *v)
		throw EXCEPTION("Something's wrong");
}

void enumerate()
{
	std::set<std::string> algos;

	gcrypt::md::base::enumerate(algos);

	for (const auto &algo:algos)
	{
		std::cout << algo
			  << ": "
			  << gcry_md_get_algo_dlen(gcrypt::md::base
						   ::number(algo))
			  << std::endl;
	}
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

		options->addDefaultOptions();

		LIBCXX_NAMESPACE::option::parser opt_parser(LIBCXX_NAMESPACE::option::parser::create());

		opt_parser->setOptions(options);

		if (opt_parser->parseArgv(argc, argv) ||
		    opt_parser->validate())
			exit(0);

		LIBCXX_NAMESPACE::http::useragent::base::https_enable();

		testmd();
		enumerate();
	} catch (LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}

