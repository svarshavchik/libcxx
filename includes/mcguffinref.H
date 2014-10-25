/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mcguffinref_H
#define x_mcguffinref_H

#include <x/ptr.H>
#include <x/weakptr.H>
#include <x/ondestroy.H>
#include <x/namespace.h>

#include <mutex>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename ref_type> class mcguffinrefObj;

//! Mcguffin destructor callback trigger

template<typename ref_type>
class mcguffinref_cbObj : virtual public obj {

public:
	//! The connection to the controller
	weakptr<ptr<mcguffinrefObj<ref_type> > > disconnect_ref;

	//! Constructor
	mcguffinref_cbObj() noexcept
	{
	}

	//! Destructor
	~mcguffinref_cbObj() noexcept
	{
	}

	//! Destructor callback
	void destroyed() noexcept
	{
		ptr<mcguffinrefObj<ref_type> > getptr(disconnect_ref.getptr());

		if (!getptr.null())
		{
			ref_type p;

			{
				std::lock_guard<std::mutex> lock(getptr->mutex);

				p=getptr->p;
				getptr->p=ref_type();
			}
			getptr->destroyed(p);
		}
	}
};

//! Mcguffin-based reference

//! \see mcguffinref

template<typename ref_type>
class mcguffinrefObj : virtual public obj {

	//! The reference
	ref_type p;

	//! Mutex that protects the reference.
	std::mutex mutex;

	//! Destructor callback handle
	ondestroyptr ondestroycb;

public:
	friend class mcguffinref_cbObj<ref_type>;

	//! Retrieve the reference

	ref_type getptr()
	{
		std::lock_guard<std::mutex> lock(mutex);

		return p;
	}
		
	//! Constructor
	mcguffinrefObj()

	{
	}

	//! Destructor
	~mcguffinrefObj() noexcept
	{
	}

	//! Install the reference

	void install(//! The reference
		     const ref_type &ptrArg,

		     //! The mcguffin
		     const ref<obj> &mcguffin)
	{
		std::lock_guard<std::mutex> lock(mutex);

		p=ref_type();
		ondestroycb=ondestroyptr();

		auto cb=ref<mcguffinref_cbObj<ref_type> >::create();

		auto d=ondestroy::create([cb]
					 {
						 cb->destroyed();
					 }, mcguffin);

		cb->disconnect_ref=
			ptr<mcguffinrefObj<ref_type> >(this);

		p=ptrArg;
		ondestroycb=d;
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
