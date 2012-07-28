/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_propertyvaluefwd_H
#define x_propertyvaluefwd_H

#include <x/tostring.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace property {
#if 0
	};
};
#endif

//! Property name type
typedef std::wstring propvalue;

//! For internal use

#define LIBCXX_PROPERTY_NAME(n) L ## n


template<typename argType,
	 typename stringType=stringize<propvalue, argType> > class value;

#if 0
{

	{
#endif
	}
}
#endif
