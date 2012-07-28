/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_messageimpl_H
#define x_http_messageimpl_H

#include <iterator>
#include <algorithm>
#include <functional>
#include <cctype>
#include <set>
#include <list>
#include <vector>
#include <x/http/messageimplfwd.H>
#include <x/http/responseimpl.H>
#include <x/http/exception.H>
#include <x/uriimpl.H>
#include <x/getlinecrlf.H>
#include <x/chrcasecmp.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	//! Classes for handling HTTP messages

	//! These classes provided building blocks for implementing applications
	//! that send and receive HTTP messages. These classes are not meant
	//! to be used for implementing a generic, fast, HTTP server. Rather,
	//! they are more appropriate for implementing application servers.
	//!
	//! \ref useragent "http::useragent" is a useful, high level HTTP
	//! client.

	namespace http {
#if 0
	};
};
#endif


template<typename iter_type>
iter_type messageimpl::parse(//! Beginning input sequence iterator
			     iter_type beg_iter,

			     //! Ending input sequence iterator
			     iter_type end_iter,

			     //! Maximum number of headers to parse (0-unlimited)
			     size_t maxlimit)
{
	do
	{
		std::pair<iter_type, bool>
			p=getlinecrlf(beg_iter, end_iter, start_line);

		if (!p.second)
			bad_message();

		beg_iter=p.first;
	} while (start_line.empty());

	iter_type i;

	try {
		i=headersimpl<headersbase::crlf_endl>::parse(beg_iter,
							     end_iter,
							     maxlimit);
	} catch (const response_exception &e)
	{
		throw;
	} catch (const exception &e)
	{
		bad_message();
	}
	return i;
}

template<typename iter_type>
iter_type messageimpl::httpver_parse(iter_type b,
				     iter_type e,
				     httpver_t &httpver)

{
	iter_type p=std::find(b, e, '/');

	if (p == e ||
	    chrcasecmp::str_not_equal_to()(std::string(b, p), "HTTP"))
		bad_message();
	++p;

	unsigned major=0, minor=0;

	while (p != e)
	{
		if (!isdigit(*p))
			break;

		major=major * 10 + *p++ - '0';
	}

	if (p == e || *p++ != '.')
		bad_message();

	while (p != e)
	{
		if (!isdigit(*p))
			break;

		minor=minor * 10 + *p++ - '0';
	}

	if (major != 1)
		responseimpl::throw_http_version_not_supported();

	if (minor == 0)
	{
		httpver=http10;
	}
	else
	{
		httpver=http11;
	}
	return p;
}

template<typename iter_type>
iter_type messageimpl::httpver_toString(iter_type iter, httpver_t httpver)

{
	for (const char *p="HTTP/1."; *p; p++)
		*iter++= *p;
	*iter++ = httpver == http10 ? '0':'1';

	return iter;
}

//! Predicate class for splitting comma-separated values

class messageimpl::commaValuesPredicate
	: public std::unary_function<char, bool> {

public:
	//! Constructor
	commaValuesPredicate() noexcept {}

	//! Destructor
	~commaValuesPredicate() noexcept {}

	//! Predicate test for a delimiter
	bool operator()(char c) const noexcept
	{
		return strchr(", \r\t\n\v\b", c) != 0;
	}
};

template<typename iter_type>
iter_type messageimpl::getCommaValues(const char *headername,
				      iter_type outputIterator)
	const
{
	commaValuesPredicate predicate;

	for (std::pair<const_iterator, const_iterator>
		     headers=equal_range(headername);
	     headers.first != headers.second; ++headers.first)
	{
		for (std::string::const_iterator
			     b=headers.first->second.begin(),
			     e=headers.first->second.end();
		     (b=std::find_if(b, e, std::not1(predicate))) != e; )
		{
			std::string::const_iterator p=b;

			b=std::find_if(b, e, predicate);
			*outputIterator++=std::string(p, b);
		}
	}
	return outputIterator;
}

#ifndef DOXYGEN
extern template std::istreambuf_iterator<char>
messageimpl::parse(std::istreambuf_iterator<char>,
		   std::istreambuf_iterator<char>, size_t);
extern template std::istreambuf_iterator<char>
messageimpl::httpver_parse(std::istreambuf_iterator<char>,
			   std::istreambuf_iterator<char>,
			   httpver_t &);

extern template std::ostreambuf_iterator<char>
messageimpl::httpver_toString(std::ostreambuf_iterator<char>, httpver_t)
;

extern template std::insert_iterator<messageimpl::keywordseti_t>
messageimpl::getCommaValues(const char *headername,
			    std::insert_iterator<keywordseti_t>
			    outputIterator) const;

extern template std::back_insert_iterator<std::list<std::string> >
messageimpl::getCommaValues(const char *headername,
			    std::back_insert_iterator<std::list<std::string> >
			    outputIterator) const;

extern template std::back_insert_iterator<std::vector<std::string> >
messageimpl::getCommaValues(const char *headername,
			    std::back_insert_iterator<std::vector<std::string> >
			    outputIterator) const;

#endif


#if 0
{
	{
#endif
	}
}
#endif
