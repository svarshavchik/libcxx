/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_join_H
#define x_join_H

#include <x/namespace.h>
#include <x/joinfwd.H>

namespace LIBCXX_NAMESPACE {

#if 0
}
#endif

template<typename OutputIterator, typename InputIterator>
OutputIterator join(InputIterator beg_iter,
		    InputIterator end_iter,
		    const typename iter_value_type<InputIterator>::value_t &separator,
		    OutputIterator out_iter)
{
	bool need_separator=false;

	while (beg_iter != end_iter)
	{
		if (need_separator)
			out_iter=std::copy(separator.begin(), separator.end(),
					   out_iter);
		need_separator=true;

		out_iter=std::copy(beg_iter->begin(),
				   beg_iter->end(), out_iter);
		++beg_iter;
	}
	return out_iter;
}

template<typename OutputIterator, typename InputIterator>
OutputIterator join(InputIterator beg_iter,
		    InputIterator end_iter,
		    const typename iter_value_type<InputIterator>::value_t
		    ::value_type *separator,
		    OutputIterator out_iter)
{
	return join(beg_iter, end_iter,
		    typename iter_value_type<InputIterator>::value_t(separator),
		    out_iter);
}

template<typename OutputIterator, typename InputIterator, size_t n>
OutputIterator join(InputIterator beg_iter,
		    InputIterator end_iter,
		    const typename iter_value_type<InputIterator>::value_t
		    ::value_type separator[n],
		    OutputIterator out_iter)
{
#ifdef LIBCXX_INTERNAL_DEBUG
	LIBCXX_INTERNAL_DEBUG;
#endif

	return join(beg_iter, end_iter,
		    typename iter_value_type<InputIterator>::value_t(separator),
		    out_iter);
}

template<typename OutputIterator, typename InputIterator, size_t n>
OutputIterator join(InputIterator beg_iter,
		    InputIterator end_iter,
		    typename iter_value_type<InputIterator>::value_t
		    ::value_type separator[n],
		    OutputIterator out_iter)
{
#ifdef LIBCXX_INTERNAL_DEBUG
	LIBCXX_INTERNAL_DEBUG;
#endif

	return join(beg_iter, end_iter,
		    typename iter_value_type<InputIterator>::value_t(separator),
		    out_iter);
}

template<typename Container, typename OutputIterator, typename sep_type>
OutputIterator join(const Container &container,
		    const sep_type &separator,
		    OutputIterator out_iter)
{
	return join(container.begin(), container.end(), separator, out_iter);
}

template<typename InputIterator, typename sep_type>
typename iter_value_type<InputIterator>::value_t join(InputIterator beg_iter,
						      InputIterator end_iter,
						      const sep_type &separator)
{
	typedef typename iter_value_type<InputIterator>::value_t string_t;

	string_t string;

	join(beg_iter, end_iter, separator,
	     std::back_insert_iterator<string_t>(string));

	return string;
}

template<typename InputIterator, size_t n>
typename iter_value_type<InputIterator>::value_t join(InputIterator beg_iter,
						      InputIterator end_iter,
						      const typename iter_value_type<InputIterator>::value_t::value_type (&separator)[n])
{
#ifdef LIBCXX_INTERNAL_DEBUG
	LIBCXX_INTERNAL_DEBUG;
#endif

	return join(beg_iter, end_iter,
		    static_cast <const typename iter_value_type<InputIterator>::value_t::value_type *>(separator));
}

template<typename InputIterator, size_t n>
typename iter_value_type<InputIterator>::value_t join(InputIterator beg_iter,
						      InputIterator end_iter,
						      typename iter_value_type<InputIterator>::value_t::value_type (&separator)[n])
{
#ifdef LIBCXX_INTERNAL_DEBUG
	LIBCXX_INTERNAL_DEBUG;
#endif

	return join(beg_iter, end_iter,
		    static_cast <const typename iter_value_type<InputIterator>::value_t::value_type *>(separator));
}



template<typename Container, typename sep_type>
typename Container::value_type join(const Container &container,
				    const sep_type &separator)
{
	typedef typename Container::value_type string_t;

	string_t string;

	join(container.begin(), container.end(), separator,
	     std::back_insert_iterator<string_t>(string));

	return string;
}

template<typename Container, size_t n>
typename Container::value_type join(const Container &container,
				    const typename Container::value_type::value_type (&separator)[n])
{
#ifdef LIBCXX_INTERNAL_DEBUG
	LIBCXX_INTERNAL_DEBUG;
#endif

	return join(container, static_cast<const typename Container::value_type::value_type *>(separator));
}

template<typename Container, size_t n>
typename Container::value_type join(const Container &container,
				    typename Container::value_type::value_type (&separator)[n])
{
#ifdef LIBCXX_INTERNAL_DEBUG
	LIBCXX_INTERNAL_DEBUG;
#endif

	return join(container, static_cast<const typename Container::value_type::value_type *>(separator));
}

#if 0
{
#endif
}
#endif
