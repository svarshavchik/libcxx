/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/options.H"
#include "x/mime/encoder.H"
#include "x/exception.H"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>

static std::string dumpencoder(const LIBCXX_NAMESPACE::mime::encoder &e)
{
	std::stringstream o;

	std::copy(e->begin(), e->end(), std::ostreambuf_iterator<char>(o));

	// Find the random boundary, and delete it.
	o.seekg(0);

	std::string boundary, s;

	while (!std::getline(o, s).eof())
	{
		if (s.size() > 5 &&
		    s.substr(0, 2) == "--" &&
		    s.substr(s.size()-3) == "--\r")
		{
			boundary=s.substr(2, s.size()-5);
		}
	}

	size_t p=0;
	s=o.str();

	std::string::iterator q;

	while ((q=std::search(s.begin()+p, s.end(),
			      boundary.begin(), boundary.end()))
	       != s.end())
	{
		p=q-s.begin();
		s.erase(q, q+boundary.size());
	}

	return s;
}


void testmimeencoder()
{
	static const struct {
		const char *input;
		bool no8bit;
		bool usecrlf;
		size_t linewidth;
		const char *encoding;
		const char *output;
	} tests[]={
		{
			"abracadabra\n",
			false,
			false,
			76,
			"7bit",
			"abracadabra\n"
		},

		{
			"abracadabra\n",
			false,
			false,
			6,
			"quoted-printable",
			"abraca=\ndabra\n"
		},

		{
			"hocuspocus\r\n",
			false,
			true,
			5,
			"quoted-printable",
			"hocus=\r\npocus\r\n"
		},

		{
			"abraca\xc2\xa0pocus\n",
			false,
			false,
			76,
			"8bit",
			"abraca\xc2\xa0pocus\n",
		},			

		{
			"hocus\xc2\xa0" "cadabra\n",
			true,
			false,
			76,
			"quoted-printable",
			"hocus=C2=A0cadabra\n",
		},			
		{
			"walla\xc2\xa0walla\xc2\xa0washington\xc2\xa0",
			true,
			false,
			76,
			"base64",
			"d2FsbGHCoHdhbGxhwqB3YXNoaW5ndG9uwqA=\n",
		},			
	};

	for (const auto &test:tests)
	{
		std::string s=test.input;
		std::string e=test.output;
		std::string o;

		auto encoder=
			LIBCXX_NAMESPACE::mime::create_encoder(s,
							       test.no8bit,
							       test.usecrlf,
							       test.linewidth);

		std::copy(encoder.first->begin(),
			  encoder.first->end(),
			  std::back_insert_iterator<std::string>(o));

		if (e != o)
		{
			std::replace(e.begin(), e.end(), '\n', '<');
			std::replace(e.begin(), e.end(), '\r', '<');

			std::replace(o.begin(), o.end(), '\n', '<');
			std::replace(o.begin(), o.end(), '\r', '<');

			e=std::string(test.encoding) + ":" + e;
			o=std::string(encoder.second) + ":" + o;

			throw EXCEPTION("Expected: " + e + "\n"
					"     Got: " + o);
		}
	}

	typedef LIBCXX_NAMESPACE::headersimpl<LIBCXX_NAMESPACE
					      ::headersbase::crlf_endl>
		crlf_headers_t;

	typedef LIBCXX_NAMESPACE::headersimpl<LIBCXX_NAMESPACE
					      ::headersbase::lf_endl>
		lf_headers_t;


	LIBCXX_NAMESPACE::mime::encoder section1=
		({
			crlf_headers_t headers;

			LIBCXX_NAMESPACE::mime
				::make_section(headers,
					       std::string("hocus\xc2\xa0"
							   "cadabra\r\n"),
					       true,
					       10);
		});

	LIBCXX_NAMESPACE::mime::encoder multipart=
		({
			LIBCXX_NAMESPACE::mime::encoder
				sections[]={section1, section1};

			crlf_headers_t headers;

			LIBCXX_NAMESPACE::mime
				::make_multipart_section(headers,
							 "alternative",
							 sections,
							 sections+2,
							 0);
		});

	std::string s=dumpencoder(multipart);


	if (s !=
	    "Content-Type: multipart/alternative; boundary=\"\"\r\n"
	    "\r\n"
	    "This is a MIME multipart message\r\n"
	    "--\r\n"
	    "Content-Transfer-Encoding: quoted-printable\r\n"
	    "\r\n"
	    "hocus=C2=\r\n"
	    "=A0cadabra\r\n"
	    "\r\n"
	    "--\r\n"
	    "Content-Transfer-Encoding: quoted-printable\r\n"
	    "\r\n"
	    "hocus=C2=\r\n"
	    "=A0cadabra\r\n"
	    "\r\n"
	    "----\r\n")
	{
		throw EXCEPTION("Encoding test 1 failed:\n" + s);
	}

	multipart=
		({
			LIBCXX_NAMESPACE::mime::encoder
				sections[]={section1, section1};

			crlf_headers_t headers;

			headers.append("Subject", "Multipart Message");
			LIBCXX_NAMESPACE::mime
				::make_multipart_section(headers,
							 "alternative",
							 sections,
							 sections+2,
							 30);
		});

	s=dumpencoder(multipart);

	if (s !=
	    "Subject: Multipart Message\r\n"
	    "Content-Type: multipart/alternative;\r\n"
	    "    boundary=\"\"\r\n"
	    "\r\n"
	    "This is a MIME multipart message\r\n"
	    "--\r\n"
	    "Content-Transfer-Encoding: quoted-printable\r\n"
	    "\r\n"
	    "hocus=C2=\r\n"
	    "=A0cadabra\r\n"
	    "\r\n"
	    "--\r\n"
	    "Content-Transfer-Encoding: quoted-printable\r\n"
	    "\r\n"
	    "hocus=C2=\r\n"
	    "=A0cadabra\r\n"
	    "\r\n"
	    "----\r\n")
	{
		throw EXCEPTION("Encoding test 2 failed:\n" + s);
	}

	section1=LIBCXX_NAMESPACE::mime
		::make_base64_section(lf_headers_t(),
				      std::string("Hello world!\n"));

	s=std::string(section1->begin(), section1->end());

	if (s != "Content-Transfer-Encoding: base64\n\nSGVsbG8gd29ybGQhCg==\n")
		throw EXCEPTION("make_base64_section failed: " + s);

	section1=LIBCXX_NAMESPACE::mime
		::make_qp_section(lf_headers_t(),
				  std::string("Hello\xc2\xa0world!\n"));

	s=std::string(section1->begin(), section1->end());

	if (s != "Content-Transfer-Encoding: quoted-printable\n"
	    "\n"
	    "Hello=C2=A0world!\n")
		throw EXCEPTION("make_qp_section failed: " + s);

	section1=LIBCXX_NAMESPACE::mime
		::make_plain_section(lf_headers_t(),
				     std::string("Hello world!\n"));

	s=std::string(section1->begin(), section1->end());

	if (s != "\n"
	    "Hello world!\n")
		throw EXCEPTION("make_plain_section failed: " + s);

	{
		std::ofstream o("testmimeencoder.tmp");

		o << "Hello world!" << std::endl;
		o.close();
	}

	section1=LIBCXX_NAMESPACE::mime
		::make_plain_section(lf_headers_t(),
				     LIBCXX_NAMESPACE::mime::from_file
				     ("testmimeencoder.tmp"));

	if (s != "\n"
	    "Hello world!\n")
		throw EXCEPTION("from_file failed: " + s);

	{
		auto fd=LIBCXX_NAMESPACE::fd::base::open("testmimeencoder.tmp",
							 O_RDONLY);

		for (size_t i=0; i<2; i++)
		{
			section1=LIBCXX_NAMESPACE::mime
				::make_plain_section(lf_headers_t(),
						     LIBCXX_NAMESPACE::mime
						     ::from_file(fd));

			if (s != "\n"
			    "Hello world!\n")
				throw EXCEPTION("from_fd failed: " + s);
		}
	}
	unlink("testmimeencoder.tmp");
}

