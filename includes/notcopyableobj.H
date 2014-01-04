/*
** Copyright 2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_notcopyableobj_H
#define x_notcopyableobj_H

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Copy constructors and assignment operators are prohibited.

class notcopyableObj {

public:

	inline notcopyableObj() {}

	//! Not implemented

	notcopyableObj(const notcopyableObj &)=delete;

	//! Not implemented
	notcopyableObj &operator=(const notcopyableObj &)=delete;

	//! Not implemented
	notcopyableObj(notcopyableObj &&)=delete;

	//! Not implemented
	notcopyableObj &operator=(notcopyableObj &&)=delete;
};

#if 0
{
#endif
}
#endif

