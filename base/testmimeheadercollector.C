/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/options.H"
#include "x/mime/contentheadercollector.H"
#include "x/mime/headercollector.H"
#include "x/mime/headeriter.H"
#include "x/exception.H"
#include <iostream>
#include <sstream>
#include <iterator>

void header_collector_test(const std::string &headers,
			   const std::string &expected)
{
	std::map<std::string, std::string> collected_headers;

	auto collector=LIBCXX_NAMESPACE::mime::make_header_collector
		([&collected_headers]
		 (const std::string &name,
		  const std::string &name_lc,
		  const std::string &contents)
		 {
			 collected_headers[name]=contents;
		 });

	typedef LIBCXX_NAMESPACE::mime::header_iter<decltype(collector)>
		header_iter_t;

	typedef LIBCXX_NAMESPACE::mime::bodystart_iter<header_iter_t>
		bodystart_iter_t;

	typedef LIBCXX_NAMESPACE::mime::newline_iter<bodystart_iter_t>
		newline_iter_t;

	*std::copy(headers.begin(),
		   headers.end(),
		   newline_iter_t::create(bodystart_iter_t::create(header_iter_t::create(collector))))
		=LIBCXX_NAMESPACE::mime::eof;

	std::ostringstream o;

	for (const auto &h:collected_headers)
		o << h.first << '=' << h.second << '|';

	std::string s=o.str();

	if (s != expected)
		throw EXCEPTION("Ended up with " + s + " but expected " +
				expected);
}

void testheadercollector()
{
	header_collector_test("a1: b1\n"
			      "a2 : b2 \n"
			      "a3:b3\n"
			      "a4: b4\n    b4a\n"
			      "\n",
			      "a1=b1|a2=b2|a3=b3|a4=b4 b4a|");
	header_collector_test("a5: b5\n", "a5=b5|");
	header_collector_test("a6: b6", "a6=b6|");
	header_collector_test("a7:\n   b7\n", "a7=b7|");

}

void contentheader_collector_test(const std::string &headers,
				  const std::string &expected,
				  bool mime_10_required)
{
	auto collector=LIBCXX_NAMESPACE::mime::contentheader_collector
		::create(mime_10_required);

	typedef LIBCXX_NAMESPACE::mime::header_iter<decltype(collector)>
		header_iter_t;

	typedef LIBCXX_NAMESPACE::mime::bodystart_iter<header_iter_t>
		bodystart_iter_t;

	typedef LIBCXX_NAMESPACE::mime::newline_iter<bodystart_iter_t>
		newline_iter_t;

	*std::copy(headers.begin(),
		   headers.end(),
		   newline_iter_t::create(bodystart_iter_t::create(header_iter_t::create(collector))))
		=LIBCXX_NAMESPACE::mime::eof;

	std::ostringstream o;

	for (const auto &h:collector.get()->content_headers)
	{
		o << h.first << '=' << h.second.value() << '|';
	}

	std::string s=o.str();

	if (s != expected)
		throw EXCEPTION("Ended up with " + s + " but expected " +
				expected);
}

void testcontentheadercollector()
{
	contentheader_collector_test("Mime-Version: 1.0\n"
				     "Content-Type: text/plain; test=1\n"
				     "Irrelevant: header\n"
				     "\n",
				     "Content-Type=text/plain; test=1|",
				     true);
	contentheader_collector_test("Content-Type: text/plain; test=2\n"
				     "\n",
				     "",
				     true);
	contentheader_collector_test("Content-Type: text/plain; test=3\n"
				     "\n",
				     "Content-Type=text/plain; test=3|",
				     false);
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
		testheadercollector();
		testcontentheadercollector();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
