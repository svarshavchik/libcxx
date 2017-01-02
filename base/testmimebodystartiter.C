/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/options.H"
#include "x/mime/newlineiter.H"
#include "x/mime/bodystartiter.H"
#include "x/mime/headeriter.H"
#include "x/mime/headercollector.H"
#include "x/mime/entityparser.H"
#include "x/mime/structured_content_header.H"
#include "x/mime/sectiondecoder.H"
#include "x/chrcasecmp.H"
#include "x/join.H"
#include "x/exception.H"
#include <iostream>
#include <sstream>
#include <iterator>
#include <vector>
#include <algorithm>

#define NEWLINE LIBCXX_NAMESPACE::mime::newline_start, '\n', LIBCXX_NAMESPACE::mime::newline_end

#define BODYSTART LIBCXX_NAMESPACE::mime::body_start

#define SEPARATOR LIBCXX_NAMESPACE::mime::separator_line_start

void testmimeiter()
{
	static struct {
		const std::vector<int> test_seq;
	} tests[] = {
		{
			{'a', NEWLINE, 'b', NEWLINE, SEPARATOR, NEWLINE, BODYSTART, 'c', NEWLINE, NEWLINE },
		},

		{
			{SEPARATOR, NEWLINE, BODYSTART, 'c', NEWLINE, NEWLINE },
		},

		{
			{BODYSTART},
		},
	};

	size_t test_num=0;

	for (const auto &test:tests)
	{
		++test_num;

		std::vector<int> expected=test.test_seq;

		expected.push_back(LIBCXX_NAMESPACE::mime::eof);

		std::vector<int> actual;

		typedef std::back_insert_iterator<std::vector<int>> iter_t;

		auto test_iter=
			LIBCXX_NAMESPACE::mime::bodystart_iter<iter_t>
			::create(iter_t(actual));

		for (int n:expected)
		{
			if (n != BODYSTART && n != SEPARATOR)
				*test_iter++=n;
		}

		if (expected != actual)
			throw EXCEPTION(({
						std::ostringstream o;

						o << "Test " << test_num
						  << " failed" << std::endl;

						o.str();
					}));
	}

}

void testmimeiter2()
{
	std::string hello_world;

	std::string content_transfer_encoding;
	std::string content_type="application";
	std::string charset;

	auto processor=
		LIBCXX_NAMESPACE::mime::make_section_processor
		(LIBCXX_NAMESPACE::mime::header_collector
		 ::create([&]
			  (const std::string &name,
			   const std::string &name_lc,
			   const std::string &value)
		{
			LIBCXX_NAMESPACE::chrcasecmp::str_equal_to cmp;

			if (cmp(name, LIBCXX_NAMESPACE::mime
				::structured_content_header
				::content_transfer_encoding))
			{
				content_transfer_encoding=
					LIBCXX_NAMESPACE::mime
					::structured_content_header(value)
					.value;
			}

			if (cmp(name, LIBCXX_NAMESPACE::mime
				::structured_content_header
				::content_type))
			{
				LIBCXX_NAMESPACE::mime
				::structured_content_header hdr(value);

				content_type=hdr.mime_content_type();
				charset=hdr.charset("iso-8859-1");
			}
		}), [&]
		 {
			 typedef std::back_insert_iterator<std::string>
				 ins_iter_t;

			 ins_iter_t ins_iter(hello_world);

			 return content_type == "text"
				 ? LIBCXX_NAMESPACE::mime::section_decoder
				 ::create(content_transfer_encoding,
					  ins_iter,
					  charset,
					  "UTF-8")
				 : LIBCXX_NAMESPACE::mime::section_decoder
				 ::create(content_transfer_encoding,
					  ins_iter);
		 }, LIBCXX_NAMESPACE::mime::sectioninfo::create());

	typedef LIBCXX_NAMESPACE::mime::header_iter<decltype(processor)>
		header_iter_t;

	typedef LIBCXX_NAMESPACE::mime::bodystart_iter<header_iter_t>
		bodystart_iter_t;

	typedef LIBCXX_NAMESPACE::mime::newline_iter<bodystart_iter_t>
		newline_iter_t;

	std::string mime_message=
		"Subject: test\n"
		"Content-Type: text/plain; charset=iso-8859-1\n"
		"Content-Transfer-Encoding: quoted-printable\n"
		"\n"
		"Hello=A0world!\n";

	std::copy(mime_message.begin(),
		  mime_message.end(),
		  newline_iter_t::create
		  (bodystart_iter_t::create
		   (header_iter_t::create(processor))))
		.get()->eof();

	if (hello_world != "Hello\xc2\xa0world!\n")
		throw EXCEPTION("body_collector test failed");
}

