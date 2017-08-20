/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_refiteratorfwd_H
#define x_refiteratorfwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>
#include <x/refptr_traits.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<typename ref_type> class refobjiterator;
template<typename value_type> class outputrefiteratorObj;
template<typename value_type, typename ...TupleType>
class outputrefiteratorTupleObj;

//! A referenced-counted pointer that implements iterator requirements.

//! \see refobjiterator

template<typename obj_type, typename baseType=ptrref_base>

#ifndef DOXYGEN
using refiterator=refobjiterator<ref<obj_type, baseType>>;
#else
class refiterator {};
#endif

//! A referenced-counted pointer that implements iterator requirements.

//! \see refobjiterator

template<typename obj_type, typename baseType=ptrref_base>
#ifndef DOXYGEN
using ptriterator=refobjiterator<ptr<obj_type, baseType>>;
#else
class ptriterator {};
#endif

//! A referenced-counted pointer that implements iterator requirements.

//! \see refobjiterator

template<typename obj_type, typename baseType=ptrref_base>
using const_refiterator=refobjiterator<const_ref<obj_type, baseType>>;

//! A referenced-counted pointer that implements iterator requirements.

//! \see refobjiterator

template<typename obj_type, typename baseType=ptrref_base>
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

template<typename value_type, typename ...Args>
inline refiterator< outputrefiteratorTupleObj<value_type, typename
					      std::decay<Args>::type...> >
make_outputrefiterator(Args && ...args);

#if 0
{
#endif
}
#endif
