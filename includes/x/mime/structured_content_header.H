/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mime_structured_content_header_H
#define x_mime_structured_content_header_H

#include <x/mime/parameter.H>
#include <x/headersimpl.H>
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

	//! The \c message/rfc822 content type
	static const char message_rfc822[];

	//! The \c message/global content type
	static const char message_global[];

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
	~structured_content_header();

	//! Construct with just a value parameter
	structured_content_header(const std::string &valueArg);

	//! Construct from headers
	structured_content_header(const headersbase &req,
				  const std::string &name);

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

	//! Convenience function - append a parameter

	structured_content_header &operator+=(const parameter_t &param);

	//! Convenience function - append a parameter

	structured_content_header &operator+=(parameter_t &&param);

	//! Convenience function - append a parameter

	structured_content_header &operator()(const std::string &name,
					      const std::string &value);

	//! Convenience function - append a parameter

	//! Perform RFC 2231 encoding, if necessary.

	structured_content_header &rfc2231(const std::string &name,
					   const std::string &value,
					   const std::string &charset,
					   size_t width=76);

	//! Convenience function - append a parameter

	//! Perform RFC 2231 encoding, if necessary.

	structured_content_header &rfc2231(const std::string &name,
					   const std::string &value,
					   const std::string &charset,
					   const std::string &language,
					   size_t width=76);

	//! Convenience function - append a parameter

	//! Perform RFC 2047 encoding, if necessary.

	structured_content_header &rfc2047(const std::string &name,
					   const std::string &value,
					   const std::string &charset);

	//! Convenience function - append a parameter

	//! Perform RFC 2047 encoding, if necessary.

	structured_content_header &rfc2047(const std::string &name,
					   const std::string &value,
					   const std::string &charset,
					   const std::string &language);

private:
	class LIBCXX_HIDDEN formatwords_cb;

	struct LIBCXX_HIDDEN format_cb_string;
	template<typename header_type>
	struct LIBCXX_HIDDEN format_words_wrap;

	//! Format the structured header.

	void format(formatwords_cb &callback) const;

public:
	//! Append to existing headers

	template<typename header_type>
	void append(headersimpl<header_type> &headers,
		    const std::string &name,
		    size_t maxwidth=76);

	//! Format the header as a single string

	operator std::string() const;

	//! Return the MIME content type, in lowercase, default "text".

	std::string mime_content_type() const;

	//! Return the MIME content subtype, in lowercase, default "plain".

	std::string mime_content_subtype() const;

	//! Whether the MIME content type/subtype is message/rfc822 or message/global
	bool is_message() const;

	//! Whether the MIME content type is multipart
	bool is_multipart() const;

	//! Return the charset parameter, if not present return the default.
	std::string charset(const std::string &default_charset) const;

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