void testmimeiter3()
{
	std::string hello_world;

	std::string content_transfer_encoding;
	std::string content_type="application";
	std::string charset;

	auto processor=
		LIBCXX_NAMESPACE::mime::make_section_processor
		(LIBCXX_NAMESPACE::mime::header_collector
		 ::create([&]
			  (const std::string &name,
			   const std::string &name_lc,
			   const std::string &value)
		{
		}),
		 [&]
		 {
			 return LIBCXX_NAMESPACE::mime::
				 make_entity_parser
				 (LIBCXX_NAMESPACE::mime::header_collector
				  ::create([&]
					   (const std::string &name,
					    const std::string &name_lc,
					    const std::string &value)
					   {
						   LIBCXX_NAMESPACE::chrcasecmp::str_equal_to cmp;

						   if (cmp(name, LIBCXX_NAMESPACE::mime
							   ::structured_content_header
							   ::content_transfer_encoding))
						   {
							   content_transfer_encoding=
								   LIBCXX_NAMESPACE::mime
								   ::structured_content_header(value)
								   .value;
						   }

						   if (cmp(name, LIBCXX_NAMESPACE::mime
							   ::structured_content_header
							   ::content_type))
						   {
							   LIBCXX_NAMESPACE::mime
								   ::structured_content_header hdr(value);

							   content_type=hdr.mime_content_type();
							   charset=hdr.charset("iso-8859-1");
						   }
					   }), [&]
				  {
					  typedef std::back_insert_iterator<std::string>
						  ins_iter_t;

					  ins_iter_t ins_iter(hello_world);

					  return content_type == "text"
						  ? LIBCXX_NAMESPACE::mime::section_decoder
						  ::create(content_transfer_encoding,
							   ins_iter,
							   charset,
							   "UTF-8")
						  : LIBCXX_NAMESPACE::mime::section_decoder
						  ::create(content_transfer_encoding,
							   ins_iter);
				  }, LIBCXX_NAMESPACE::mime::sectioninfo::create());
		 }, LIBCXX_NAMESPACE::mime::sectioninfo::create());

	typedef LIBCXX_NAMESPACE::mime::header_iter<decltype(processor)>
		header_iter_t;

	typedef LIBCXX_NAMESPACE::mime::bodystart_iter<header_iter_t>
		bodystart_iter_t;

	typedef LIBCXX_NAMESPACE::mime::newline_iter<bodystart_iter_t>
		newline_iter_t;

	std::string mime_message=
		"Subject: test\n"
		"Content-Type: message/rfc822\n"
		"\n"
		"Content-Type: text/plain; charset=iso-8859-1\n"
		"Content-Transfer-Encoding: quoted-printable\n"
		"\n"
		"Hello=A0world!\n";

	std::copy(mime_message.begin(),
		  mime_message.end(),
		  newline_iter_t::create
		  (bodystart_iter_t::create
		   (header_iter_t::create(processor))))
		.get()->eof();

	if (hello_world != "Hello\xc2\xa0world!\n")
		throw EXCEPTION("body_collector test failed");
}

class content_headersObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	content_headersObj()
	{
	}

	~content_headersObj()
	{
	}

	std::string content_type;
	std::string content_transfer_encoding;
};

static LIBCXX_NAMESPACE::mime::header_collector
get_headers(const LIBCXX_NAMESPACE::ref<content_headersObj> &headers)
{
	return LIBCXX_NAMESPACE::mime::header_collector
		::create([headers]
			 (const std::string &name,
			  const std::string &name_lc,
			  const std::string &value)
			 {
				 LIBCXX_NAMESPACE::chrcasecmp::str_equal_to cmp;

				 if (cmp(name, LIBCXX_NAMESPACE::mime
					 ::structured_content_header
					 ::content_type))
				 {
					 headers->content_type=value;
				 }

				 if (cmp(name, LIBCXX_NAMESPACE::mime
					 ::structured_content_header
					 ::content_transfer_encoding))
				 {
					 headers->content_transfer_encoding=
						 LIBCXX_NAMESPACE::mime
						 ::structured_content_header
						 (value).value;
				 }
			 });
}

class mime_processor {

public:

	typedef std::back_insert_iterator<std::string> ins_iter_t;

	ins_iter_t iter;

	mime_processor(std::string &str) : iter(str)
	{
	}

	~mime_processor()
	{
	}

	std::vector<LIBCXX_NAMESPACE::mime::sectioninfo> sections;

	LIBCXX_NAMESPACE::outputrefiterator<int>
	make_section_iterator(const LIBCXX_NAMESPACE::mime::sectioninfo &info,
			      bool is_message_rfc822)
	{
		sections.push_back(info);

		auto new_headers=
			LIBCXX_NAMESPACE::ref<content_headersObj>::create();

		return LIBCXX_NAMESPACE::mime::make_entity_parser
			(get_headers(new_headers),
			 [this, new_headers, info]()
			 -> LIBCXX_NAMESPACE::outputrefiterator<int>
			 {
				 LIBCXX_NAMESPACE::mime
					 ::structured_content_header
					  content_type(new_headers->
						       content_type);

				 return LIBCXX_NAMESPACE::mime
					 ::make_parser
					 (content_type,
					  info,
					  [this]
					  (const LIBCXX_NAMESPACE::mime::sectioninfo &info,
					   bool is_message_rfc822)
					  {
						  return make_section_iterator
							  (info,
							   is_message_rfc822);
					  },
					  [this, new_headers]
					  (const LIBCXX_NAMESPACE::mime::sectioninfo &info)
					  {
						  return get_decoder
							  (new_headers);
					  });
			 }, info);
	}

