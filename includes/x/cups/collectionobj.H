/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_cups_collectionobj_H
#define x_cups_collectionobj_H

#include <x/cups/collectionfwd.H>
#include <x/cups/destinationfwd.H>
#include <x/obj.H>
#include <unordered_map>

namespace LIBCXX_NAMESPACE::cups {
#if 0
}
#endif

//! Collection object implementation

//! \see collection

typedef std::unordered_map<std::string, option_values_t> collection_values_t;

//! Implementation object.

//! \see collection

class collectionObj : public collection_values_t,
		      virtual public obj {

public:

	using collection_values_t::collection_values_t;

	//! Destructor
	~collectionObj();
};

#if 0
{
#endif
}
#endif
