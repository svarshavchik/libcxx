#include <x/mime/newlineiter.H>
#include <x/mime/headeriter.H>
#include <x/mime/bodystartiter.H>
#include <x/mime/headercollector.H>
#include <x/mime/sectiondecoder.H>
#include <x/mime/entityparser.H>
#include <x/mime/structured_content_header.H>
#include <x/chrcasecmp.H>
#include <iostream>

int main()
{
	std::string content_transfer_encoding;
	std::string content_type="text";
	std::string charset;

	auto info=x::mime::sectioninfo::create();

	auto processor=
		x::mime::make_entity_parser
		(x::mime::header_collector::create
		 ([&]
		  (const std::string &name,
		   const std::string &name_lc,
		   const std::string &value)
		{
			x::chrcasecmp::str_equal_to cmp;

			if (cmp(name, x::mime::structured_content_header
				::content_transfer_encoding))
			{
				content_transfer_encoding=
					x::mime
					::structured_content_header(value)
					.value;
			}

			if (cmp(name, x::mime::structured_content_header
				::content_type))
			{
				x::mime::structured_content_header hdr(value);

				content_type=hdr.mime_content_type();
				charset=hdr.charset("iso-8859-1");
			}
		}), [&]
		 {
			 typedef std::ostreambuf_iterator<char> dump_iter_t;

			 dump_iter_t dump_to_stdout(std::cout);

			 return content_type == "text"
				 ? x::mime::section_decoder
				 ::create(content_transfer_encoding,
					  dump_to_stdout,
					  charset,
					  "UTF-8")
				 : x::mime::section_decoder
				 ::create(content_transfer_encoding,
					  dump_to_stdout);
		 }, info);

	typedef x::mime::newline_iter<decltype(processor)>
		newline_iter_t;

	std::copy(std::istreambuf_iterator<char>(std::cin),
		  std::istreambuf_iterator<char>(),
		  newline_iter_t::create(processor))
		.get()->eof();

	std::cout << info->header_char_cnt << " bytes in the header, "
		  << info->body_char_cnt << " bytes in the body." << std::endl
		  << info->header_line_cnt << " lines in the header, "
		  << info->body_line_cnt << " lines in the body." << std::endl;
	if (info->no_trailing_newline)
		std::cout << "No trailing newline" << std::endl;
	return 0;
}
