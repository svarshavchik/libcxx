/*
** Copyright 2012-2019 Double Precision, Inc.
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

template<typename OutputIterator, typename InputIterator,
	 typename sep_type>
OutputIterator join(InputIterator beg_iter,
		    InputIterator end_iter,
		    const sep_type &separator,
		    OutputIterator out_iter)
{
	bool need_separator=false;

	while (beg_iter != end_iter)
	{
		if (need_separator)
			out_iter=join_copy<sep_type>::copy(out_iter,
							   separator);
		need_separator=true;

		out_iter=join_copy<
			typename std::remove_cv<
				typename std::remove_reference<
					decltype(*beg_iter)>
				::type>::type>::copy(out_iter, *beg_iter);
		++beg_iter;
	}
	return out_iter;
}

template<typename OutputIterator, typename InputIterator, size_t n>
OutputIterator join(InputIterator beg_iter,
		    InputIterator end_iter,
		    const typename std::decay<decltype(*beg_iter)>::type
		    ::value_type separator[n],
		    OutputIterator out_iter)
{
#ifdef LIBCXX_INTERNAL_DEBUG
	LIBCXX_INTERNAL_DEBUG;
#endif

	return join(beg_iter, end_iter,
		    &separator[0],
		    out_iter);
}

template<typename OutputIterator, typename InputIterator, size_t n>
OutputIterator join(InputIterator beg_iter,
		    InputIterator end_iter,
		    typename std::decay<decltype(*beg_iter)>::type
		    ::value_type separator[n],
		    OutputIterator out_iter)
{
#ifdef LIBCXX_INTERNAL_DEBUG
	LIBCXX_INTERNAL_DEBUG;
#endif

	return join(beg_iter, end_iter,
		    &separator[0],
		    out_iter);
}

template<typename Container, typename OutputIterator, typename sep_type>
OutputIterator join(const Container &container,
		    const sep_type &separator,
		    OutputIterator out_iter)
{
	return join(std::begin(container),
		    std::end(container), separator, out_iter);
}

template<typename InputIterator, typename sep_type>
auto join(InputIterator beg_iter,
	  InputIterator end_iter,
	  const sep_type &separator)
	-> typename string_ret_type<typename std::decay<decltype(*beg_iter)>
				    ::type>::type
{
	typedef typename string_ret_type<typename
					 std::decay<decltype(*beg_iter)>
					 ::type>::type string_t;

	string_t string;

	join(beg_iter, end_iter, separator,
	     std::back_insert_iterator<string_t>(string));

	return string;
}

template<typename InputIterator, typename sep_type, size_t n>
auto join(InputIterator beg_iter,
	  InputIterator end_iter,
	  const sep_type (&separator)[n])
	-> typename string_ret_type<typename std::decay<decltype(*beg_iter)>
				    ::type>::type
{
#ifdef LIBCXX_INTERNAL_DEBUG
	LIBCXX_INTERNAL_DEBUG;
#endif

	return join(beg_iter, end_iter, &separator[0]);
}

template<typename InputIterator, typename sep_type,size_t n>
auto join(InputIterator beg_iter,
	  InputIterator end_iter,
	  sep_type (&separator)[n])
	-> typename string_ret_type<typename std::decay<decltype(*beg_iter)>
				    ::type>::type
{
#ifdef LIBCXX_INTERNAL_DEBUG
	LIBCXX_INTERNAL_DEBUG;
#endif

	return join(beg_iter, end_iter, &separator[0]);
}

template<typename Container, typename sep_type>
auto join(const Container &container,
	  const sep_type &separator)
	-> typename string_ret_type<
		typename std::remove_reference<decltype(*std::begin(container))>
		::type >::type
{
	typedef typename string_ret_type<
		typename std::remove_reference<decltype(*std::begin(container))>
		::type >::type string_t;

	string_t string;

	join(std::begin(container), std::end(container), separator,
	     std::back_insert_iterator<string_t>(string));

	return string;
}

template<typename Container, typename sep_type, size_t n>
auto join(const Container &container,
	  const sep_type (&separator)[n])
	-> typename string_ret_type<
		typename std::remove_reference<decltype(*std::begin(container))>
		::type >::type 
{
#ifdef LIBCXX_INTERNAL_DEBUG
	LIBCXX_INTERNAL_DEBUG;
#endif

	return join(container, &separator[0]);
}

template<typename Container, typename sep_type, size_t n>
auto join(const Container &container,
	  sep_type (&separator)[n])
	-> typename string_ret_type<
		typename std::remove_reference<decltype(*std::begin(container))>
		::type >::type 
{
#ifdef LIBCXX_INTERNAL_DEBUG
	LIBCXX_INTERNAL_DEBUG;
#endif

	return join(container, &separator[0]);
}

#if 0
{
#endif
}
#endif
