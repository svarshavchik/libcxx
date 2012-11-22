/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/form.H"
#include "x/http/useragent.H"
#include "x/http/responseimpl.H"
#include "x/property_value.H"
#include "x/mime/newlineiter.H"
#include "x/mime/bodystartiter.H"
#include "x/mime/headeriter.H"
#include "x/mime/headercollector.H"
#include "x/mime/contentheadercollector.H"
#include "x/mime/sectioniter.H"
#include "x/mime/sectiondecoder.H"
#include "x/mime/rfc2047.H"
#include "x/refiterator.H"
#include "gettext_in.h"

#define LIBCXX_TEMPLATE_DECL
#include <http/useragentobj_t.H>
#undef LIBCXX_TEMPLATE_DECL

namespace LIBCXX_NAMESPACE {
	namespace http {
		namespace form {
#if 0
		};
	};
};
#endif

#define LIBCXX_TEMPLATE_DECL
#include "http/form_t.H"
#undef LIBCXX_TEMPLATE_DECL

property::value<size_t> LIBCXX_HIDDEN
formmaxsize(LIBCXX_NAMESPACE_WSTR "::http::form::maxsize", 1024 * 1024 * 10);

size_t getformmaxsize()
{
	return formmaxsize.getValue();
}

const char hex[]="0123456789ABCDEFabcdef";

parametersObj::parametersObj() : consumedFlag(false)
{
}

parametersObj::~parametersObj() noexcept
{
}

parametersObj::parametersObj(const std::string &query_string)
	: consumedFlag(false)
{
	decode_params(query_string.begin(), query_string.end());
}

template<>
void
parametersObj::decode_params(std::string::iterator b, std::string::iterator e)

{
	return decode_params(static_cast<std::string::const_iterator>(b),
			     static_cast<std::string::const_iterator>(e));
}

// -------------------------------------------------------------------------

const char *parametersObj::word_iter::curchar() noexcept
{
	if (!*ptr)
		return ptr;

	switch (escape) {
	case do_escape:
		// Do not encode only unreserved characters, RFC 3986.

		if ((*ptr >= '0' && *ptr <= '9') ||
		    (*ptr >= 'A' && *ptr <= 'Z') ||
		    (*ptr >= 'a' && *ptr <= 'z') ||
		    (*ptr == '-' || *ptr == '_' || *ptr == '.' || *ptr == '~'))
			return ptr;
		return "%";
	case escape_next1:
		return &hex[ ((*ptr) >> 4) & 15];
	case escape_next2:
		return &hex[*ptr & 15];
	default:
		break;
	}
	return ptr;
}

void parametersObj::word_iter::advance()
{
	switch (escape) {
	case do_escape:
		if (*curchar() == '%')
		{
			escape=escape_next1;
			return;
		}
		break;
	case escape_next1:
		escape=escape_next2;
		return;
	case escape_next2:
		escape=do_escape;
		break;
	default:
		break;
	}
	++ptr;
}

bool parametersObj::encode_iter::eof() const
{
	while (begin_iter != end_iter)
	{
		if (*cur_word.curchar())
			return false;

		switch (next_word) {
		case next_do_equals:
			cur_word=word_iter("=", false);
			next_word=next_do_value;
			return false;
		case next_do_value:
			cur_word=word_iter(begin_iter->second.c_str(), true);
			next_word=next_do_advance;
			break;
		case next_do_advance:
			if (++begin_iter == end_iter)
				return true;
			cur_word=word_iter("&", false);
			next_word=next_do_param;
			return false;
		case next_do_param:
			cur_word=word_iter(begin_iter->first.c_str(), true);
			next_word=next_do_equals;
			break;
		}
	}
	return true;
}

const char *parametersObj::encode_iter::curchar() noexcept
{
	(void)eof();
	return cur_word.curchar();
}

void parametersObj::encode_iter::advance()
{
	if (!eof())
		cur_word.advance();
}

