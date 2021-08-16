/*
** Copyright 2013-2021 Double Precision, Inc.
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
#include <libxml/xinclude.h>
#include <cstdarg>
#include <map>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::xml::implparserObj);

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

error_handler::error::error() noexcept
{
	thread_error=this;
	errorflag=false;
}

error_handler::error::~error()
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

	static void handleValidationError(void *ctx, const char *format, ...)
	{
		size_t n;
		size_t o=512;

		// It's unlikely that the small fragments of error messages
		// that libxml2 emits, piecemeal, will exceed 1024 bytes. But,
		// if so, adapt.

		while (1)
		{
			n=o+512;

			char buffer[n+1];

			buffer[n]=0;
			// See vsprintf(3) note, old glibc, just in case.

			va_list args;
			va_start(args, format);
			o=vsnprintf(buffer, n, format, args);
			va_end(args);

			if (o >= n)
				continue;

			// We better have thread_error initialized

			if (!error_handler::error::thread_error)
				std::cerr << buffer << std::flush;
			else
				error_handler::error::thread_error->message
					<< buffer;
			break;
		}
	}
};

// Global scope constructor, install our error handler hook.

error_handler::error_handler() noexcept
{
	xmlSetStructuredErrorFunc(NULL, handleStructuredError);
	xmlSetGenericErrorFunc(NULL, handleValidationError);
}

error_handler::~error_handler()
{
}

static error_handler capture_it;

parserObj::parserObj()
{
}

parserObj::~parserObj()
{
}

implparserObj::implparserObj(const std::string_view &uriArg,
			     const std::string_view &options)
	: uri(uriArg),
	  p(nullptr),
	  p_options(0),
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

implparserObj::~implparserObj()
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

	auto ret=ref<impldocObj>::create(p->myDoc, locale::base::global());

	xmlFreeParserCtxt(p);
	p=nullptr;
	buffer_size=0;

	// The XML_PARSE_XINCLUDE option seems to be used by the TextReader
	// API only, so we handle it ourselves.

	if (p_options & XML_PARSE_XINCLUDE)
	{
		error_handler::error capture;

		int rc=xmlXIncludeProcessFlags(ret->p, p_options);

		capture.check();

		if (rc < 0)
			throw EXCEPTION(libmsg(_txt("xincluded document parsing failed")));
	}

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

parser parserBase::create(const std::string_view &uri)
{
	return create(uri, "");
}

parser parserBase::create(const std::string_view &uri,
			  const std::string_view &options)
{
	return ref<implparserObj>::create(uri, options);
}

#if 0
{
#endif
}
