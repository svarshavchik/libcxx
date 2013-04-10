/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "xml_internal.h"
#include "gettext_in.h"
#include "x/fd.H"
#include "x/exception.H"
#include "x/strtok.H"
#include "x/join.H"
#include "x/chrcasecmp.H"
#include "x/messages.H"
#include <cstdarg>
#include <map>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::xml::implparserObj);

namespace LIBCXX_NAMESPACE {
	namespace xml {
#if 0
	};
};
#endif

error_handler::error::error() noexcept
{
	thread_error=this;
	errorflag=false;
}

error_handler::error::~error() noexcept
{
	thread_error=0;
}

void error_handler::error::check()
{
	if (errorflag)
		throw EXCEPTION(message.str());
}

error_handler::error __thread *error_handler::error::thread_error;
bool __thread error_handler::error::errorflag;

extern "C" {

	static void handleStructuredError(void *dummy,
					  xmlErrorPtr error)
	{
		std::ostream &o=error_handler::error::thread_error
			? error_handler::error::thread_error->message
			: std::cerr;

		if (error->file)
		{
			o << error->file << "(" << error->line << "): ";
		}
		o << error->message << std::flush;
		if (error->level >= XML_ERR_ERROR)
			error_handler::error::errorflag=true;
	}
};

// Global scope constructor, install our error handler hook.

error_handler::error_handler() noexcept
{
	xmlSetStructuredErrorFunc(NULL, handleStructuredError);
}

error_handler::~error_handler() noexcept
{
}

static error_handler capture_it;

parserObj::parserObj()
{
}

parserObj::~parserObj() noexcept
{
}

implparserObj::implparserObj(const std::string &uriArg,
			     const std::string &options) : uri(uriArg),
							   p(nullptr),
							   buffer_size(0)
{
// XML_PARSER_ options that are pulled out of libxml/parser.h
	struct {
		int optvalue;
		const char *optname;
	} parser_options[]={
#include "xml_parser_options.h"
	};

	std::set<std::string, chrcasecmp::str_less> requested_options;

	{
		std::list<std::string> option_list;

		strtok_str(options, ", \r\t\n", option_list);

		for (const auto &option:option_list)
			requested_options.insert(option);
	}

	p_options=0;

	for (const auto &option:parser_options)
	{
		auto iter=requested_options.find(option.optname);

		if (iter != requested_options.end())
		{
			p_options |= option.optvalue;
			requested_options.erase(iter);
		}
	}

	if (!requested_options.empty())
		throw EXCEPTION(gettextmsg(libmsg(_txt("Unknown XML parsing options: %1%")),
					   join(requested_options, ", ")));

	buffer.resize(fdbaseObj::get_buffer_size());
}

implparserObj::~implparserObj() noexcept
{
	try {
		// Clean up, destroy any existing parser context.

		if (p)
			done();
	} catch (const exception &e)
	{
	}
}

void implparserObj::operator=(char c)
{
	if (buffer_size >= buffer.size())
		flush();
	buffer[buffer_size++]=c;
}

doc implparserObj::done()
{
	flush();

	if (p == nullptr)
		return doc::create();

	// Last chunk.
	{
		error_handler::error capture;

		xmlParseChunk(p, nullptr, 0, 1);

		capture.check();
	}

	auto ret=ref<impldocObj>::create(p->myDoc);

	xmlFreeParserCtxt(p);
	p=nullptr;
	buffer_size=0;
	return ret;
}

// The output gets collected in chunks, parse it.

void implparserObj::flush()
{
	if (buffer_size == 0)
		return;

	{
		error_handler::error capture;

		// The first chunk instantiates the underlying libxml2 parser.
		if (p == nullptr)
		{
			docuri=uri;
			p=xmlCreatePushParserCtxt(NULL, NULL, &buffer[0],
						  buffer_size, docuri.c_str());
			xmlCtxtUseOptions(p, p_options);
		}
		else
		{
			xmlParseChunk(p, &buffer[0], buffer_size, 0);
		}

		capture.check();
	}
	buffer_size=0;
}

parser parserBase::create(const std::string &uri)
{
	return create(uri, "");
}

parser parserBase::create(const std::string &uri, const std::string &options)
{
	return ref<implparserObj>::create(uri, options);
}

#if 0
{
	{
#endif
	}
}
