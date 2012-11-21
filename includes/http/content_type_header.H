/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_content_type_header_H
#define x_http_content_type_header_H

#include <x/http/parameter.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	class headersbase;

	namespace http {

#if 0
	};
};
#endif


//! Parsed contents of an HTTP Content-Type: header

//! See RFC 2616, section 14.17
//!

class content_type_header {

public:

	//! The "Content-Type" header name

	//! \internal
	//!
	static const char name[];

	//! The \c application/x-www-form-urlencoded content type

	static const char application_x_www_form_urlencoded[];

	//! Media type
	std::string type;

	//! Media subtype
	std::string subtype;

	//! media parameters and accept parameters

	//! Keyed by case-insensitive attribute name,
	//! value is the attribute value

	typedef std::multimap<std::string, parameter_t, chrcasecmp::str_less
			      > parameters_t;

	//! Media parameters
	parameters_t parameters;

	//! Default constructor
	content_type_header() noexcept;

	//! Default destructor
	~content_type_header() noexcept;

	//! Construct from HTTP headers
	content_type_header(const headersbase &);

	//! Construct from HTTP headers
	content_type_header &operator=(const headersbase &);

	//! Construct from a string
	content_type_header(const std::string &);

	//! Construct from a string
	content_type_header(const char *);

	//! Construct from a string
	content_type_header &operator=(const char *);

	//! Construct from a string
	content_type_header &operator=(const std::string &);

	//! Compare type/subtype

	//! \return \c true if the \c mimetype parameter is "type/subtype"
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
		iter=std::copy(type.begin(), type.end(), iter);

		*iter++='/';

		iter=std::copy(subtype.begin(), subtype.end(), iter);

		for (parameters_t::const_iterator
			     b(parameters.begin()), e(parameters.end());
		     b != e; ++b)
		{
			*iter++=';';
			*iter++=' ';
			iter=b->toString(iter);
		}
		return iter;
	}
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
