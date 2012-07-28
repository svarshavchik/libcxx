/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_str2char_H
#define x_str2char_H

#include <x/namespace.h>
#include <x/exception.H>
#include <iterator>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Convert a std::string input iterator to a char input iterator

//! This template takes an input iterator over std::string-s as a template
//! parameter, and converts it to an input iterator over chars, which iterates
//! over the individual characters in the strings, completely skipping any
//! empty strings.

template<typename string_iter_t>
class str2char_input_iter : public std::iterator<std::input_iterator_tag,
						 typename std::iterator_traits<typename std::iterator_traits<string_iter_t>::value_type::const_iterator>::value_type> {

	//! Current iterator

	string_iter_t cur_iter;

	//! Ending iterator

	string_iter_t end_iter;

	//! Iterator over the underlying characters
	typedef typename std::iterator_traits<string_iter_t>::value_type
	char_container;

	//! Character iterator for the current string

	typename char_container::const_iterator ch_iter;

	//! Ending character iterator for the current string

	typename char_container::const_iterator end_ch_iter;

public:

	//! The underlying character type

	typedef typename std::iterator_traits
	<typename char_container::const_iterator>
	::value_type char_t;

private:

	//! Temporary placeholder for post-increment operator

	char_t savedch;

public:
	//! The constructor takes the beginning iterator and an ending iterator

	str2char_input_iter(const string_iter_t &cur_iterArg,
			    const string_iter_t &end_iterArg)
 : cur_iter(cur_iterArg),
				   end_iter(end_iterArg)
	{
		init_string_iter();
	}

	//! A constructor with a single parameter passes the ending iterator

	str2char_input_iter(const string_iter_t &end_iterArg)
 : cur_iter(end_iterArg),
				   end_iter(end_iterArg)
	{
	}

	//! Or a default constructor, if possible

	str2char_input_iter()
	{
	}

	//! Destructor

	~str2char_input_iter() noexcept
	{
	}

	//! The \c * operator

	char_t operator*() const noexcept
	{
		return *ch_iter;
	}

	//! The preincrement operator

	str2char_input_iter<string_iter_t> &operator++()
	{
		if (++ch_iter == end_ch_iter)
		{
			++cur_iter;
			init_string_iter();
		}
		return *this;
	}

	//! The postincrement operator

	const char *operator++(int)
	{
		savedch=*ch_iter;
		operator++();
		return &savedch;
	}

	//! Compare for equality

	//! \note
	//! Two ending iterators are considered equal.
	//! Any other situation does not.

	bool operator==(const str2char_input_iter<string_iter_t> &o)
		const noexcept
	{
		return cur_iter == end_iter &&
			o.cur_iter == o.end_iter;
	}

	//! Compare for inequality

	//! \note
	//! Two ending iterators are considered equal.
	//! Any other situation does not.

	bool operator!=(const str2char_input_iter<string_iter_t> &o)
		const noexcept
	{
		return !operator==(o);
	}

private:

	//! Set up the beginning character iterator for a new string

	void init_string_iter()
	{
		while (cur_iter != end_iter)
		{
			ch_iter=cur_iter->begin();
			end_ch_iter=cur_iter->end();

			if (ch_iter != end_ch_iter)
				return;

			++cur_iter;
		}
	}
};

#if 0
{
#endif
}
#endif
