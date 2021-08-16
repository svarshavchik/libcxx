/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_messageimplfwd_H
#define x_http_messageimplfwd_H

#include <string>
#include <set>
#include <functional>
#include <cctype>

#include <x/headersimpl.H>
#include <x/exception.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

//! HTTP version

typedef enum class httpver_enum_t {
	httpnone=-1,
	http11,
	http10,
} httpver_t;

//! An HTTP/1.1 message

//! An HTTP message is, basically, a list of headers, so this is just a
//! subclass of \ref headersimpl "headersimpl", an
//! implementation of \ref headersbase "headersbase" that uses the \c CRLF
//! newline sequence.

class messageimpl : public headersimpl<headersbase::crlf_endl> {

protected:

	//! The start line in the HTTP 1.1 message
	std::string start_line;

public:
	//! Constructor
	messageimpl() noexcept;

	//! Constructor
	messageimpl(const headersbase &headers);

	//! Destructor
	~messageimpl();

	//! Parse an HTTP/1.1 message out of some input sequence

	//! \return The iterator after the terminating blank line has been
	//! read from the sequence.
	//!
	//! A parsing error throws an exception.

	template<typename iter_type>
	iter_type parse(//! Beginning input sequence iterator
			iter_type beg_iter,

			//! Ending input sequence iterator
			iter_type end_iter,

			//! Maximum number of headers to parse (0-unlimited)
			size_t maxlimit);

	//! Parse "HTTP/1.x" from an input sequence

	template<typename iter_type>
	iter_type httpver_parse(//! Beginning input sequence iterator
				iter_type b,
				//! Ending input sequence itertor
				iter_type e,
				//! Parsed http version
				httpver_t &httpver)
;

	//! Convert an HTTP version to string, write it to an output sequence

	template<typename iter_type>
	static iter_type httpver_toString(iter_type iter, httpver_t httpver)
;

	//! Case-insensitive set type

	typedef std::set<std::string, chrcasecmp::str_less> keywordseti_t;

	//! Parse a header that contains comma-separated values.

	//! Multiple occurences of this header get parsed
	//!
	template<typename iter_type>
	iter_type getCommaValues(//! The header's name
				 const char *headername,

				 //! Output iterator which received std::strings

				 iter_type outputIterator)
		const;

	//! Parse a header that contains comma-separated values into a keywordseti_t

	//! Multiple occurences of this header get parsed
	//!
	void getCommaValuesToSet(//! The header's name
				 const char *headername,

				 //! The keywords get loaded into this set.
				 keywordseti_t &ks) const;

	//! An error has occured parsing the message

	//! The subclass throws an appropriate exception, for this case.
	//!
	virtual void bad_message() __attribute__((noreturn))=0;
public:

	//! The "Content-Length" header name

	static const char content_length[];

	//! The "Transfer-Encoding" header name

	static const char transfer_encoding[];

	//! "chunked"

	static const char transfer_encoding_chunked[];

	//! The "Connection" header name

	static const char connection[];

	class commaValuesPredicate;

	//! Check if Connection: header contains the "closed" keyword.

	bool connection_closed() const;

	//! The "Expect" header name

	static const char expect[];

	//! The 100-continue keyword

	static const char expect_100_continue[];
};

#if 0
{
#endif
}
#endif
