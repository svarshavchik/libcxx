/*
** Copyright 2013-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_dtdfwd_H
#define x_xml_dtdfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

class dtdObj;

//! A reference-counted XML external or internal subset readonly handle

//! This is a \ref ref "reference" to an object that represents an internal
//! or an external DTD subset.

typedef ref<dtdObj> dtd;

//! A nullable reference pointer to an \ref dtd "XML DTD subset".
typedef ptr<dtdObj> dtdptr;

//! A reference to a constant \ref dtd "XML DTD subset".
typedef const_ref<dtdObj> const_dtd;

//! A nullable reference pointer to a constant \ref dtd "XML DTD subset".
typedef const_ptr<dtdObj> const_dtdptr;

#if 0
{
#endif
}
#endif
