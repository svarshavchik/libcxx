/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_csv_H
#define x_csv_H

#include <x/namespace.h>
#include <x/exception.H>

#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
#include <iostream>
#include <type_traits>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Output one CSV-formatted column

//! \internal

template<typename char_type, typename traits_type,
	 typename alloc_type, typename output_type>
output_type tocsv_value(output_type out_iter,
			const std::basic_string<char_type, traits_type,
			alloc_type> &value)
{
	typename std::basic_string<char_type, traits_type, alloc_type>
		::const_iterator b, e;

	for (b=value.begin(), e=value.end(); b != e; ++b)
		if (*b == '"' || *b == ',' ||
		    (*b >= 0 && *b < ' '))
			break;

	if (b == e)
		return std::copy(value.begin(), value.end(), out_iter);

	*out_iter++='"';

	for (b=value.begin(), e=value.end(); b != e; ++b)
	{
		if (*b == '"')
			*out_iter++='"';

		*out_iter++=*b;
	}
	*out_iter++='"';
	return out_iter;
}

//! Output one CSV-formatted column

//! \internal

template<typename char_type, typename output_type>
output_type tocsv_value(output_type out_iter,
			const char_type *value)
{
	return tocsv_value(out_iter, std::basic_string<char_type>(value));
}

//! Convert a container of strings to a CSV-formatted line

//! This function defines "CSV" as follows:
//!
//! - the string values are separated by a single comma character.
//! - values that do not contain commas, quote characters, or control characters
//! appear as is.
//! - all other string values begin with a quote character and end with a single
//! quote character. quote values in the string are doubled.
//!
//! \note
//! Strings may contain newline characters. This means that these string values
//! get quoted, and this CSV-formatted line may, therefore, contain newlines!
//!
//! \return The output iterator, after the line is formatted.
//!
//! \note
//! This function does not output the newline character sequence to the output
//! iterator.
//! The caller is responsible for outputing the newline character sequence to
//! the output iterator.
//!
//! This function is defined for both narrow and wide character string iterators
//! however for narrow character string iterators, only "backwards-compatible
//! supersets of US-ASCII" will work. UTF-8 is fine.

template<typename input_iter,
	 typename output_iter>
output_iter tocsv(//! String container's beginning input iterator
		  input_iter beg_iter,

		  //! String container's ending input iterator
		  input_iter end_iter,

		  //! Output iterator
		  output_iter out_iter)
{
	const char *sep="";

	while (beg_iter != end_iter)
	{
		while (*sep)
			*out_iter++=*sep++;

		out_iter=tocsv_value(out_iter, *beg_iter++);

		sep=",";
	}
	return out_iter;
}

//! Parse a CSV-formatted line into a container

//! This function defines "CSV" as follows:
//!
//! - the string values are separated by a single comma character.
//! - if a string value does not begin with a quote character, the string value
//! is terminated by the next comma character or the next control character.
//! - if a string value begins with a quote character, its value ends with
//! another single quote character. Two consecutive quote characters are
//! parsed as a single quote character that's parsed as a string.
//!
//! This function consumes the passed input iterator until one of the following
//! conditions is true:
//!
//! - this function terminates immediately without parsing any string values
//! if the input iterator passed to this function points to a control
//! character.
//! - when a string value does not begin with a quote character, when the input
//! iterator is equal to the ending iterator or when the input iterator points
//! to a control character.
//! - when a string value begins with a quote character, if the input iterator
//! becomes equal to the ending iterator while looking for the ending quote
//! character.
//! - when a string value begins with a quote character, if after the
//! trailing quote character the input iterator compares equal to the ending
//! input iterator, or if the value pointed to by the input iterator is not a
//! comma.
//!
//! \return the input iterator after the CSV-formatted line is parsed.
//! If the input iterator is equal to the ending input iterator, or if it does
//! not point to the newline sequence, the caller should consider the
//! CSV-formatted input as malformed.

template<typename input_iterator,
	 typename container_type>
input_iterator fromcsv(//! The beginning \c char input iterator
		       input_iterator beg_iter,

		       //! The ending \c char input iterator
		       input_iterator end_iter,

		       //! The container for the string values.

		       //! The parsed string values are inserted into the
		       //! container by invoking its push_back() method.
		       container_type &container)
{
	typedef typename std::decay<decltype(*beg_iter)>::type char_type;

	bool skip_comma=false;

	while (beg_iter != end_iter)
	{
		std::basic_string<char_type> s;

		if (skip_comma)
			++beg_iter;
		else
		{
			if (*beg_iter >= 0 && *beg_iter < ' ')
				break;
		}

		skip_comma=true;

		if (beg_iter == end_iter || *beg_iter != '"')
		{
			while (beg_iter != end_iter &&
			       *beg_iter != ',' && *beg_iter >= ' ')
			{
				s.push_back(*beg_iter++);
			}
		}
		else
		{
			++beg_iter; // Skip the quote

			while (beg_iter != end_iter)
			{
				if (*beg_iter == '"')
				{
					++beg_iter;
					if (beg_iter == end_iter ||
					    *beg_iter != '"')
						break;
				}

				s.push_back(*beg_iter++);
			}
		}

		container.push_back(s);

		if (beg_iter != end_iter && *beg_iter == ',')
			continue;
		break;
	}

	return beg_iter;
}

//! Helper object for parsing CSV header rows

//! \internal
//!
template<typename char_type>
class csvheaderrow_iter {

	//! Column names to number map
	std::map<std::basic_string<char_type>, size_t> &header;

	//! Next column number
	size_t colnum;

public:
	//! Constructor
	csvheaderrow_iter(std::map<std::basic_string<char_type>,
				   size_t> &headerArg)
		noexcept : header(headerArg), colnum(0) {}

	//! Destructor
	~csvheaderrow_iter() {}

	//! Register a CSV column
	void push_back(const std::basic_string<char_type> &s) noexcept
	{
		header[s]=colnum++;
	}
};

//! Parse a CSV header row into a map

//! The map is keyed by column name, and gives the column number.
//!
//! \return the input iterator after the CSV-formatted header row is parsed.
//! If the input iterator is equal to the ending input iterator, or if it does
//! not point to the newline sequence, the caller should consider the
//! CSV-formatted input as malformed.

template<typename input_iterator, typename char_type>
input_iterator csvheaderrow(//! The beginning \c char input iterator
			    input_iterator beg_iter,

			    //! The ending \c char input iterator
			    input_iterator end_iter,

			    //! The CSV header

			    std::map<std::basic_string<char_type>,
				     size_t> &header)

{
	csvheaderrow_iter<char_type> iter(header);

	return fromcsv(beg_iter, end_iter, iter);
}

#ifndef DOXYGEN
#define LIBCXX_TEMPLATE_DECL extern
#include <x/csv_t.H>
#undef LIBCXX_TEMPLATE_DECL
#endif

#if 0
{
#endif
}
#endif
