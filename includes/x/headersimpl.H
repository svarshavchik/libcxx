/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_headersimpl_H
#define x_headersimpl_H

#include <cstring>
#include <x/headersbase.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Headers that are terminated by an LF or a CRLF sequence

//! Instantiate this template using either headersbase::lf_endl or
//! headersbase::crlf_endl template class to produce a class that parses
//! headers using the LF or the CRLF end of line sequence.

template<typename endl_type>
class headersimpl : public headersbase {

public:

	//! Constructor
	headersimpl() noexcept;

	//! Constructor
	headersimpl(const headersbase &headers);

	//! Destructor
	~headersimpl();

	//! Clear all headers

	void clear()
	{
		headerlist.clear();
		headermap.clear();
	}

	//! Parse the headers from a sequence

	//! The headers get parsed from the sequence that's specified by a
	//! beginning and an ending iterator.
	//!
	//! The headers are terminated by a single line.
	template<typename iter_type>
	iter_type parse(//! Beginning iterator
			iter_type beg_iter,

			//! Ending iterator
			iter_type end_iter,

			//! Maximum number of lines to parse, 0 means unlimited.

			//! An %exception gets thrown when the number of parsed
			//! lines exceeds this.

			size_t maxlimit)
	{
		headerlist.clear();
		headermap.clear();

		std::string line;

		while (1)
		{
			std::pair<iter_type, bool>
				ret(endl_type::line(beg_iter, end_iter, line));

			beg_iter=ret.first;
			if (!ret.second)
				parse_endofstream();

			if (line.size() == 0)
				break;

			if (maxlimit && --maxlimit == 0)
				parse_toomanyheaders();

			switch (headerlist.empty() ? 0:*line.begin()) {
			case ' ':
			case '\t':
				fold_header(line);
				break;
			default:
				new_header(line);
				break;
			}
		}
		return beg_iter;
	}

	//! Write the headers to an output sequence
	template<typename iter_type>
	iter_type to_string(//! Output iterator
			   iter_type iter,

			   //! Set to true to omit the trailing blank line
			   bool noseparator=false)
		const
	{
		return headersbase::to_string(iter, endl_type::eol_str,
					     noseparator);
	}

	//! Manually append a line of text to the parsed headers.

	//! If the line begins with a whitespace character, it gets appended
	//! to the most recently added header. Otherwise it must be formatted
	//! as "name: value".
	//!
	//! \return an iterator to the newly-created header. An ending iterator
	//! gets returned if this line is empty or it
	//! began with a whitespace character and
	//! was appended to some previous header.
	//!
	//! \note
	//! This is an iterator to a map of header names to values, as such
	//! the returned iterator will not necessarily be the next to the
	//! ending iterator.

	iterator append(//! A line of text

			//! An %exception gets thrown if this string contains
			//! the end of line character sequence.
			const std::string &header);


	//! Manually append a line of text to the parsed headers.

	//! \overload
	iterator append(//! Header's name.
			const std::string &name,

			//! Header's value.

			//! An %exception gets thrown if this string contains
			//! the end of line character sequence.
			const std::string &value);


	//! Replace an existing header.

	//! All existing header or headers with this name are removed.
	//! "name: value" is added to the headers.
	//!
	//! \return an iterator to the replacement header.

	iterator replace(//! Header's name.
			 const std::string &name,

			 //! Header's value.

			 //! An %exception gets thrown if this string contains
			 //! the end of line character sequence.
			 const std::string &value);
private:
	//! A helper object for implementing headers[name]=value

	struct eqhelper {

		//! Saved header name
		const std::string &headername;

		//! The original object
		headersimpl<endl_type> &obj;

		//! Constructor

		eqhelper(const std::string &headernameArg,
			 headersimpl<endl_type> &objArg)
			: headername(headernameArg), obj(objArg)
		{
		}
			 
		//! The assignment operation invokes replace().

		iterator operator=(const std::string &value)
		{
			return obj.replace(headername, value);
		}
	};

public:

	//! Syntactic sugar: allow headers[name]=value syntax

	eqhelper operator[](const std::string &headername)
	{
		return eqhelper(headername, *this);
	}

private:
	//! Fold a header
	void fold_header(const std::string &line);
public:
	//! Convenience function for parsing headers from an input stream

	template<typename char_type, typename traits_type>
	std::basic_istream<char_type, traits_type> &
	parse(//! Input stream
	      std::basic_istream<char_type, traits_type> &i,

	      //! Maximum number of headers to parse, 0 means unlimited
	      size_t maxlimit)

	{
		parse(std::istreambuf_iterator<char_type, traits_type>(i),
		      std::istreambuf_iterator<char_type, traits_type>(),
		      maxlimit);
		return i;
	}
};

#ifndef DOXYGEN

extern template class headersimpl<headersbase::crlf_endl>;
extern template class headersimpl<headersbase::lf_endl>;

extern template
std::istreambuf_iterator<char>
headersimpl<headersbase::crlf_endl>::parse<std::istreambuf_iterator<char>
					   >(std::istreambuf_iterator<char>,
					     std::istreambuf_iterator<char>,
					     size_t);

extern template
std::istream &
headersimpl<headersbase::crlf_endl>::parse(std::istream &, size_t);

extern template
std::ostreambuf_iterator<char>
headersimpl<headersbase::crlf_endl>::to_string<std::ostreambuf_iterator<char>
					      >(std::ostreambuf_iterator<char>,
						bool)
	const;

extern template
std::istreambuf_iterator<char>
headersimpl<headersbase::lf_endl>::parse<std::istreambuf_iterator<char>
					   >(std::istreambuf_iterator<char>,
					     std::istreambuf_iterator<char>,
					     size_t);

extern template
std::istream &
headersimpl<headersbase::lf_endl>::parse(std::istream &, size_t);

extern template
std::ostreambuf_iterator<char>
headersimpl<headersbase::lf_endl>::to_string<std::ostreambuf_iterator<char>
					    >(std::ostreambuf_iterator<char>,
					      bool)
	const;

#endif

#if 0
{
#endif
}
#endif
