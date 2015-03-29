/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_refiteratorfwd_H
#define x_refiteratorfwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

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

//! A referenced-counted pointer that implements iterator requirements.

//! \see refobjiterator

template<typename obj_type, typename baseType=ptrrefBase>
using refiterator=refobjiterator<ref<obj_type, baseType>>;

//! A referenced-counted pointer that implements iterator requirements.

//! \see refobjiterator

template<typename obj_type, typename baseType=ptrrefBase>
using ptriterator=refobjiterator<ptr<obj_type, baseType>>;

//! A referenced-counted pointer that implements iterator requirements.

//! \see refobjiterator

template<typename obj_type, typename baseType=ptrrefBase>
using const_refiterator=refobjiterator<const_ref<obj_type, baseType>>;

//! A referenced-counted pointer that implements iterator requirements.

//! \see refobjiterator

template<typename obj_type, typename baseType=ptrrefBase>
using const_ptriterator=refobjiterator<const_ptr<obj_type, baseType>>;

//! A referenced-counted output iterator.

//! \see outputrefiteratorObj

template<typename value_type>
using outputrefiterator=refiterator<outputrefiteratorObj<value_type>>;

//! A referenced-counted output iterator.

//! \see outputrefiteratorObj

template<typename value_type>
using outputrefiteratorptr=ptriterator<outputrefiteratorObj<value_type>>;

//! A referenced-counted output iterator.

//! \see outputrefiteratorObj

template<typename value_type>
using const_outputrefiterator=const_refiterator<outputrefiteratorObj<value_type>>;

//! A referenced-counted output iterator.

//! \see outputrefiteratorObj

template<typename value_type>
using const_outputrefiteratorptr=const_ptriterator<outputrefiteratorObj<value_type>>;

#if 0
{
#endif
}
#endif

