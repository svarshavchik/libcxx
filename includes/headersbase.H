/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_headersbase_H
#define x_headersbase_H

#include <string>
#include <map>
#include <x/chrcasecmp.H>
#include <x/getlinecrlf.H>
#include <x/exception.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
	namespace http {
		class cgiimpl;
	};
#if 0
};
#endif

//! Headers

//! Common logic for handling generic "headers", as in HTTP or Email/MIME
//! headers. The headers get folded and parsed into a name and a value.
//! The recognized end of line character sequence may be either LF or CRLF.
//! Two internal classes provide traits that determine the end-of-line
//! sequence, which is then used by the headersimpl template subclass.
//!
//! Internally, the headers are saved in a list structure, preserving their
//! original order. A multimap gets keyed on a header's case-insensitive name,
//! so multiple occurences of a given header may be processed.

class headersbase {

public:
	//! Copy constructor

	//! The copy constructor is an expensive operation.
	//!
	headersbase(const headersbase &);

	//! Assignment operator

	//! The copy constructor is an expensive operation.
	//!

	headersbase &operator=(const headersbase &);

	//! A stored header

	class header : public std::string {
	public:
		//! Constructor
		header(const std::string &value) noexcept
		: std::string(value)
		{
		}

		//! Move constructor
		header(std::string &&value) noexcept
		: std::string(std::move(value))
		{
		}

		//! Destructor
		~header() noexcept
		{
		}

		//! Return the header's name
		std::string name() const noexcept;

		//! Return the beginning iterator for the header's value

		std::string::const_iterator begin() const noexcept;

		//! Return the ending iterator for the header's value

		std::string::const_iterator end() const noexcept;

		//! Return the header value as an ordinary \c std::string

		//! This is more expensive. Use iterators where possible.

		std::string value() const noexcept
		{
			return std::string(begin(), end());
		}
	};

	//! A container for headers in their original order

	typedef std::list<header> headerlist_t;

	//! An associative container of headers, keyed by header name.

	class header_map_val_t {

		//! Where in headerlist this header is.
		headerlist_t::iterator headerp;

	public:
		friend class headersbase;

		//! Constructor
		header_map_val_t(headerlist_t::iterator headerpArg)
			noexcept : headerp(headerpArg)
		{
		}

		//! Destructor

		~header_map_val_t() noexcept
		{
		}

		//! Return the header's name
		std::string name() const noexcept
		{
			return headerp->name();
		}


		//! Return the beginning iterator for the header's value

		std::string::const_iterator begin() const noexcept
		{
			return headerp->begin();
		}

		//! Return the ending iterator for the header's value

		std::string::const_iterator end() const noexcept
		{
			return headerp->end();
		}

		//! Return the header value as an ordinary \c std::string

		//! This is more expensive. Use valueiter() where possible.

		std::string value() const noexcept
		{
			return headerp->value();
		}
	};

	//! A multimap type for looking up headers

	typedef std::multimap<std::string, header_map_val_t,
			      chrcasecmp::str_less> map_t;

protected:
	//! The headers, in original order

	//! The headers are folded. The trailing LF or CRLF is removed from
	//! the string. However, any embedded LF or CRLF is preserved when
	//! the header is folded.

	headerlist_t headerlist;

	//! A multimap that looks up headers.
	map_t headermap;

public:

	//! Constructor
	headersbase() noexcept;

	//! Destructor
	~headersbase() noexcept;

	//! Iterator type returned by lookup methods.

	typedef map_t::const_iterator const_iterator;

	//! Modifiable iterator

	typedef map_t::iterator iterator;

	//! Search for a given header.

	//! \return a \c std::pair giving the beginning and ending iterator
	//! for the header's value(s), if any.

	std::pair<const_iterator, const_iterator>
	equal_range(const std::string &name) const noexcept
	{
		return headermap.equal_range(name);
	}

	//! Search for a given header.

	//! \overload

	std::pair<iterator, iterator>
	equal_range(const std::string &name) noexcept
	{
		return headermap.equal_range(name);
	}

	//! Return the beginning iterator for the headers

	iterator begin() noexcept { return headermap.begin(); }

	//! Return the beginning iterator for the headers

	const_iterator begin() const noexcept { return headermap.begin(); }

	//! Return the ending iterator for the headers

	iterator end() noexcept { return headermap.end(); }

	//! Return the ending iterator for the headers

	const_iterator end() const noexcept { return headermap.end(); }

	//! Look up a header

	//! A single occurence of this header is expected.
	//!
	const_iterator find(const std::string &name) const noexcept
	{
		return headermap.find(name);
	}

	//! Look up a header

	//! \overload
	//!
	iterator find(const std::string &name) noexcept
	{
		return headermap.find(name);
	}

	//! Erase a header

	void erase(iterator iter)
	{
		headerlist.erase(iter->second.headerp);
		headermap.erase(iter);
	}

	//! Erase headers

	//! \overload
	void erase(iterator beg_iter, iterator end_iter)
	{
		while (beg_iter != end_iter)
		{
			iterator p=beg_iter;

			++beg_iter;
			erase(p);
		}
	}

	//! Erase headers

	//! \overload
	void erase(const std::pair<iterator, iterator> &p)
	{
		erase(p.first, p.second);
	}

	//! Erase headers

	//! \overload
	void erase(const std::string &name)
	{
		erase(equal_range(name));
	}

