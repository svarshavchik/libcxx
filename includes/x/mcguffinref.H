/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mcguffinref_H
#define x_mcguffinref_H

#include <x/ptr.H>
#include <x/mpobj.H>
#include <x/weakptr.H>
#include <x/ondestroy.H>
#include <x/mpobj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename ref_type> class mcguffinrefObj;

//! Mcguffin destructor callback trigger

//! \internal

template<typename ref_type>
class mcguffinref_cbObj : virtual public obj {

	//! Type of disconnect_ref
	typedef mpobj<weakptr<ptr<mcguffinrefObj<ref_type> > >> disconnect_ref_t;

	//! The connection to the controller
	disconnect_ref_t disconnect_ref;

public:

	//! Constructor
	mcguffinref_cbObj() noexcept
	{
	}

	//! Destructor
	~mcguffinref_cbObj()
	{
	}

	//! Arm the trigger

	void set_ref(const ptr<mcguffinrefObj<ref_type>> &refptr)
	{
		typename disconnect_ref_t::lock lock{disconnect_ref};

		*lock=refptr;
	}

	//! Get the trigger object.

	auto get_ref()
	{
		typename disconnect_ref_t::lock lock{disconnect_ref};

		return lock->getptr();
	}

	//! Destructor callback
	void destroyed() noexcept
	{
		auto getptr=get_ref();

		if (!getptr.null())
		{
			ref_type p;

			{
				typename mpobj<ref_type>::lock lock{getptr->p};

				p=*lock;

				*lock=ref_type();
			}
			getptr->destroyed(p);
		}
	}
};

//! Mcguffin-based reference

//! \see mcguffinref

template<typename ref_type>
class mcguffinrefObj : virtual public obj {

public:
	//! Mutex-protected reference type

	typedef mpobj<ref_type> p_t;

	//! The reference
	p_t p;

	// //! Destructor callback handle
	// ondestroyptr ondestroycb;

	friend class mcguffinref_cbObj<ref_type>;

	//! Retrieve the reference

	ref_type getptr()
	{
		typename p_t::lock lock{p};

		return *lock;
	}

	//! Constructor
	mcguffinrefObj()

	{
	}

	//! Destructor
	~mcguffinrefObj()
	{
	}

	//! Install the reference

	void install(//! The reference
		     const ref_type &ptrArg,

		     //! The mcguffin
		     const ref<obj> &mcguffin)
	{
		typename p_t::lock lock{p};

		*lock=ref_type();
		// ondestroycb=ondestroyptr();

		auto cb=ref<mcguffinref_cbObj<ref_type> >::create();

		auto d=ondestroy::create([cb]
					 {
						 cb->destroyed();
					 }, mcguffin);

		cb->set_ref(ptr<mcguffinrefObj<ref_type> >(this));

		*lock=ptrArg;
		// ondestroycb=d;
	}

	//! Optional callback
	virtual void destroyed(const ref_type &p)
	{
	}
};

//! A reference that depends on a mcguffin

//! After constructing this object, invoke its install() with a reference and
//! a \ref ptr "mcguffin".
//!
//! Use getptr() to retrieve the previously installed reference.
//!
//! When the mcguffin gets destroyed, getptr() returns null from that point on.
//!
//! destroyed(), if defined by the subclass, gets invoked when the mcguffin
//! gets destroyed, and received the installed reference. getptr() already
//! returns null, destroyed() in the subclass gets last dibs on it.

template<typename objClass>
using mcguffinref=ref<mcguffinrefObj<ptr<objClass>>>;

#if 0
{
#endif
}
#endif
