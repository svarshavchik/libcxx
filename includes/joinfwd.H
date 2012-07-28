/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_joinfwd_H
#define x_joinfwd_H

#include <x/namespace.h>
#include <x/iter_value.H>

#include <algorithm>
#include <iterator>
#include <string>
#include <list>
#include <vector>
#include <set>

namespace LIBCXX_NAMESPACE {

#if 0
}
#endif

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

template<typename OutputIterator, typename InputIterator>
OutputIterator join(InputIterator beg_iter,
		    InputIterator end_iter,
		    const typename iter_value_type<InputIterator>::value_t &separator,
		    OutputIterator out_iter);

//! The opposite of strtok(): combine strings together, with a separator

//! \overload
//!

template<typename OutputIterator, typename InputIterator>
OutputIterator join(InputIterator beg_iter,
		    InputIterator end_iter,
		    const typename iter_value_type<InputIterator>::value_t
		    ::value_type *separator,
		    OutputIterator out_iter);

//! The opposite of strtok(): combine strings together, with a separator

//! \overload
//!

template<typename OutputIterator, typename InputIterator, size_t n>
OutputIterator join(InputIterator beg_iter,
		    InputIterator end_iter,
		    const typename iter_value_type<InputIterator>::value_t
		    ::value_type separator[n],
		    OutputIterator out_iter);

//! The opposite of strtok(): combine strings together, with a separator

//! \overload
//!

template<typename OutputIterator, typename InputIterator, size_t n>
OutputIterator join(InputIterator beg_iter,
		    InputIterator end_iter,
		    typename iter_value_type<InputIterator>::value_t
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
typename iter_value_type<InputIterator>::value_t join(InputIterator beg_iter,
						      InputIterator end_iter,
						      const sep_type &separator);

//! The opposite of strtok(): combine strings together, with a separator

//! \overload
//!

template<typename InputIterator, size_t n>
typename iter_value_type<InputIterator>::value_t join(InputIterator beg_iter,
						      InputIterator end_iter,
						      const typename iter_value_type<InputIterator>::value_t::value_type (&separator)[n]);

//! The opposite of strtok(): combine strings together, with a separator

//! \overload
//!

template<typename InputIterator, size_t n>
typename iter_value_type<InputIterator>::value_t join(InputIterator beg_iter,
						      InputIterator end_iter,
						      typename iter_value_type<InputIterator>::value_t::value_type (&separator)[n]);

//! The opposite of strtok(): combine strings together, with a separator

//! \overload
//!
template<typename Container, typename sep_type>
typename Container::value_type join(const Container &container,
				    const sep_type &separator);

//! The opposite of strtok(): combine strings together, with a separator

//! \overload
//!
template<typename Container, size_t n>
typename Container::value_type join(const Container &container,
				    const typename Container::value_type::value_type (&separator)[n]);

//! The opposite of strtok(): combine strings together, with a separator

//! \overload
//!
template<typename Container, size_t n>
typename Container::value_type join(const Container &container,
				    typename Container::value_type::value_type (&separator)[n]);

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
