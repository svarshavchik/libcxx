/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_iter_value_H
#define x_iter_value_H

#include <x/namespace.h>
#include <iterator>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Given an iterator, figure out what it's iterating over

//! This is usually <tt>std::iterator_traits&lt;iter_type&gt;::%value_type</tt>.
//! However, this doesn't work for insert and stream iterators, their value
//! is specified as a void, so more work is needed.
//!
//! <tt>iter_value_type&lt;iter_type&gt;::%value_t</tt> gives what
//! \c iter_type iterates over.

template<typename iter_type>
class iter_value_type {

public:

	//! The value type that's being iterated over

	typedef typename std::iterator_traits<iter_type>::value_type value_t;
};

//! \c std::back_insert_iterator specialization

template<typename container_type>
class iter_value_type<std::back_insert_iterator<container_type> > {

public:

	//! The value type that's being iterated over

	typedef typename container_type::value_type value_t;
};

//! \c std::front_insert_iterator specialization

template<typename container_type>
class iter_value_type<std::front_insert_iterator<container_type> > {

public:

	//! The value type that's being iterated over

	typedef typename container_type::value_type value_t;
};

//! \c std::insert_iterator specialization

template<typename container_type>
class iter_value_type<std::insert_iterator<container_type> > {

public:

	//! The value type that's being iterated over

	typedef typename container_type::value_type value_t;
};

//! \c std::istream_iterator specialization

template <typename t_type, typename char_type, typename traits_type,
	  typename ptrdiff_type>
class iter_value_type<std::istream_iterator<t_type, char_type, traits_type,
					    ptrdiff_type> > {

public:

	//! The value type that's being iterated over

	typedef char_type value_t;
};

//! \c std::ostream_iterator specialization

template <typename t_type, typename char_type, typename traits_type>
class iter_value_type<std::ostream_iterator<t_type, char_type, traits_type> > {

public:

	//! The value type that's being iterated over

	typedef char_type value_t;
};

//! \c std::istreambuf_iterator specialization

template <typename char_type, typename traits_type>
class iter_value_type<std::istreambuf_iterator<char_type, traits_type> > {

public:

	//! The value type that's being iterated over

	typedef char_type value_t;
};

//! \c std::ostreambuf_iterator specialization

template <typename char_type, typename traits_type>
class iter_value_type<std::ostreambuf_iterator<char_type, traits_type> > {

public:

	//! The value type that's being iterated over

	typedef char_type value_t;
};

#if 0
{
#endif
}
#endif
