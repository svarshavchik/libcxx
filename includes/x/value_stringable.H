/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_valuestringable_H
#define x_valuestringable_H

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Class stringability

//! A class that implements toString()/fromString() and/or
//! toWideString()/fromWideString() must declare
//! "static const stringable_t stringable=value" for one of these values:

typedef enum {
	//! The class implements toString() and fromString() only.
	class_tostring,

	//! The class implements toString() and fromString() only, ignoring the requested locale, always using UTF-8

	class_tostring_utf8,

	//! The class implements toWideString() and fromWideString() only.
	class_towstring,

	//! The class implements toString(), fromString(), toWideString() and fromWideString()
	class_toboth } stringable_t;

#if 0
{
#endif
}
#endif
