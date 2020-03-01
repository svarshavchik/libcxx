#include <x/mime/newlineiter.H>
#include <x/mime/headeriter.H>
#include <x/mime/bodystartiter.H>
#include <x/mime/headercollector.H>
#include <x/mime/sectiondecoder.H>
#include <x/mime/entityparser.H>
#include <x/mime/structured_content_header.H>
#include <x/mime/contentheadercollector.H>
#include <x/chrcasecmp.H>
#include <iostream>

class customsectioninfoObj;

typedef x::ref<customsectioninfoObj> customsectioninfo;

typedef x::const_ref<customsectioninfoObj> const_customsectioninfo;

// Custom subclass stores the content type, in addition to sectioninfoObj's metadata.

class customsectioninfoObj : public x::mime::sectioninfoObj {

public:

	std::string content_type;

	customsectioninfoObj() {}

	~customsectioninfoObj() {}

	// The custom subclass must implement create_subsection(), and
	// the subsection constructor, that forwards a reference to the parent
	// object to x::mime::sectioninfoObj's constructor.

	customsectioninfo create_subsection()
	{
		return customsectioninfo::create(customsectioninfo(this));
	}

	customsectioninfoObj(const customsectioninfo &parentArg)
		: x::mime::sectioninfoObj(parentArg)
	{
	}
};

x::outputrefiterator<int> parse_section(const x::headersbase &,
					const customsectioninfo &);

x::outputrefiterator<int> create_parser(const customsectioninfo &info,
					bool is_message_rfc822)
{
	auto header_iter=
		x::mime::contentheader_collector::create(is_message_rfc822);

	auto headers=header_iter.get();

	return x::mime::make_entity_parser
		(header_iter,
		 [headers, info]
		 {
			 return parse_section(headers->content_headers,
					      info);
		 }, info);
}

x::outputrefiterator<int>
parse_section(const x::headersbase &headers,
	      const customsectioninfo &info)
{
	x::mime::structured_content_header
		content_type(headers,
			     x::mime::structured_content_header::content_type);

	info->content_type=content_type.value;

	if (content_type.is_message())
		return x::mime::make_message_rfc822_parser
			(create_parser, info);

	if (content_type.is_multipart())
		return x::mime::make_multipart_parser
			(content_type.boundary(), create_parser, info);

	typedef std::ostreambuf_iterator<char> dump_iter_t;

	dump_iter_t dump_to_stdout(std::cout);

	std::string content_transfer_encoding=
		x::mime::structured_content_header
		(headers,
		 x::mime::structured_content_header::content_transfer_encoding)
		.value;

	return content_type.mime_content_type() == "text"
		? x::mime::section_decoder::create(content_transfer_encoding,
						   dump_to_stdout,
						   content_type
						   .charset("iso-8859-1"),
						   "UTF-8")
		: x::mime::section_decoder::create(content_transfer_encoding,
						   dump_to_stdout);
}

void dump(const const_customsectioninfo &info)
{
	std::cout << "MIME section " << info->index_name()
		  << " (" << info->content_type
		  << ") starts at character offset "
		  << info->starting_pos << std::endl
		  << "  " << info->header_char_cnt << " bytes in the header, "
		  << info->body_char_cnt << " bytes in the body." << std::endl
		  << "  " << info->header_line_cnt << " lines in the header, "
		  << info->body_line_cnt << " lines in the body." << std::endl;
	if (info->no_trailing_newline)
		std::cout << "  No trailing newline" << std::endl;
	for (const auto &child:info->children)
	{
		std::cout << std::endl;
		dump(child);
	}
}

int main()
{
	x::mime::sectioninfoptr top_level_info;

	std::copy(std::istreambuf_iterator<char>(std::cin),
		  std::istreambuf_iterator<char>(),
		  x::mime::make_document_entity_parser
		  ([&top_level_info]
		   (const customsectioninfo &info,
		    bool is_message_rfc822)
		   {
			   top_level_info=info;
			   return create_parser(info, is_message_rfc822);
		   },
		   // Need to specify all the parameters to
		   // make_document_entity_parser() in order to construct
		   // the top level entity's custom section information object.
		   false,
		   true,
		   customsectioninfo::create()))
		.get()->eof();

	if (top_level_info.null())
	{
		std::cerr << "How did we get here?" << std::endl;
		return 0;
	}

	dump(top_level_info);
	return 0;
}