void testmimeencoder2()
{
	static const struct {
		const char *string;
		const char *lang;
		const char *charset;
		size_t width;
		const char *expected;
	} tests[] = {
		{
			"ataleoftwocities.txt",
			"en",
			"utf-8",
			76,
			"filename=ataleoftwocities.txt",
		},
		{
			"ataleoftwocities.txt",
			"en",
			"utf-8",
			30,
			"filename*0*=utf-8'en'a; filename*1*=taleoftwoc;\n"
			"    filename*2*=ities.txt",
		},
		{
			"a\xA0tale\xA0of\xA0twocities.txt",
			"en",
			"iso-8859-1",
			76,
			"filename*=iso-8859-1'en'a%A0tale%A0of%A0twocities.txt",
		},
		{
			"a\xA0tale\xA0of\xA0twocities.txt",
			"en",
			"iso-8859-1",
			30,
			"filename*0*=iso-8859-1'en'a; filename*1*=%A0tale%A0;\n"
			"    filename*2*=of%A0twoci; filename*3*=ties.txt",
		}
	};

	for (const auto &test:tests)
	{
		LIBCXX_NAMESPACE::headersimpl<LIBCXX_NAMESPACE
					      ::headersbase::lf_endl> headers;

		LIBCXX_NAMESPACE::mime::structured_content_header
			("attachment")
			.rfc2231("filename",
				 test.string,
				 test.charset,
				 test.lang,
				 test.width).append(headers, "Header");

		std::ostringstream o;

		headers.toString(std::ostreambuf_iterator<char>(o));

		std::string s=o.str();

		std::string expected=std::string("Header: attachment; ")
			+ test.expected + "\n\n";

		if (s != expected)
			throw EXCEPTION("Expected:\n" + expected
					+ "\nGot:\n" + s);
	}
}

