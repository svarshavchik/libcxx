/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_strtok_H
#define x_strtok_H

#include <x/namespace.h>

#include <algorithm>
#include <cstring>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! strtok(), C++-style

//! \code
//!
//! std::string str;
//! class Predicate predicate;
//!
//! std::list<std::string> container;
//!
//! strtok_if(str, predicate, container);
//! \endcode
//!
//! strtok_if() extracts zero or more character sequences from \c str, and
//! appends them to the \c container via the container's push_back() method.
//! The predicate function defines character sequence delimiters.
//! Consecutive character sequences for which the predicate function returns
//! \c false are extracted and added to the container.
//! Once the starting and the ending character sequence position is obtained,
//! the container's \c obj_type is constructed by passing the starting and the
//! ending character sequence to the \c obj_type's constructor, then
//! the constructed object gets added to the container via its push_back()
//! method.
//!
//! For example, if the predicate function evaluates true for all whitespace
//! characters, all whitespace-delimited words get extracted and placed into
//! the container.
//!
//! The predicate function gets evaluated once for each character in the
//! character string, in sequence.
//!
//! Leading and trailing delimiter characters get stripped off. Empty strings
//! do not get added to the container. If the predicate returns \c true for
//! all characters in \c str, nothing gets added to the container.
//!
//! \note
//! Any existing contents of the container are not modified. strtok_if()
//! only adds additional objects to the container

template<typename char_type,
	 typename traits_type,
	 typename alloc_type,
	 typename predicate_type,
	 typename container_type>
void strtok_if( //! The character string
	       const std::basic_string<char_type, traits_type, alloc_type> &str,

	       //! The predicate object
	       const predicate_type &Predicate,

	       //! The container
	       container_type &Container) noexcept
{
	typename std::basic_string<char_type, traits_type, alloc_type>
		::const_iterator b=str.begin(), e=str.end(), p;

	while ((b=std::find_if(b, e, [&Predicate]
			       (char c)
			       {
				       return !Predicate(c);
			       })) != e)
	{
		p=std::find_if(b, e, Predicate);
		Container.push_back(std::basic_string<char_type,
				    traits_type, alloc_type>(b, p));

		b=p;
	}
}

//! Predicate class for strtok_str()

//! \internal
//! This is a predicate class that implements the search function for
//! strtok_str()

template<typename char_type>
class strtok_predicate : public std::unary_function<char_type, bool> {

public:
	//! The string to search.

	const char_type *toks;

	//! The search operator

	bool operator()(const char_type &v) const noexcept
	{
		const char_type *c=toks;

		while (*c)
		{
			if (*c++ == v)
				return true;
		}
		return false;
	}
};

//! strtok(), C++-style

//! \code
//!
//! std::string str;
//!
//! std::list<std::string> container;
//!
//! strtok_str(str, " \t\r\n", container);
//! \endcode
//!
//! This is a version of strtok_if() that replaces a predicate object with
//! a literal character string. strtok_str() constructs a predicate object
//! that returns \c true for all characters specified in the character string,
//! and invokes strtok_if().

template<typename char_type,
	 typename traits_type,
	 typename alloc_type,
	 typename container_type>
void strtok_str( const std::basic_string<char_type, traits_type, alloc_type> &str,
		 const char_type *toks,
		 container_type &Container) noexcept
{
	strtok_predicate<char_type> s;

	s.toks=toks;

	strtok_if(str, s, Container);
}

//! strtok(), C++-style

//! \overload
//! The output of this strtok_str() is always the same, since both the
//! original string and the delimiter string are constant string literals.
//! This function is a convenient shortcut for initializing a container
//! with many predefined strings.

template<typename char_type,
	 typename container_type>
void strtok_str( const char_type *str,
		 const char_type *toks,
		 container_type &Container) noexcept
{
	strtok_str(std::basic_string<char_type>(str), toks, Container);
}

#if 0
{
#endif
}
#endif
