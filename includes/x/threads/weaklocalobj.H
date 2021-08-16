/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_threads_localobj_H
#define x_threads_localobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/mcguffinref.H>
#include <x/threads/weaklocalfwd.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A reference-counted object that implements a weak thread local object

//! \see makeweakthreadlocal()

template<typename objClass, typename baseType>
class weakthreadlocalObj : virtual public obj {

	//! A mcguffin-based reference to the underlying object
	ref<mcguffinrefObj<ptr<objClass, baseType> > > refobj;

public:

	//! Constructor
	weakthreadlocalObj(//! The object to make a weak therad local object
			   const ref<objClass, baseType> &refobjArg,

			   //! The local thread's mcguffin
			   const ref<obj> &mcguffin)
		: refobj(ref<mcguffinrefObj<ptr<objClass, baseType> > >
			 ::create())
	{
		refobj->install(refobjArg, mcguffin);
	}

	//! Destructor
	~weakthreadlocalObj() {}

	//! Retrieve the underlying object.
	ptr<objClass, baseType> getptr() const
	{
		return refobj->getptr();
	}
};

#if 0
{
#endif
}

#endif
