/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mime_structured_content_header_H
#define x_mime_structured_content_header_H

#include <x/mime/parameter.H>
#include <x/headersbase.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	class headersbase;

	namespace mime {

#if 0
	};
};
#endif

//! A structured MIME header.

//! With functions for parsing the HTTP Content-Type: header.
//! See RFC 2616, section 14.17

class structured_content_header {

public:

	//! The "Content-Type" header name

	//! \internal
	//!
	static const char content_type[];

	//! The "Content-Disposition" header name
	static const char content_disposition[];

	//! The "Content-Transfer-Encoding" header name
	static const char content_transfer_encoding[];

	//! The \c application/x-www-form-urlencoded content type

	static const char application_x_www_form_urlencoded[];

	//! The \c multipart/form-data content type

	static const char multipart_form_data[];

	//! First word, the value

	std::string value;

	//! media parameters and accept parameters

	//! Keyed by case-insensitive attribute name,
	//! value is the attribute value

	typedef std::multimap<std::string, parameter_t, chrcasecmp::str_less
			      > parameters_t;

	//! Media parameters
	parameters_t parameters;

	//! Default constructor
	structured_content_header() noexcept;

	//! Default destructor
	~structured_content_header() noexcept;

	//! Construct from headers
	structured_content_header(const headersbase &req,
				  const std::string &name);

	//! Construct from a string
	structured_content_header(const std::string &);

	//! Construct from a string
	structured_content_header(const char *);

	//! Construct from a string
	structured_content_header &operator=(const char *);

	//! Construct from a string
	structured_content_header &operator=(const std::string &);

	//! Compare value

	//! \return \c true if the \c value parameter is valueArg
	//!

	bool operator==(const std::string &mimetype) const;

	//! Compare type/subtype

	//! \return \c true if the \c mimetype parameter is not "type/subtype"
	//!
	bool operator!=(const std::string &mimetype) const
	{
		return !operator==(mimetype);
	}

	//! Convert the Accept: header value to a single string

	template<typename iter_type>
	iter_type toString(//! Output iterator
			   iter_type iter) const
	{
		iter=std::copy(value.begin(), value.end(), iter);

		for (parameters_t::const_iterator
			     b(parameters.begin()), e(parameters.end());
		     b != e; ++b)
		{
			*iter++=';';
			*iter++=' ';
			iter=b->second.toString(iter);
		}
		return iter;
	}

	//! Return the MIME content type, in lowercase, default "text".

	std::string mime_content_type() const;

	//! Return the MIME content subtype, in lowercase, default "plain".

	std::string mime_content_subtype() const;

	//! Return the charset parameter, if not present return ISO-8859-1
	std::string charset() const;

	//! Return the boundary parameter.
	std::string boundary() const;

	//! Return some parameter converted to UTF-8, using RFC 2047 or RFC 2231

	std::string decode_utf8(//! Parameter name
				const std::string &name,

				//! Unspecified charset is assumed to be this.
				const std::string &native_charset) const;
private:

	//! Helper object for parsing the HTTP request header
	class parser;
};

#if 0
{
	{
#endif
	}
}
#endif
