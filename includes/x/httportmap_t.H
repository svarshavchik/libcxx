/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#define LIBCXX_TEMPLATE_ITER_TYPE std::back_insert_iterator<std::vector<httportmapObj::service> >
#include <x/httportmap_to.H>
#undef LIBCXX_TEMPLATE_ITER_TYPE

#define LIBCXX_TEMPLATE_ITER_TYPE std::back_insert_iterator<std::list<httportmapObj::service> >
#include <x/httportmap_to.H>
#undef LIBCXX_TEMPLATE_ITER_TYPE

#define LIBCXX_TEMPLATE_ITER_TYPE std::insert_iterator<std::multiset<httportmapObj::service> >
#include <x/httportmap_to.H>
#undef LIBCXX_TEMPLATE_ITER_TYPE
