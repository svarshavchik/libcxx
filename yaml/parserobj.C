/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/yaml/parserobj.H"

#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

extern "C" {

	//! Stub forwards C callback to the implementing object.

	static int yaml_read_handler_callback(void *data,
					      unsigned char *buffer,
					      size_t size,
					      size_t *size_read)
	{
		return reinterpret_cast<yaml::parserObj::read_handler_base *>
			(data)->read(buffer, size, size_read);
	}
};

yaml::parserObj::~parserObj()
{
}

void yaml::parserObj::parse(read_handler_base &b)
{
	yaml_parser_t parser;

	if (!yaml_parser_initialize(&parser))
	{
		throw EXCEPTION(_("yaml_parser_initialize failed"));
	}

	try {
		yaml_parser_set_input(&parser,
				      &yaml_read_handler_callback,
				      reinterpret_cast<void *>(&b));
		parse(b, parser);
	} catch (...)
	{
		yaml_parser_delete(&parser);
		throw;
	}
	yaml_parser_delete(&parser);
}

void yaml::parserObj::parse(read_handler_base &b, yaml_parser_t &parser)
{
	while (1)
	{
		auto d=document::create((documentObj::do_not_initialize *)
					nullptr);

		documentObj::writelock_t lock(d->doc);

		if (!yaml_parser_load(&parser, *lock))
			break;

		try {
			documents.push_back(d);
		} catch (...) {
			yaml_document_delete(*lock);
			throw;
		}

		d->initialized=true;

		if (parser.eof)
			break;
	}

	if (b.caught_exception_flag)
		b.caught_exception->rethrow();

	if (parser.error != YAML_NO_ERROR)
	{
		std::ostringstream o;

		o << parser.problem << ": line "
		  << parser.problem_mark.line
		  << ", column " << parser.problem_mark.column;

		throw EXCEPTION(o.str());
	}
}

// Precompiled templates

// Punt std::iterator to a std::string::const_iterator

template<>
yaml::parserObj::parserObj(std::string::iterator b, std::string::iterator e)
	: parserObj(std::string::const_iterator(b),
		    std::string::const_iterator(e))
{
}

template yaml::parserObj::parserObj(std::string::const_iterator,
				    std::string::const_iterator);

template yaml::parserObj::parserObj(std::istreambuf_iterator<char>,
				    std::istreambuf_iterator<char>);

template class yaml::parserObj::read_handler<std::string::const_iterator>;
template class yaml::parserObj::read_handler<std::istreambuf_iterator<char> >;

#if 0
{
#endif
}