	LIBCXX_NAMESPACE::outputrefiterator<int>
	get_decoder(const LIBCXX_NAMESPACE::ref<content_headersObj> &headers)
	{
		LIBCXX_NAMESPACE::mime::structured_content_header
			hdr(headers->content_type);

		return hdr.mime_content_type() == "text"
			? LIBCXX_NAMESPACE::mime::section_decoder
			::create(headers->content_transfer_encoding,
				 iter,
				 hdr.charset("iso-8859-1"),
				 "UTF-8")
			: LIBCXX_NAMESPACE::mime::section_decoder
			::create(headers->content_transfer_encoding,
				 iter);
	}
};

void testmimeiter4()
{
	static const struct {
		const char *mime_message;
		const char *decoded_message;
		const char *layout;
	} tests[]={
		{
			"Content-Type: text/plain; charset=iso-8859-1\n"
			"\n"
			"Hello\xa0world\n",
			"Hello\xc2\xa0world\n",
			"1=0,46,12,2,1",
		},
		{
			"Content-Type: text/plain; charset=iso-8859-1\n"
			"\n"
			"Hello\xa0world",
			"Hello\xc2\xa0world",
			"1=0,46,11,2,1,NOEOL",
		},
		{
			"Content-Type: multipart/alternative; boundary=xxyy\n"
			"\n"
			"--xxyy\n"
			"Content-Type: text/plain; charset=iso-8859-1\n"
			"\n"
			"Hello\xa0world\n"
			"--xxyy--\n"
			"Ignored\n",
			"Hello\xc2\xa0world",
			"1=0,52,82,2,6; 1.1=59,46,11,2,1,NOEOL",
		},
		{
			"Subject: test\n"
			"Content-Type: multipart/mixed; boundary=xxyy\n"
			"\n"
			"Multipart message\n"
			"--xxyy\n"
			"Content-Type: message/rfc822\n"
			"\n"
			"Content-Type: text/plain; charset=iso-8859-1\n"
			"Content-Transfer-Encoding: quoted-printable\n"
			"\n"
			"Hello=20world!\n"
			"\n"
			"--xxyy\n"
			"Content-Type: application/octet-stream\n"
			"\n"
			"Hello world!\n"
			"\n"
			"--xxyy--\n"
			"\n"
			"\n",
			"Hello world!\n"
			"Hello world!\n",
			"1=0,60,233,3,17; 1.1=85,30,105,2,4; 1.1.1=115,90,15,3,1; 1.2=228,40,13,2,1",
		}
	};

	size_t cnt=0;

	for (const auto &test:tests)
	{
		++cnt;

		std::string mime_message=test.mime_message;

		std::string hello_world;

		mime_processor processor(hello_world);

		std::copy(mime_message.begin(),
			  mime_message.end(),
			  LIBCXX_NAMESPACE::mime::make_document_entity_parser
			  ([&processor]
			   (const LIBCXX_NAMESPACE::mime::sectioninfo &info,
			    bool is_message_rfc822)
			   {
				   return processor.make_section_iterator
					   (info, is_message_rfc822);
			   }))
			.get()->eof();

		if (hello_world != test.decoded_message)
			throw EXCEPTION("testmimeiter4: test "
					+ LIBCXX_NAMESPACE::tostring(cnt)
					+ " failed (decode)");

		std::vector<std::string> sections;

		sections.reserve(processor.sections.size());

		std::transform(processor.sections.begin(),
			       processor.sections.end(),
			       std::back_insert_iterator<std::vector<std::string
			       >>(sections),
			       []
			       (const LIBCXX_NAMESPACE::mime::sectioninfo &s)
			       {
				       std::ostringstream o;

				       o << s->index_name() << "="
					 << s->starting_pos
					 << ","
					 << s->header_char_cnt
					 << ","
					 << s->body_char_cnt
					 << ","
					 << s->header_line_cnt
					 << ","
					 << s->body_line_cnt
					 << (s->no_trailing_newline ?
					     ",NOEOL":"");

				       return o.str();
			       });

		auto layout=LIBCXX_NAMESPACE::join(sections, "; ");

		if (layout != test.layout)
			throw EXCEPTION("testmimeiter4: layout: " + layout
					+ ", expected: " + test.layout);
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
		testmimeiter();
		testmimeiter2();
		testmimeiter3();
		testmimeiter4();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
