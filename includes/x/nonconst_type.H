/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_nonconst_type_H
#define x_nonconst_type_H

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif


//! A specialization that removes const-ness from a class

//! \c nonconst_type<T>::type resolves to \c T if it not a const type, or to
//! \c T without the const qualifier.

template<typename value_type> class nonconst_type {

public:

	//! The non-const type

	typedef value_type type;
};

//! Specialization for const types

template<typename const_value_type> class nonconst_type<const const_value_type>
{

public:
	//! Remove const-ness

	typedef const_value_type type;
};

#if 0
{
#endif
}
#endif