void testmimeencoder3()
{
	LIBCXX_NAMESPACE::headersimpl<LIBCXX_NAMESPACE
				      ::headersbase::lf_endl> headers;

	headers.append("Mime-Version", "1.0");
	headers.append("Subject", "test");

	auto section1=LIBCXX_NAMESPACE::mime
		::make_section(headers,
			       std::string("Hello world!\n"));

	headers.clear();

	auto section2=LIBCXX_NAMESPACE::mime
		::make_message_rfc822_section(headers, section1);

	std::string s(section2->begin(), section2->end());

	if (s !=
	    "Content-Type: message/rfc822\n"
	    "\n"
	    "Mime-Version: 1.0\n"
	    "Subject: test\n"
	    "Content-Transfer-Encoding: 7bit\n"
	    "\n"
	    "Hello world!\n")
		throw EXCEPTION("message/rfc822 test failed");
}

void testmimeencoder4()
{
	const struct {
		const char *charset;
		const char *language;
		const char *value;
		const char *expected;
	} tests[]={
		{
			"iso-8859-1",
			"EN",
			"Hello world!",
			"Hello world!"
		},
		{
			"iso-8859-1",
			"",
			"Hello\xA0world!",
			"=?iso-8859-1?Q?Hello=A0world!?="
		},
		{
			"iso-8859-1",
			"EN",
			"Hello\xA0\xA0\xA0world!",
			"=?iso-8859-1*EN?B?SGVsbG+goKB3b3JsZCE=?=",
		},
	};

	for (const auto &test:tests)
	{
		LIBCXX_NAMESPACE::mime::structured_content_header
			header("rfc2047");

		header.rfc2047("test", test.value, test.charset,
			      test.language);

		std::string s=header;

		std::string expected="rfc2047; test=\""
			+ std::string(test.expected) + "\"";

		if (s != expected)
			throw EXCEPTION("Expected: " + expected + "\n"
					"Got:      " + s);
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
		testmimeencoder();
		testmimeencoder2();
		testmimeencoder3();
		testmimeencoder4();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
