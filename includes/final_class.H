/*
** Copyright 2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_final_class_H
#define x_final_class_H

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Simulate a final class

//! \code
//! class X : virtual private final_class<X> {
//!    // ...
//! };
//! \endcode
//!
//! Prevents X from being subclassed.

template<typename T>
class LIBCXX_HIDDEN final_class {

	//! Private constructor accessible only by the final class.

	final_class() {}

public:
	//! Allow the final class access to the constructor.

	friend T;
};


#if 0
{
#endif
}
#endif

