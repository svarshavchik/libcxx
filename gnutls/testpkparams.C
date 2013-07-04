/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "gnutls/init.H"
#include "gnutls/pkparams.H"
#include "gnutls/dhparams.H"
#include "options.H"
#include "property_properties.H"
#include "exception.H"

#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>

static void testpkparams()
{
	static const gnutls_pk_algorithm algo_list[]={
		GNUTLS_PK_RSA, GNUTLS_PK_DSA};

	static const char * const algo_names[]={
		"RSA", "DH"
	};

	int i;

	for (i=0; i<2; i++)
	{
		std::cout << algo_names[i]
			  << ": Importing default parameters..."
			  << std::endl << std::flush;

		LIBCXX_NAMESPACE::gnutls::pkparams
			pk=LIBCXX_NAMESPACE::gnutls::pkparams
			::create(algo_list[i]);
		pk->import();
	}
}

static void testpkparams2()
{
	LIBCXX_NAMESPACE::gnutls::dhparams dhp=
		LIBCXX_NAMESPACE::gnutls::dhparams::create();
	dhp->import();
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::option::list
			options(LIBCXX_NAMESPACE::option::list::create());

		options->addDefaultOptions();

		LIBCXX_NAMESPACE::option::parser
			parser(LIBCXX_NAMESPACE::option::parser::create());

		parser->setOptions(options);

		if (parser->parseArgv(argc, argv) ||
		    parser->validate())
			exit(1);

		alarm(30);
		testpkparams();
		testpkparams2();
	} catch (LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
	}
	return 0;
}

