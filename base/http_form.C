/*
** Copyright 2012-2019 Double Precision, Inc.
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
#include "x/mime/entityparser.H"
#include "x/mime/rfc2047.H"
#include "x/refiterator.H"
#include "gettext_in.h"

#define LIBCXX_TEMPLATE_DECL
#include "x/http/useragentobj_t.H"
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
#include "x/http/form_t.H"
#undef LIBCXX_TEMPLATE_DECL

property::value<size_t> LIBCXX_HIDDEN
formmaxsize(LIBCXX_NAMESPACE_STR "::http::form::maxsize", 1024 * 1024 * 10);

size_t getformmaxsize()
{
	return formmaxsize.get();
}

const char hex[]="0123456789ABCDEFabcdef";

parametersObj::parametersObj() : consumedFlag(false)
{
}

parametersObj::~parametersObj()
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

parametersObj::encode_iter::~encode_iter()
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

parametersObj::filereceiver::~filereceiver()
{
}

parametersObj::filereceiverObj::filereceiverObj()
{
}

parametersObj::filereceiverObj::~filereceiverObj()
{
}

// Iterator over the contents of a multipart/form-data message.

// create_multipart_formdata_iterator() saves the parameters it receives
// in an instance of this object, then uses MIME entity parsing templates
// to handle the form decoding, using a captured ref to this object.

class LIBCXX_HIDDEN parametersObj::rfc2388Obj : virtual public obj {

 public:

	// Parameters received by create_multipart_formdata_iterator()
	const filereceiverfactorybase &factory;
	parametersObj &params;
	std::string form_encoding;

	// Counts down maximum allowed form size
	size_t formmaxsize;

	// Output iterator that receives decoded field contents. Puts it into
	// the form parameters object.

	class LIBCXX_PUBLIC decodeParameterObj
		: public outputrefiteratorObj<char> {

	public:
		ref<rfc2388Obj> me;

		// Iterator to the inserted field value in parametersObj.
		// Before we start decoding the value of this field, we
		// insert our starting point, an empty string.

		parametersObj::iterator valueiter;

		decodeParameterObj(const ref<rfc2388Obj> &meArg,
				   const parametersObj::iterator &valueiterArg)
			LIBCXX_HIDDEN : me(meArg), valueiter(valueiterArg)
		{
		}

		~decodeParameterObj() LIBCXX_HIDDEN
		{
		}

		void operator=(char c) final override LIBCXX_HIDDEN
		{
			if (me->formmaxsize == 0)
				responseimpl::throw_request_entity_too_large();
			--me->formmaxsize;
			valueiter->second.push_back(c);
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

		receiverObj(const ref<filereceiverObj> &recvArg) LIBCXX_HIDDEN
			: recv(recvArg),
			  buffer_size(fdbaseObj::get_buffer_size())
		{
			buffer.reserve(buffer_size);
		}

		~receiverObj() LIBCXX_HIDDEN {}

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

	// A wrapper that invokes close() when the entire file contents
	// are received. This is a wrapper for the instantiated section decoder.

	class receiverWrapperObj : public outputrefiteratorObj<int> {

	public:
		// The constructed receiver

		ref<receiverObj> receiver;

		// Declare the sectiondecoder that handles the actual decoding.
		typedef decltype(mime::section_decoder::create
				 (headersbase(),
				  refiterator<receiverObj>(ptr<receiverObj>()))
				 ) iter_t;
		iter_t iter;

		// Constructor
		receiverWrapperObj(const ref<receiverObj> &receiverArg,
				   iter_t && iterArg)
			: receiver(receiverArg), iter(iterArg)
		{
		}

		~receiverWrapperObj()
		{
		}

		// Pass through everything to sectiondecoder, but wake up when
		// we see an eof.
		void operator=(int c) final override
		{
			*iter=c;

			if (c == mime::eof)
				receiver->close();
		}
	};

	rfc2388Obj(const filereceiverfactorybase &factoryArg,
		   parametersObj &paramsArg,
		   const std::string &form_encodingArg)
		: factory(factoryArg),
		params(paramsArg),
		form_encoding(form_encodingArg),
		formmaxsize(getformmaxsize())
	{
	}

	~rfc2388Obj()
	{
	}

	// MIME section processor factory. Returns an output iterator that
	// decodes the next MIME section in a multipart/form-data request.

	static outputrefiterator<int>
		get_parser(const ref<rfc2388Obj> &me,
			   const mime::sectioninfo &info,
			   bool flag)
	{
		auto header_iter=
			x::mime::contentheader_collector::create(false);
		auto headers=header_iter.get();

		return x::mime::make_entity_parser
			(header_iter,
			 [me, headers, info]
			 {
				 return parse_section(headers->content_headers,
						      me,
						      info);
			 }, info);
	}

	// Collected the headers of the next MIME section. Now what?

	static outputrefiterator<int>
		parse_section(const headersbase &headers,
			      const ref<rfc2388Obj> &me,
			      const mime::sectioninfo &info)
	{
		mime::structured_content_header
			content_type(headers,
				     mime::structured_content_header
				     ::content_type);

		// Handle multipart/mixed, as per RFC 2388.

		// This is done by instantiating a MIME multipart parser that
		// recursively invokes get_parser(), as the section processor
		// factory.

		if (content_type.is_multipart())
			return mime::make_multipart_parser
				(content_type.boundary(),
				 [me]
				 (const mime::sectioninfo &info, bool flag)
				 {
					 return rfc2388Obj::get_parser(me,
								       info,
								       flag);
				 }, info);

		mime::structured_content_header
			content_disposition(headers,
					    mime::structured_content_header
					    ::content_disposition);
		// Get field name
		std::string form_name=
			content_disposition.decode_utf8("name",
							me->form_encoding);

		if (form_name.size()+1 > me->formmaxsize)
			responseimpl::throw_request_entity_too_large();

		me->formmaxsize-=form_name.size()+1;

		std::string filename=
			content_disposition.decode_utf8("filename",
							me->form_encoding);

		// If the filename field is not present, return an output
		// iterator that captures the value of a non-file upload field.

		if (filename.empty())
		{
			return mime::section_decoder::create
				(headers, "ISO-8859-1",
				 "UTF-8",
				 make_refiterator
				 (ref<decodeParameterObj>
				  ::create(me, me->params.insert(std::make_pair
								 (form_name,
								  "")))));
		}

		auto next_receiver=me->factory.next(headers,
						    form_name,
						    filename);

		// If no file receiver was provided,
		// abort.

		if (next_receiver.null())
			responseimpl::throw_not_found();

		// Save the receiver object, so we can invoke close()
		auto receiver=ref<receiverObj>::create(next_receiver);

		return ref<receiverWrapperObj>
			::create(receiver, mime::section_decoder::create
				 (headers,
				  refiterator<receiverObj>(receiver)));
	}
};

outputrefiterator<int>
parametersObj::
create_multipart_formdata_iterator(const std::string &boundary,
				   const filereceiverfactorybase &factoryArg,
				   parametersObj &me,
				   const std::string &form_encoding)
{
	auto args=ref<rfc2388Obj>::create(factoryArg, me,
					  form_encoding);
	return mime::make_multipart_parser
		(boundary,
		 [args]
		 (const mime::sectioninfo &info, bool flag)
		 {
			 return rfc2388Obj::get_parser(args, info, flag);
		 },
		 mime::sectioninfo::create());
}

#if 0
{
	{
		{
#endif
		}
	}
}
