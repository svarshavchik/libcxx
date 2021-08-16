/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#define LIBCXX_TEMPLATE_DECL_INSERT_ITER_TYPE \
    std::back_insert_iterator<std::basic_string<LIBCXX_CHAR_TYPE> >
#include <x/join_to5.H>
#undef LIBCXX_TEMPLATE_DECL_INSERT_ITER_TYPE

#define LIBCXX_TEMPLATE_DECL_INSERT_ITER_TYPE \
    std::ostreambuf_iterator<LIBCXX_CHAR_TYPE>
#include <x/join_to5.H>
#undef LIBCXX_TEMPLATE_DECL_INSERT_ITER_TYPE

#ifndef DOXYGEN
LIBCXX_TEMPLATE_DECL template std::basic_string<LIBCXX_CHAR_TYPE>
join(LIBCXX_CONTAINER_ITERATOR,
     LIBCXX_CONTAINER_ITERATOR,
     const std::basic_string<LIBCXX_CHAR_TYPE> &);
#endif
