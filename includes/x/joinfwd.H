/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_joinfwd_H
#define x_joinfwd_H

#include <x/namespace.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <list>
#include <vector>
#include <set>
#include <type_traits>

namespace LIBCXX_NAMESPACE {

#if 0
}
#endif

//! Copy a separator or a joined element into an output iterator

//! \internal

template<typename sep_type> class join_copy {

public:

	//! This is just a call to std::copy()

	template<typename iter_type>
	static iter_type copy(iter_type out_iter,
			      const sep_type &separator)
	{
		return std::copy(std::begin(separator),
				 std::end(separator),
				 out_iter);
	}
};

//! Copy a separator or a joined element into an output iterator

//! Specialization for separators or elements that are character pointers.
//!
//! \internal

template<typename char_type>
class join_copy<const char_type *> {

public:

	template<typename iter_type>
	static iter_type copy(iter_type out_iter,
			      const char_type *separator)
	{

		//! Copy separator until a null byte.

		while (*separator)
		{
			*out_iter = *separator;
			++out_iter;
			++separator;
		}

		return out_iter;
	}
};

//! Specialization to allow literal strings for separators

template<size_t n>
class join_copy<const char[n]> : public join_copy<const char *> {
};

//! Specialization to allow literal strings for separators

template<size_t n>
class join_copy<char[n]> : public join_copy<const char *> {
};

template<typename char_type>
class join_copy<char_type *>
	: public join_copy<const char *> {
};

//! Helper class that figures out the return value type from join().

//! \internal
template<typename container_value_type>
class non_const_string_ret_type {

public:

	//! The default return value type is the same as what's being joined.

	typedef container_value_type type;
};

//! Specialization

//! When joining const char *s, the return value is a basic_string.
//! \internal

template<typename char_type>
class non_const_string_ret_type<const char_type *> {

public:

	//! Return value from a join() of const characters.

	typedef std::basic_string<char_type> type;
};

//! Helper class that figures out the return value from join()

//! \internal

template<typename T>
class string_ret_type {

public:
	//! Remove constant and volative attribues, then call non_const_string_ret_type

	typedef typename non_const_string_ret_type<
	typename std::remove_cv<T>::type
	>::type type;
};


//! The opposite of strtok(): combine strings together, with a separator

//! Overloaded join() template that takes a collection of strings, and
//! concatenates them together, with a separator string sandwiched in between
//! them.
//!
//! Various join() template definitions provide different ways of specifying
//! how things get joined together.
//!
//! - The collection strings can be specified as an input sequence defined by
//! a beginning iterator and an ending iterator, or as a container.
//!
//! - The separator string may be specified as a string object, or as a literal,
//! native string.
//!
//! - The joined string gets written to an output iterator, in which case
//! join() returns the new value of the output iterator. Or, the joined string
//! gets written into a new string buffer, and the new string gets returned.

template<typename OutputIterator, typename InputIterator,
	 typename sep_type>
OutputIterator join(InputIterator beg_iter,
		    InputIterator end_iter,
		    const sep_type &separator,
		    OutputIterator out_iter);

//! The opposite of strtok(): combine strings together, with a separator

//! \overload
//!

template<typename OutputIterator, typename InputIterator, size_t n>
OutputIterator join(InputIterator beg_iter,
		    InputIterator end_iter,
		    const typename std::decay<decltype(*beg_iter)>::type
		    ::value_type separator[n],
		    OutputIterator out_iter);

//! The opposite of strtok(): combine strings together, with a separator

//! \overload
//!

template<typename OutputIterator, typename InputIterator, size_t n>
OutputIterator join(InputIterator beg_iter,
		    InputIterator end_iter,
		    typename std::decay<decltype(*beg_iter)>::type
		    ::value_type separator[n],
		    OutputIterator out_iter);

//! The opposite of strtok(): combine strings together, with a separator

//! \overload
//!

template<typename Container, typename OutputIterator, typename sep_type>
OutputIterator join(const Container &container,
		    const sep_type &separator,
		    OutputIterator out_iter);

//! The opposite of strtok(): combine strings together, with a separator

//! \overload
//!
template<typename InputIterator, typename sep_type>
auto join(InputIterator beg_iter,
	  InputIterator end_iter,
	  const sep_type &separator)
	-> typename string_ret_type<typename std::decay<decltype(*beg_iter)>
				    ::type>::type;

//! The opposite of strtok(): combine strings together, with a separator

//! \overload
//!

template<typename InputIterator, typename sep_type, size_t n>
auto join(InputIterator beg_iter,
	  InputIterator end_iter,
	  const sep_type (&separator)[n])
	-> typename string_ret_type<typename std::decay<decltype(*beg_iter)>
				    ::type>::type;

//! The opposite of strtok(): combine strings together, with a separator

//! \overload
//!

template<typename InputIterator, typename sep_type, size_t n>
auto join(InputIterator beg_iter,
	  InputIterator end_iter,
	  sep_type (&separator)[n])
	-> typename string_ret_type<typename std::decay<decltype(*beg_iter)>
				    ::type>::type;

//! The opposite of strtok(): combine strings together, with a separator

//! \overload
//!
template<typename Container, typename sep_type>
auto join(const Container &container,
	  const sep_type &separator)
	-> typename string_ret_type<
		typename std::remove_reference<decltype(*std::begin(container))>
		::type >::type;

//! The opposite of strtok(): combine strings together, with a separator

//! \overload
//!
template<typename Container, typename sep_type, size_t n>
auto join(const Container &container,
	  const sep_type (&separator)[n])
	-> typename string_ret_type<
		typename std::remove_reference<decltype(*std::begin(container))>
		::type >::type;

//! The opposite of strtok(): combine strings together, with a separator

//! \overload
//!
template<typename Container, typename sep_type, size_t n>
auto join(const Container &container,
	  sep_type (&separator)[n])
	-> typename string_ret_type<
		typename std::remove_reference<decltype(*std::begin(container))>
		::type >::type;

#ifndef DOXYGEN
#define LIBCXX_TEMPLATE_DECL extern
#include <x/join_t.H>
#undef LIBCXX_TEMPLATE_DECL
#endif

#if 0
{
#endif
}
#endif