parametersObj::encode_iter::encode_iter(const const_parameters &parametersArg,
					bool endingIterator)
 : begin_iter(endingIterator ?
				      parametersArg->end() :
				      parametersArg->begin()),
			   end_iter(parametersArg->end()),
			   params(parametersArg)
{
	if (begin_iter != end_iter)
		cur_word=word_iter(begin_iter->first.c_str(), true);
	next_word=next_do_equals;
}

parametersObj::encode_iter::~encode_iter() noexcept
{
}

parametersObj::encode_iter parametersObj::encode_begin() const
{
	return encode_iter(const_parameters(this), false);
}

parametersObj::encode_iter parametersObj::encode_end() const
{
	return encode_iter(const_parameters(this), true);
}

//////////////////////////////////////////////////////////////////////////////

parametersObj::filereceiver::filereceiver()
{
}

parametersObj::filereceiver::~filereceiver() noexcept
{
}

parametersObj::filereceiverObj::filereceiverObj()
{
}

parametersObj::filereceiverObj::~filereceiverObj() noexcept
{
}

// Iterator over the contents of a multipart/form-data message.

// Defines and constructs various smaller internal object to handle its
// contents.
class LIBCXX_HIDDEN parametersObj::rfc2388Obj
	: public outputrefiteratorObj<int> {

 public:

	// Output iterator collects form-data section's into a string.

	class LIBCXX_PUBLIC formdataObj : virtual public obj {

	public:
		typedef std::iterator<std::output_iterator_tag, void,
				      void, void, void> iterator_traits;

		std::string value;

		formdataObj() LIBCXX_HIDDEN {}
		~formdataObj() noexcept LIBCXX_HIDDEN {}

		formdataObj &operator*() LIBCXX_HIDDEN { return *this; }
		formdataObj &operator++() LIBCXX_HIDDEN { return *this; }
		formdataObj *before_postoper() LIBCXX_HIDDEN
		{
			return this;
		}

		void operator=(char c) LIBCXX_HIDDEN
		{
			value.push_back(c);
		}
	};

	// Output iterator over contents of a received file. Collects characters
	// into chunks, which are passed along, piecemeal.

	class LIBCXX_PUBLIC receiverObj : virtual public obj {

	public:

		typedef std::iterator<std::output_iterator_tag, void,
				      void, void, void> iterator_traits;

		ref<filereceiverObj> recv;

		std::vector<char> buffer;
		size_t buffer_size;

		receiverObj(const ref<filereceiverObj> &recvArg)
			LIBCXX_HIDDEN : recv(recvArg),
			  buffer_size(fdbaseObj::get_buffer_size())
		{
			buffer.reserve(buffer_size);
		}

		~receiverObj() noexcept LIBCXX_HIDDEN {}

		receiverObj &operator*() LIBCXX_HIDDEN { return *this; }
		receiverObj &operator++() LIBCXX_HIDDEN { return *this; }
		receiverObj *before_postoper() LIBCXX_HIDDEN
		{
			return this;
		}

		void operator=(char c) LIBCXX_HIDDEN
		{
			if (buffer.size() >= buffer_size)
			{
				recv->write(buffer);
				buffer.clear();
			}
			buffer.push_back(c);
		}

		// End of section, flush
		void close() LIBCXX_HIDDEN
		{
			if (!buffer.empty())
				recv->write(buffer);
			recv->close();
		}
	};

	class decoderObj;

	class multipartMixedObj;

	typedef refiterator<decoderObj> sectiondecoder_t;

	typedef mime::section_iter<sectiondecoder_t> sectioniter_t;

	typedef mime::section_iterptr<sectiondecoder_t> sectioniterptr_t;

	// Output iterator processes a single multipart/form-data section.

	// Collects its headers, then figures out what to do next.

	class sectionObj : virtual public obj {

	public:
		typedef std::iterator<std::output_iterator_tag, void,
				      void, void, void> iterator_traits;

		// Counts down from getformmaxsize()
		size_t formmaxsize;

		// What it was in the constructor
		size_t formmaxsize_orig;

		// Factory for creating file receivers
		const filereceiverfactorybase &factory;

		// Original parameter object that's being constructed
		parametersObj &params;

		// Form's original encoding
		std::string form_encoding;

		// Whether this is recursively constructed to deal with a
		// multipart/mixed section.

		bool in_multipart_mixed;

		// Collector for the header portion of the section.
		mime::contentheader_collector headers;

		typedef mime::header_iter<mime::contentheader_collector
					  > parser_t;
		// Output iterator that feeds headers.
		parser_t parser;

		// Parsed field name, converted to UTF-8.
		std::string form_name;

		// Collector for a form-data field value, when reading the
		// body section.
		ptr<formdataObj> formdataptr;

		// Content transfer decoder for the body section.
		outputrefiteratorptr<int> bodydecoder;

		// File receiver, when the section turns out to be an upload.
		ptr<receiverObj> receiver;

		// The recursively-constructed multipart/mixed section
		ptr<multipartMixedObj> multipart_mixed;

		// Flag, we should be receiving something.
		bool in_body;

		sectionObj(size_t formmaxsizeArg,
			   const filereceiverfactorybase &factoryArg,
			   parametersObj &paramsArg,
			   bool in_multipart_mixedArg,
			   const std::string &form_encodingArg)
			: formmaxsize(formmaxsizeArg),
			  formmaxsize_orig(formmaxsizeArg),
			  factory(factoryArg),
			  params(paramsArg),
			  form_encoding(form_encodingArg),
			  in_multipart_mixed(in_multipart_mixedArg),
			  headers(mime::contentheader_collector::create(false)),
			  parser(parser_t::create(headers)),
			  in_body(false)
		{
		}

		~sectionObj() noexcept {}

		// Iterator operators
		sectionObj &operator*() { return *this; }
		sectionObj &operator++() { return *this; }
		sectionObj *before_postoper()
		{
			return this;
		}

		void operator=(int c);
	};

	// "section_start" constructs a new section, then diverts the output
	// stream to it, until "section_end".

	class decoderObj : virtual public obj {

	public:
		typedef std::iterator<std::output_iterator_tag, void,
				      void, void, void> iterator_traits;

		const filereceiverfactorybase &factory;

		parametersObj &params;
		std::string form_encoding;

		// Counts down from getformmaxsize()
		size_t formmaxsize;

		// Whether this is recursively constructed for a multipart/mixed
		// section.
		bool in_multipart_mixed;

		// The headers for the current section
		ptriterator<sectionObj> current_section_headers;

		typedef mime::bodystart_iterptr< refiterator<sectionObj>
						 > current_section_t;

		//! The current section's body iterator
		current_section_t current_section;

		decoderObj(const filereceiverfactorybase &factoryArg,
			   parametersObj &paramsArg,
			   bool in_multipart_mixedArg,
			   const std::string &form_encodingArg)
			: factory(factoryArg), params(paramsArg),
			  form_encoding(form_encodingArg),
			  formmaxsize(getformmaxsize()),
			  in_multipart_mixed(in_multipart_mixedArg)
		{
		}

		~decoderObj() noexcept
		{
		}

		decoderObj &operator*() { return *this; }
		decoderObj &operator++() { return *this; }
		decoderObj *before_postoper()
		{
			return this;
		}

		void operator=(int c)
		{
			if (c == mime::section_start)
			{
				auto s=refiterator<sectionObj>
					::create(formmaxsize, factory, params,
						 in_multipart_mixed,
						 form_encoding);
				current_section=current_section_t::create(s);
				current_section_headers=s;
				return;
			}

			if (!current_section.null())
			{
				if (c == mime::section_end)
				{
					*current_section++=mime::eof;

					// Keep track of consumed quota
					if (!current_section_headers.null())
						formmaxsize=
							current_section_headers
							.get()->formmaxsize;
					current_section=current_section_t();
					current_section_headers=
						ptriterator<sectionObj>();
				}
				else
					*current_section++=c;
				return;
			}
		}
	};

	typedef mime::newline_iter<sectioniter_t> newlineiter_t;

	newlineiter_t newlineiter;

	rfc2388Obj(const std::string &boundaryArg,
		   const filereceiverfactorybase &factoryArg,
		   parametersObj &paramsArg,
		   const std::string &form_encoding)
		: newlineiter(newlineiter_t::create
			      (sectioniter_t::create
			       (sectiondecoder_t::create(factoryArg,
							 paramsArg, false,
							 form_encoding),
				boundaryArg),
			       true // HTTP 1.1 uses CRLF!
			       ))
	{
	}

	~rfc2388Obj() noexcept {}

	void operator=(int c) { *newlineiter++=c; }
};

