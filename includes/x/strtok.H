/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_strtok_H
#define x_strtok_H

#include <x/namespace.h>

#include <algorithm>
#include <cstring>
#include <string_view>
#include <string>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! strtok(), C++-style

//! \code
//!
//! std::string str;
//!
//! std::list<std::string> container;
//!
//! strtok_if(str, [](char c) { return c == ' ';}, container);
//! \endcode
//!
//! strtok_if() extracts zero or more character sequences from \c str, and
//! appends them to the \c container via the container's push_back() method.
//! The second parameter is a predicate function that
//! defines character sequence delimiters.
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

template<typename string_type,
	 typename predicate_type,
	 typename container_type>
void strtok_if( //! The character string
	       const string_type &str,

	       //! The predicate object
	       const predicate_type &Predicate,

	       //! The container
	       container_type &Container) noexcept
{
	auto b=std::begin(str),
		e=std::end(str);

	while ((b=std::find_if(b, e, [&Predicate]
			       (auto c)
			       {
				       return !Predicate(c);
			       })) != e)
	{
		auto p=std::find_if(b, e, Predicate);
		Container.push_back(typename container_type::value_type(b, p));

		b=p;
	}
}

//! strtok(), C++-style

//! \overload
//!
//! This overloaded strtok() takes an extra parameter that defines a
//! predicate that returns true for a quoting character, that allows
//! parsed works to contain whitespace predicate characters.
//!
//! Two consecutive quote characters insert a single quote character
//! into the parsed token.

template<typename string_type,
	 typename predicate_type,
	 typename qpredicate_type,
	 typename container_type>
void strtok_if( //! The character string
	       const string_type &str,

	       //! The predicate object
	       const predicate_type &Predicate,

	       //! The quote predicate object
	       const qpredicate_type &qPredicate,

	       //! The container
	       container_type &Container) noexcept
{
	auto b=std::begin(str),
		e=std::end(str);

	while ((b=std::find_if(b, e, [&Predicate]
			       (auto c)
			       {
				       return !Predicate(c);
			       })) != e)
	{
		if (qPredicate(*b))
		{
			++b;
			auto p=std::find_if(b, e, qPredicate);

			string_type token(b, p);

			while (p != e)
			{
				++p;
				if (p != e && qPredicate(*p))
				{
					b=p;
					++p;
					p=std::find_if(p, e, qPredicate);
					token.append(b, p);
				}
				else
					break;
			}
			Container.push_back(token);
			b=p;
			continue;
		}
		auto p=std::find_if(b, e, Predicate);
		Container.push_back(typename container_type::value_type(b, p));

		b=p;
	}
}

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

template<typename string_type,
	 typename container_type>
void strtok_str( const string_type &str,
		 const typename string_type::value_type *toks,
		 container_type &Container) noexcept
{
	strtok_if(str,
		  [toks]
		  (auto v)
		  {
			  auto c=toks;

			  while (*c)
			  {
				  if (*c++ == v)
					  return true;
			  }
			  return false;
		  }, Container);
}

//! strtok(), C++-style

//! \overload

template<typename char_type,
	 typename container_type>
void strtok_str( const char_type *str,
		 const char_type *toks,
		 container_type &Container) noexcept
{
	strtok_str(std::basic_string_view<char_type>(str), toks,
		   Container);
}

//! strtok(), C++ style

//! \overload
//!
//! \code
//!
//! std::string str;
//!
//! std::list<std::string> container;
//!
//! strtok_str(str, " \t\r\n", '"', container);
//! \endcode
//!
//! This overloaded version of strtok_str() takes an extra parameter that
//! specifies the quoting character.

template<typename string_type, typename container_type>
void strtok_str( const string_type &str,
		 const typename string_type::value_type *toks,
		 typename string_type::value_type quoting_character,
		 container_type &Container) noexcept
{
	strtok_if(str,
		  [toks]
		  (auto v)
		  {
			  auto c=toks;

			  while (*c)
			  {
				  if (*c++ == v)
					  return true;
			  }
			  return false;
		  },
		  [quoting_character]
		  (auto v)
		  {
			  return v == quoting_character;
		  },
		  Container);
}

//! strtok(), C++-style

//! \overload

template<typename char_type,
	 typename container_type>
void strtok_str( const char_type *str,
		 const char_type *toks,
		 char_type quoting_character,
		 container_type &Container) noexcept
{
	strtok_str(std::basic_string_view<char_type>(str), toks,
		   quoting_character, Container);
}

//! Trim a character sequence, determined by a beginning and an ending iterator.

//! Updates the iterator parameters to exclude leading and trailing whitespace.

template<typename iter_type>
void trim(iter_type &b, iter_type &e)
{
	while (b != e && (*b == ' ' || *b == '\t' || *b == '\r' || *b == '\n'))
		++b;
	while (b != e && (e[-1] == ' ' || e[-1] == '\t' || e[-1] == '\r'
			  || e[-1] == '\n'))
		--e;
}

//! Remove whitespace from string.

template<typename string_type>
string_type trim(const string_type &str)
{
	auto b=str.begin(), e=str.end();

	trim(b, e);

	return {b, e};
}

#ifndef DOXYGEN
extern template void trim(const char *&, const char *&);
extern template void trim(const char32_t *&, const char32_t *&);
extern template void trim(std::string::iterator &, std::string::iterator &);
extern template void trim(std::string::const_iterator &,
			  std::string::const_iterator &);
extern template void trim(std::u32string::iterator &, std::u32string::iterator &);
extern template void trim(std::u32string::const_iterator &,
			  std::u32string::const_iterator &);

extern template std::string trim(const std::string &);
extern template std::u32string trim(const std::u32string &);
#endif

#if 0
{
#endif
}
#endif
