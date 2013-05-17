/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_refiteratorfwd_H
#define x_refiteratorfwd_H

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<typename ref_type> class refobjiterator;
template<typename value_type> class outputrefiteratorObj;
template<typename value_type, typename ...TupleType>
class outputrefiteratorTupleObj;

template<typename value_type, typename ...Args>
refobjiterator<ref<outputrefiteratorTupleObj<value_type, typename
					     std::decay<Args>::type...>>>
make_outputrefiterator(Args && ...args);

#if 0
{
#endif
}
#endif

