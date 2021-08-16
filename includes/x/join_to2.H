/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#define LIBCXX_JOIN_TO3_FIRST
#define LIBCXX_CONTAINER_ITERATOR \
	LIBCXX_CONTAINER_TYPE<std::basic_string<LIBCXX_CHAR_TYPE> >::const_iterator
#include <x/join_to3.H>
#undef LIBCXX_CONTAINER_ITERATOR
#undef LIBCXX_JOIN_TO3_FIRST

#ifndef DOXYGEN

#ifdef LIBCXX_NO_NONCONST_ITERATOR

#else

#define LIBCXX_CONTAINER_ITERATOR \
	LIBCXX_CONTAINER_TYPE<std::basic_string<LIBCXX_CHAR_TYPE> >::iterator
#define LIBCXX_CONTAINER_CONST_ITERATOR \
	LIBCXX_CONTAINER_TYPE<std::basic_string<LIBCXX_CHAR_TYPE> >::const_iterator

template<>
std::back_insert_iterator<std::basic_string<LIBCXX_CHAR_TYPE> >
join(LIBCXX_CONTAINER_ITERATOR beg,
     LIBCXX_CONTAINER_ITERATOR end,
     const std::basic_string<LIBCXX_CHAR_TYPE> &separator,
     std::back_insert_iterator<std::basic_string<LIBCXX_CHAR_TYPE> > iter)
#ifdef LIBCXX_TEMPLATE_SPEC
{
	return join(static_cast<LIBCXX_CONTAINER_CONST_ITERATOR>(beg),
		    static_cast<LIBCXX_CONTAINER_CONST_ITERATOR>(end),
		    separator,
		    iter);
}
#else
;
#endif

template<>
std::back_insert_iterator<std::basic_string<LIBCXX_CHAR_TYPE> >
join(LIBCXX_CONTAINER_ITERATOR beg,
     LIBCXX_CONTAINER_ITERATOR end,
     const LIBCXX_CHAR_TYPE * const &separator,
     std::back_insert_iterator<std::basic_string<LIBCXX_CHAR_TYPE> > iter)
#ifdef LIBCXX_TEMPLATE_SPEC
{
	return join(static_cast<LIBCXX_CONTAINER_CONST_ITERATOR>(beg),
		    static_cast<LIBCXX_CONTAINER_CONST_ITERATOR>(end),
		    separator,
		    iter);
}
#else
;
#endif
#undef LIBCXX_CONTAINER_ITERATOR
#undef LIBCXX_CONTAINER_CONST_ITERATOR

#endif

#endif
