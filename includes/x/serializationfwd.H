/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_serializationfwd_H
#define x_serializationfwd_H

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {


	namespace serialization {
#if 0
	};
};
#endif

//! Default serialization traits

//! The serialization traits class defines a single static function:
//! classcreate(), which initializes a reference to an object just before
//! the referenced object gets deserialized.

class default_traits {

public:
	//! Instantiate a reference to a deserialized class

	template<typename ref_type>
	static void classcreate(ref_type &ptr)
	{
		if (ptr.null())
			ptr=ref_type::create();
	}
};

#if 0
{
	{
#endif
	}
}
#endif
