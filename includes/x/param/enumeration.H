/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_param_enumeration_H
#define x_param_enumeration_H

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
	namespace param {
#if 0
	}
}
#endif

//! An enumeration that does not have a get_list() function.

//! The list of possible values get stored in an array.

template<typename enum_t> class enumeration {

protected:

	//! A list of all values in the enumeration

	static const enum_t manual_enumeration[];

	//! The size of the manual_enumeration array

	static const size_t manual_enumeration_cnt;
};

#if 0
{
	{
#endif
	}
}

#endif
