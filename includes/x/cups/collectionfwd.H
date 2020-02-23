/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_cups_collectionfwd_H
#define x_cups_collectionfwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>
#include <unordered_map>
#include <string>

namespace LIBCXX_NAMESPACE {
	namespace cups {
#if 0
	}
}
#endif

class collectionObj;

//! A collection of option values.

//! This encapsulates a collection of IPP attributes for a single
//! destination option. This object is just a map of strings. The map's
//! values are \ref option_values_t "option values". This represents a
//! named collection of option values.

typedef ref<collectionObj> collection;

//! A nullable pointer reference to an attribute collection.

typedef ptr<collectionObj> collectionptr;

//! A constant attribute collection.

typedef const_ref<collectionObj> const_collection;

//! A nullable pointer reference to a const attribute collection.

typedef const_ptr<collectionObj> const_collectionptr;

#if 0
{
	{
#endif
	}
}
#endif
