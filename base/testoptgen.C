/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/options.H"
#include "x/locale.H"
#include "x/messages.H"

#include <iterator>
#include <algorithm>

class outputFormatClass {

public:

	std::string s;

	template<typename OutputIterator>
	OutputIterator to_string(OutputIterator iter,
				const LIBCXX_NAMESPACE::const_locale &localeRef)
		const
	{
		return (std::copy(s.begin(), s.end(), iter));
	}


	template<typename InputIterator>
	static outputFormatClass from_string(InputIterator beg_iter,
					    InputIterator end_iter,
					    const LIBCXX_NAMESPACE::const_locale &localeArg)

	{
		outputFormatClass c;

		std::copy(beg_iter, end_iter,
			  std::back_insert_iterator<std::string>(c.s));
		return c;
	}
};

static int setregion(std::string region)
{
	std::cout << "setregion: " << region << std::endl;
	return 0;
}

static int setflag()
{
	std::cout << "setflag called" << std::endl;
	return 0;
}

class testoptions_superclass {

public:
	int counter;

	testoptions_superclass(int nbeersArg) : counter(nbeersArg)
	{
	}

	int beercount()
	{
		return counter;
	}
};

#include "testoptgen.h"

int main(int argc, char **argv)
{
	try {
		const char *p=getenv("BEERS");
		testoptions opts(p ? atoi(p):1);

		std::list<std::string> args(opts.parse(argc, argv)->args);

		if (opts.verbose->isSet())
			std::cout << "Verbose: " << opts.verbose->value
				  << std::endl;

		if (opts.debug->isSet())
			std::cout << "Debug: " << opts.debug->value
				  << std::endl;

		if (opts.funcs->isSet())
			std::cout << "    funcs: " << opts.funcs->value << std::endl;

		if (opts.io->isSet())
			std::cout << "    io: " << opts.io->value << std::endl;

		if (opts.outputformat->isSet())
			std::cout << "Output format: " << opts.outputformat->value.s
				  << std::endl;

		if (opts.columns->isSet())
			std::cout << "Column mask: " << opts.columns->value
				  << std::endl;

		for (std::list<std::string>::iterator
			     b(opts.filename->values.begin()),
			     e(opts.filename->values.end()); b != e; ++b)
		{
			std::cout << "Filename: " << std::endl;
		}

		for (std::list<std::string>::iterator
			     b(args.begin()), e(args.end()); b != e; ++b)
		{
			std::cout << "Arg: " << *b << std::endl;
		}

	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