class LIBCXX_HIDDEN parametersObj::rfc2388Obj::multipartMixedObj
	: public outputrefiteratorObj<int> {

 public:

	sectioniter_t iter;

	ref<decoderObj> decoder;

	multipartMixedObj(sectioniter_t &&iterArg,
			  const ref<decoderObj> &decoderArg)
		: iter(std::move(iterArg)), decoder(decoderArg)
	{
	}

	~multipartMixedObj() noexcept
	{
	}

	void operator=(int c) override
		{
			*iter++=c;
		}
};

void parametersObj::rfc2388Obj::sectionObj::operator=(int c)
{
	// Limit the maximum size of headers, and form-data
	// fields.

	if (mime::nontoken(c) &&
	    (!in_body || !formdataptr.null()) && --formmaxsize == 0)
		responseimpl::throw_request_entity_too_large();

	*parser++=c;

	if (in_body)
		*bodydecoder=c;

	if (c == mime::body_start)
	{
		// Collected the headers, figure out what's
		// next.

		auto &h=*headers.get();

		mime::structured_content_header
			ct(h.content_headers,
			   mime::structured_content_header
			   ::content_type);

		mime::structured_content_header
			cd(h.content_headers,
			   mime::structured_content_header
			   ::content_disposition);

		std::string filename;

		// Get field name
		form_name=cd.decode_utf8("name", form_encoding);

		// When NOT in a multipart/mixed section, the top level
		// section, see if this is a multipart section (and assume
		// that it's mixed). This blocks multipart inside another
		// multipart.

		if (!in_multipart_mixed &&
		    ct.mime_content_type() == "multipart")
		{
			auto decoder=ref<decoderObj>::create(factory, params,
							     true,
							     form_encoding);

			bodydecoder=multipart_mixed=
				ref<multipartMixedObj>::create
				(sectioniter_t::create
				 (sectiondecoder_t(decoder),
				  ct.boundary()), decoder);
		}
		else if ((filename=cd.decode_utf8("filename",
						  form_encoding)).empty())
		{
			// Credit for good behavior
			formmaxsize=formmaxsize_orig;

			formmaxsize -= form_name.size();

			// Decode the MIME section to UTF-8,
			// collect the field value.

			auto value=make_refiterator
				(ref<formdataObj>::create());

			formdataptr=value;
			bodydecoder=mime::section_decoder::create
				(h.content_headers, "UTF-8", value);
		}
		else
		{
			// Receive this file.

			auto next_receiver=factory.next(h.content_headers,
							form_name,
							filename);

			// If no file receiver was provided,
			// abort.

			if (next_receiver.null())
				responseimpl::throw_not_found();

			// Save the receiver object, so we can invoke close()
			receiver=ref<receiverObj>::create(next_receiver);

			bodydecoder=mime::section_decoder::create
				(h.content_headers,
				 refiterator<receiverObj>(receiver));
		}
		in_body=true;
		return;
	}

	if (c == mime::eof)
	{
		// Finish the section.

		if (!formdataptr.null())
			params.insert(std::make_pair(form_name,
						     formdataptr->value));
		if (!receiver.null())
			receiver->close();

		if (!multipart_mixed.null())
			// Keep on top of things...
			formmaxsize=multipart_mixed->decoder->formmaxsize;
	}
}

outputrefiterator<int>
parametersObj::
create_multipart_formdata_iterator(const std::string &boundary,
				   const filereceiverfactorybase &factoryArg,
				   parametersObj &me,
				   const std::string &form_encoding)
{
	return ref<rfc2388Obj>::create(boundary, factoryArg, me,
				       form_encoding);
}

#if 0
{
	{
		{
#endif
		}
	}
}
