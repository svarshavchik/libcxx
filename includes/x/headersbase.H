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
		~header()
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

		~header_map_val_t()
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
	~headersbase();

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

		//! This is a CRLF-based header format
		static constexpr bool usecrlf=true;
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

		//! This is not a CRLF-based header format
		static constexpr bool usecrlf=false;
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

public:

	//! Parse a structured header

	//! Common algorithm used to parse the contents of a header
	//! structure as comma or semicolon-delimited words,
	//! with quoted content.

	template<typename isDelimiterCharFunctor,
		 typename parameterFunctor,
		 typename headerIterator>
	static void
	parse_structured_header(//! Functor: takes a char, returns true if char is a delimiter (typically ',' or ';').
				isDelimiterCharFunctor &&isDelimiter,

				//! Callback, first parameter is the return value from isDelimiter, second value is the parsed word.

				parameterFunctor &&parameter,

				//! Header value, beginning iterator
				headerIterator beginIter,

				//! Header value, ending iterator
				headerIterator endIter)
	{
		std::string value;

		// Mini-state machine.
		enum { notcollecting, collecting, skippedspaces, inquote
		} state=notcollecting;

		for ( ; beginIter != endIter; ++beginIter)
		{
			if (state == collecting || state == skippedspaces)
			{
				auto flag=isDelimiter(*beginIter);

				if (flag)
				{
					// End of one word, begni the next one.

					parameter(flag, value);
					state=notcollecting;
					value.clear();
					continue;
				}
			}

			switch (*beginIter) {
			case ' ':
			case '\t':
			case '\r':
			case '\n':
				if (state == collecting ||
				    state == skippedspaces)
				{
					// Outside of quotes, replace
					// a whitespace sequence with a single
					// space.

					state=skippedspaces;
					continue;
				}
				if (state == notcollecting)
					continue;
				break;
			case '"':
				if (state == skippedspaces)
					value.push_back(' ');
				if (state == inquote)
				{
					state=collecting;
				}
				else
				{
					state=inquote;
				}
				continue;
			}

			if (state == skippedspaces)
				value.push_back(' ');

			if (state != inquote)
				state=collecting;
			value.push_back(*beginIter);
		}

		if (state == collecting || state == inquote)
			parameter(0, value);
	}

	//! Something to pass for the first functor to parse_structured_header.

	//! Recognizes commas and colons as separators, returns them, or returns
	//! 0.

	static char comma_or_semicolon(char);

	//! Specialization for std::string::iterator

	//! Reduces code bloat, by using std::string::const_iterator

	template<typename isDelimiterCharFunctor,
		 typename parameterFunctor>
	void parse_structured_header(//! Functor: takes a char, returns true if char is a delimiter (typically ',' or ';').

				     isDelimiterCharFunctor &&isDelimiter,

				     //! Callback, first parameter is the return value from isDelimiter, second value is the parsed word.

				     parameterFunctor &&parameter,

				     //! Header value, beginning iterator
				     std::string::iterator beginIter,

				     //! Header value, ending iterator
				     std::string::iterator endIter)
	{
		parse_structured_header(std::string::const_iterator(beginIter),
					std::string::const_iterator(endIter),
					std::forward<isDelimiterCharFunctor>
					(isDelimiter),
					std::forward<parameterFunctor>
					(parameter));
	}

	//! Parse a structured header

	template<typename isDelimiterCharFunctor,
		 typename parameterFunctor,
		 typename container>
	static void
	parse_structured_header(//! Functor: takes a char, returns true if char is a delimiter (typically ',' or ';').

				isDelimiterCharFunctor &&isDelimiter,

				//! Callback, first parameter is the return value from isDelimiter, second value is the parsed word.

				parameterFunctor &&parameter,

				//! Container
				const container &c)
	{
		parse_structured_header(std::begin(c), std::end(c),
					std::forward<isDelimiterCharFunctor>
					(isDelimiter),
					std::forward<parameterFunctor>
					(parameter));
	}

	//! Parse a word into parameter=value

	//! Breaks a word at the first '=' sign, returns std::pair<name,value>.
	//! If there's no equal sign, the value is an empty string.

	static std::pair<std::string, std::string>
	name_and_value(//! The parameter
		       const std::string &word);

	//! Iterate over all instances of the given header, and do something.

	template<typename functorType>
	void process(//! Header's name
		     const std::string &headername,

		     //! Processing functor
		     functorType &&functor) const
	{
		for (auto values=this->equal_range(headername);
		     values.first != values.second; ++values.first)
		{
			functor(values.first->second.begin(),
				values.first->second.end());
		}
	}

	//! Write the headers to an output sequence

	template<typename iter_type>
	iter_type toString(//! Output iterator
			   iter_type iter,

			   //! The line terminator is overriden in http::cgiimpl

			   const char *newline,

			   //! Set to true to omit the trailing blank line
			   bool noseparator=false) const
	{
		for (const std::string &header:this->list())
		{
			iter=std::copy(header.begin(), header.end(), iter);

			for (const char *p=newline; *p; p++)
				*iter++ = *p;
		}
		if (!noseparator)
			for (const char *p=newline; *p; p++)
				*iter++ = *p;
		return iter;
	}

	//! Throw an exception, invalid character in token.

	static void invalid_char_in_token() __attribute__((noreturn));

	//! Emit a quoted word into an output iterator

	template<typename output_iter, typename input_iter>
	static output_iter emit_quoted_string(//! Output iterator
					      output_iter out,

					      //! Beginning iterator
					      input_iter b,

					      //! Ending iterator
					      input_iter e)
	{
		*out++='"';

		while (b != e)
		{
			if (*b == '\r' || *b == '\n' || *b == 0)
				headersbase::invalid_char_in_token();
			if (*b == '\\' || *b == '"')
				*out++='\\';
			*out++=*b++;
		}
		*out++='"';
		return out;
	}

	//! Return a string with a quoted word.

	template<typename input_iter>
	static std::basic_string<typename
				 std::iterator_traits<input_iter>::value_type>
	quoted_string(//! Beginning iterator
		      input_iter b,

		      //! Ending iterator
		      input_iter e)
	{
		typedef typename std::iterator_traits<input_iter>::value_type
			char_type;
		typedef std::basic_string<char_type> string_type;

		string_type s;

		emit_quoted_string(std::back_insert_iterator<string_type>(s),
				   b, e);

		return s;
	}

	//! Return a string with a quoted word.

	template<typename container_type>
	static auto quoted_string(//! Container
				  const container_type &container)

#ifndef DOXYGEN
	// Confusion
		-> decltype(quoted_string(container.begin(), container.end()))
#endif
	{
		return quoted_string(container.begin(), container.end());
	}
};


#ifndef DOXYGEN

extern template
std::ostreambuf_iterator<char>
headersbase::toString<std::ostreambuf_iterator<char>
		      >(std::ostreambuf_iterator<char>, const char *, bool)
	const;

extern template std::string headersbase::quoted_string(const std::string &);

#endif


#if 0
{
#endif
}
#endif
