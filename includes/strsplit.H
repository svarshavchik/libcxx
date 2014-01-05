/*
** Copyright 2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_strsplit_H
#define x_strsplit_H

#include <x/namespace.h>

#include <algorithm>
#include <type_traits>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! strsplit() - split a string, with quoting

//! \code
//! std::string str;
//!
//! std::list<std::string> container;
//!
//! strsplit_if(str.begin(), str.end(), container, []
//!            (char c)
//!            {
//!               return c == ' ' || c == '\t' || c == '\r';
//!            }, []
//!            (char c)
//!            {
//!               return c == '"';
//!            });
//!
//! strsplit(str.begin(), str.end(), container, " \t\r", '"');
//! \endcode
//!
//! strsplit_if() extracts zero or more character (or some other) sequences
//! from a sequence defined by a beginning and an ending iterator, and puts
//! each sequence to the container, passed via its third argument, via
//! push_back().
//!
//! The sequences are delimited by values for which the first lambda
//! predicate returns true. Each value in the input sequence is passed to the
//! lambda predicate. The first value for which the predicate returns false
//! forms the start of the first sequence, which ends by either the end of
//! the sequence, or a value for which the predicate returns true.
//!
//! The first lambda predicate gets invoked for each value in the sequence,
//! consecutively. When it returns false, the second lambda predicate gets
//! invoked, which tests for a quoting value. If the quoting predicate returns
//! true, the value does not get included in the current sequence. Rather, the
//! second quoting predicate gets used instead of the first one, for all
//! subsequent values. All values that the quoting predicate returns false get
//! included in the current sequence. If the quoting predicate returns true
//! twice consecutively, the second of the two values get included in the
//! quoting sequence. If the quoting predicate returns true once, and for the
//! next value the quoting predicate returns false (or its the end of the
//! sequence), this indicates the end of quoted text,
//! and strsplit_if() resumes using the first predicate, to search for the
//! end of the sequence.
//!
//! The container that's passed as the third argument must implement push_back()
//! and front().
//!
//! strsplit() is a wrapper for strsplit_if() that constructs the predicate
//! lambdas from raw character strings.

template<typename iter_type,
	 typename container_type,
	 typename separator_lambda,
	 typename quoting_lambda>
void strsplit_if(iter_type beg, iter_type end,
		 container_type &container,
		 separator_lambda &&separator,
		 quoting_lambda &&quoting)
{
	while (beg != end)
	{
		if (separator(*beg))
		{
			++beg;
			continue;
		}

		typename std::remove_reference<decltype(container.front())>::type
			new_value;

		bool tested=true;

		do
		{
			if (!tested && separator(*beg))
			{
				++beg;
				break;
			}
			tested=false;

			if (quoting(*beg))
			{
				while (++beg != end)
				{
					if (!quoting(*beg))
					{
						new_value.push_back(*beg);
						continue;
					}

					if (++beg == end)
						break;

					if (!quoting(*beg))
						break;

					new_value.push_back(*beg);
				}
			}
			else
			{
				new_value.push_back(*beg);
				++beg;
			}
		} while (beg != end);
		container.push_back(new_value);
	}
}

//! Wrapper for strsplit_if() using basic chars

template<typename iter_type,
	 typename container_type,
	 typename char_type>
void strsplit(iter_type beg, iter_type end,
	      container_type &container,
	      const char_type *sep,
	      const char_type quote)
{
	const char_type *end_sep;

	for (end_sep=sep; *end_sep; ++end_sep)
		;
	strsplit_if(beg, end, container, [sep, end_sep]
		    (char_type c)
		    {
			    return std::find(sep, end_sep, c) != end_sep;
		    },
		    [quote]
		    (char_type c)
		    {
			    return c == quote;
		    });
}
#if 0
{
#endif
}
#endif
