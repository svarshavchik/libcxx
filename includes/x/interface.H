/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_interface_H
#define x_interface_H

#include <x/obj.H>
#include <x/ref.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A reference to an object that implements a certain interface

//! This template class stores a reference to a generic object, ptr<obj>.
//! The object being referenced must be either a subclass of the template
//! class parameter, or have the template class parameter somewhere in its
//! class hierachy.
//!
//! The constructor (and the assignment operator) takes a generic object
//! reference, and dynamic_casts it to the expected class. The class pointer
//! is saved for faster access.

template<typename refType> class interface {

	//! The generic object reference
	ptr<obj> objref;

	//! The class pointer.
	refType *objtype;

public:
	//! Construct a new interface reference

	interface(//! An object reference

		  //! If this object reference is set, it must refer to
		  //! a subclass of refType, or have refType somewhere in
		  //! its class hierarchy

		  const ptr<obj> &arg=ptr<obj>() ): objtype(0)
	{
		setref(arg);
	}

	//! Default destructor
	~interface()
	{
	}

	//! Reassign this interface pointer.

	interface<refType> &operator=(const ptr<obj> &arg)
	{
		setref(arg);
	}

	//! Convert a generic object reference to a specific pointer

	//! The object being referenced must have refType in its hierarchy.
	//! Find it.

	static refType *getRef(const ref<obj> &arg)
	{
		refType *s=dynamic_cast<refType *>(&*arg);

		if (!s)
			invalid_cast(typeid(*arg).name(),
				     typeid(*s).name());
		return s;
	}

	//! Retrieve the pointer to the interface

	refType *getptr() const noexcept
	{
		return objtype;
	}

private:
	//! Update the object reference pointer.

	void setref(const ptr<obj> &arg)
	{
		refType *s=NULL;

		if (!arg.null()) s=getRef(arg);
		objref=arg;
		objtype=s;
	}

};

#if 0
{
#endif
}
#endif
