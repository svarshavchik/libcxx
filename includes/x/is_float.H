/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_is_float_H
#define x_is_float_H

#include <x/namespace.h>
#include <x/nonconst_type.H>

namespace LIBCXX_NAMESPACE {

template<typename class_type> class is_float_nonconst;


//! A specialization that identifies native float types

//! \c is_float<T>::flag is \c true if \c T is either \c float, \c double,
//! or \c long \c double. In all other cases its \c false.

template<typename class_type> class is_float
	: public is_float_nonconst<typename nonconst_type<class_type>::type> {
};

//! Implement is_float after any const-ness is removed

template<typename class_type>
class is_float_nonconst {

public:
	//! The default value of \c is_float<T>::flag is \c false

	static constexpr bool flag=false;
};

//! \c is_float specialization for floats

//! \see is_float
//!
template<>
class is_float_nonconst<float> {

public:

	//! This is a native float value

	static constexpr bool flag=true;
};

//! \c is_float specialization for doubles

//! \see is_float
//!
template<>
class is_float_nonconst<double> {

public:
	//! This is a native float value

	static constexpr bool flag=true;
};

//! \c is_float specialization for long doubles

//! \see is_float
//!
template<>
class is_float_nonconst<long double> {

public:
	//! This is a native float value

	static constexpr bool flag=true;
};

#if 0
};
#endif

#if 0
{
#endif
}
#endif