	//! Erase headers

	//! \overload
	void erase(const char *name)
	{
		erase(equal_range(name));
	}

	//! A header context where header lines are terminated by \c CRLF

	//! Use this context class for parsing headers where header lines are
	//! expected to be terminated by the \c CRLF sequence.

	class crlf_endl {

	public:

		//! Invoke getlinecrlf() to retrieve the next line.

		template<typename iter_type, typename container_type>
		static std::pair<iter_type, bool>
		line(//! Beginning input iterator
		     iter_type beg_iter,

		     //! Ending input iterator
		     iter_type end_iter,

		     //! The container to receive the line.

		     //! This is typically a \c std::string.
		     //!
		     container_type &cont)

		{
			return getlinecrlf(beg_iter, end_iter, cont);
		}

		//! The \c CRLF sequence for this header context.
		static const char eol_str[];
	};

	//! A header context where header lines are terminated by \c LF

	//! Use this context class for parsing headers where header lines are
	//! expected to be terminated by the \c LF sequence.

	class lf_endl {

	public:
		//! Invoke getlinelf() to retrieve the next line.

		template<typename iter_type, typename container_type>
		static std::pair<iter_type, bool>
		line(//! Beginning input iterator
		     iter_type beg_iter,

		     //! Ending input iterator
		     iter_type end_iter,

		     //! The container to receive the line.

		     //! This is typically a \c std::string.
		     //!
		     container_type &cont)

		{
			return getlinelf(beg_iter, end_iter, cont);
		}

		//! The \c CRLF sequence for this header context.

		static const char eol_str[];
	};

protected:

	//! Insert a new header

	//! \internal
	//!
	iterator new_header(const std::string &line);

public:
	friend class http::cgiimpl;

	//! Return the list of headers, for iteration purposes

	headerlist_t const &list() const noexcept
	{
		return headerlist;
	}

protected:
	//! An end of sequence encountered while parsing headers.
	static void parse_endofstream() __attribute__((noreturn));

	//! Number of headers exceeds maximum.
	static void parse_toomanyheaders() __attribute__((noreturn));
protected:

	//! Internal class used by parsetokenheader

	//! Iterates over all headers of the given name, returning a sequence
	//! of words that are terminated by commas or semicolons. For these
	//! purposes, each header line is considered to be terminated with a
	//! comma.
	//!
	//! Quoted content is unquoted. Whitespace is homogenized to ' '.
	//!
	class tokenhdrparser {

		//! Current header iterator
		const_iterator hdr_start;

		//! Ending header iterator
		const_iterator hdr_end;

		//! Current header value iterator
		std::string::const_iterator val_start;

		//! Ending header value iterator
		std::string::const_iterator val_end;

		//! Advance iterators to the next header
		void newhdr();

	public:
		//! Constructor
		tokenhdrparser(//! Name of header to parse
			       const std::string &headername,

			       //! Headers
			       const headersbase &headers);

		//! Destructor
		~tokenhdrparser() noexcept;

		//! Parsed value
		std::string value;

		//! Parse the next value

		//! \return 0 - no more values, else the terminator character,
		//! ',' or ';'

		char operator()(void);

	};

public:

	//! Parse a header that contains a list of parameter tokens

	//! parsetokenheader() extracts token from a header that specifies
	//! some sort of parameter sequences.
	//!
	//! The template parameter is an object that implements
	//! operator()(const std::string &, char).
	//!
	//! The header is broken up into a sequence of tokens, separated by
	//! ',' and ';' characters. This function basically gobbles up
	//! characters in a header until it sees a ',' and ';' character.
	//!
	//! parsetokenheader() invokes the callback(token, char)
	//! passing the parsed token. The second parameter is a char,
	//! indicating whether the token was terminated by ';' or ','.
	//! The last token in the header is considered to be terminated by the
	//! ',' character. If the header occurs multiple times, the header
	//! occurences are combined using the ',' character, and the last token
	//! in each occurence of the header is processed accordingly.
	//!
	//! The callback function receives the parsed token value as the
	//! the first parameter. Quoted
	//! values have the surrounding quotes stripped, and backslashes
	//! removed from escaped characters in quoted values.
	//! Leading and trailing whitespace is stripped.

	template<typename callback_type>
	void parsetokenheader(const std::string &headername,
			      callback_type &&callback)
		const
	{
		tokenhdrparser parser(headername, *this);

		char value;

		while ((value=parser()) != 0)
			callback(parser.value, value);
	}


	//! Write the headers to an output sequence

	template<typename iter_type>
	iter_type toString(//! Output iterator
			   iter_type iter,

			   //! The line terminator is overriden in http::cgiimpl

			   const char *newline) const
	{
		for (const std::string &header:this->list())
		{
			iter=std::copy(header.begin(), header.end(), iter);

			for (const char *p=newline; *p; p++)
				*iter++ = *p;
		}
		for (const char *p=newline; *p; p++)
			*iter++ = *p;
		return iter;
	}

	//! Throw an exception, invalid character in token.

	static void invalid_char_in_token() __attribute__((noreturn));
};


#ifndef DOXYGEN

extern template
std::ostreambuf_iterator<char>
headersbase::toString<std::ostreambuf_iterator<char>
		      >(std::ostreambuf_iterator<char>, const char *)
	const;

#endif




#if 0
{
#endif
}
#endif
