/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_chrcasecmp_H
#define x_chrcasecmp_H

#include <algorithm>
#include <functional>
#include <string>

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	//! US-ASCII case-insensitive comparison

	//! This namespace contains functions and algorithms for
	//! case-insensitive comparison in the US-ASCII character set.

	namespace chrcasecmp {
#if 0
	};
};
#endif

//! Convert a single character to uppercase

inline static char toupper(char a) noexcept
{
	if (a >= 'a' && a <= 'z')
		a -= 'a' - 'A';
	
	return a;
}

//! Convert a single character to lowercase

inline static char tolower(char a) noexcept
{
	if (a >= 'A' && a <= 'Z')
		a -= 'A' - 'a';
	
	return a;
}

//! Template function for generating a comparison object for a single char.

template<typename cmp> class cmp_char
	: public std::binary_function<char, char, bool> {

public:
	cmp_char() noexcept {}
	~cmp_char() noexcept {}

	//! Comparison function

	bool operator()(char a, char b) const noexcept
	{
		return cmp()(toupper(a), toupper(b));
	}
};

//! Single character comparison functor

typedef cmp_char< std::equal_to<char> > char_equal_to;

//! Single character comparison functor

typedef cmp_char< std::not_equal_to<char> > char_not_equal_to;

//! Single character comparison functor

typedef cmp_char< std::less<char> > char_less;

//! Single character comparison functor

typedef cmp_char< std::greater_equal<char> > char_greater_equal;

//! Single character comparison functor

typedef cmp_char< std::greater<char> > char_greater;

//! Single character comparison functor

typedef cmp_char< std::less_equal<char> > char_less_equal;

//! Case-insensitive lexicographical comparison

//! \return 0 if the two strings contain the same case-insensitive text.
//! A negative or a positive value that specifies whether the first string
//! sorts before or after the second string.

int compare(const std::string &a, const std::string &b) noexcept;

//! Template function for generating a comparison object for a std::string

template<typename cmp> class cmp_str
	: public std::binary_function<std::string, std::string, bool> {

public:
	cmp_str() noexcept {}
	~cmp_str() noexcept {}

	//! Comparison function
	bool operator()(const std::string &a, const std::string &b) const
	{
		return cmp()(compare(a, b), 0);
	}
};

//! String comparison functor

typedef cmp_str< std::equal_to<int> > str_equal_to;

//! String comparison functor

typedef cmp_str< std::not_equal_to<int> > str_not_equal_to;

//! String comparison functor

typedef cmp_str< std::less<int> > str_less;

//! String comparison functor

typedef cmp_str< std::greater_equal<int> > str_greater_equal;

//! String comparison functor

typedef cmp_str< std::greater<int> > str_greater;

//! String comparison functor

typedef cmp_str< std::less_equal<int> > str_less_equal;


#if 0
{
	{
#endif
	}
}
#endif
