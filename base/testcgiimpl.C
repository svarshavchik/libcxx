/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "http/cgiimpl.H"
#include "http/content_type_header.H"
#include "options.H"

#include <sstream>

static void testcgiimpl(std::list<std::string> &args)
{
	if (!args.empty() && args.front() == "-")
	{
		LIBCXX_NAMESPACE::http::form::parameters
			params(LIBCXX_NAMESPACE::http::form::parameters
			       ::create());

		args.pop_front();

		while (!args.empty())
		{
			std::string n=args.front();

			args.pop_front();

			if (args.empty())
				break;

			params->insert(std::make_pair(n, args.front()));
			args.pop_front();
		}

		std::copy(params->encode_begin(), params->encode_end(),
			  std::ostreambuf_iterator<char>(std::cout));
		std::cout << std::endl;
		return;
	}

	LIBCXX_NAMESPACE::http::cgiimpl cgi;

	int options=0;

	if (!args.empty())
	{
		std::istringstream i(args.front());

		i >> options;
	}

	LIBCXX_NAMESPACE::uriimpl uri=cgi.getURI();

	std::cout << "Method: " << LIBCXX_NAMESPACE::http::requestimpl
		::methodstr(cgi.method) << std::endl;
	std::cout << "Scheme: " << cgi.scheme << std::endl;
	std::cout << "Script: " << cgi.script_name << std::endl;
	std::cout << "Path: " << cgi.path_info << std::endl;

	for (LIBCXX_NAMESPACE::http::form::map_t::const_iterator
		     b(cgi.parameters->begin()),
		     e(cgi.parameters->end()); b != e; ++b)
		std::cout << "Parameter: " << b->first << "=" << b->second
			  << std::endl;

	std::cout << "Uri: ";

	cgi.getURI(options).toString(std::ostreambuf_iterator<char>(std::cout));

	std::cout << std::endl;

	if (cgi.hasData())
	{
		LIBCXX_NAMESPACE::http::content_type_header(cgi.headers)
			.toString(std::ostreambuf_iterator<char>(std::cout));

		std::cout << " data:" << std::endl;

		std::copy(cgi.begin(), cgi.end(),
			  std::ostreambuf_iterator<char>(std::cout));
		std::cout << std::endl;

		if (cgi.begin() != cgi.end())
			throw EXCEPTION("Second data stream?");
	}
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::option::list
			options(LIBCXX_NAMESPACE::option::list::create());

		options->addDefaultOptions();

		LIBCXX_NAMESPACE::option::parser
			opt_parser(LIBCXX_NAMESPACE::option::parser::create());

		opt_parser->setOptions(options);
		int err=opt_parser->parseArgv(argc, argv);

		if (err == 0) err=opt_parser->validate();

		if (err)
		{
			if (err == LIBCXX_NAMESPACE::option::parser::base
			    ::err_builtin)
				exit(0);

			std::cerr << opt_parser->errmessage();
			std::cerr.flush();
			exit(1);
		}

		testcgiimpl(opt_parser->args);
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}

