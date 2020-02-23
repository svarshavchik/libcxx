/*
** Copyright 2013-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_newdtdfwd_H
#define x_xml_newdtdfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {
	namespace xml {
#if 0
	};
};
#endif

class newdtdObj;

//! A reference-counted XML external or internal subset read/write handle

//! This is a \ref ref "reference" to an object that represents an internal
//! or an external DTD subset that can be modified. The referenced object
//! is a subset of \ref dtd "INSERT_LIBX_NAMESPACE::xml::dtd" that adds methods
//! that modify the subset.

typedef ref<newdtdObj> newdtd;

//! A nullable reference pointer to an \ref dtd "XML NEWDTD subset".
typedef ptr<newdtdObj> newdtdptr;

//! A reference to a constant \ref dtd "XML DTD subset".
typedef const_ref<newdtdObj> const_newdtd;

//! A nullable reference pointer to a constant \ref dtd "XML DTD subset".
typedef const_ptr<newdtdObj> const_newdtdptr;

#if 0
{
	{
#endif
	}
}
#endif
