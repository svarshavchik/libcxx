/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ondestroy_H
#define x_ondestroy_H

#include <x/ref.H>
#include <x/ondestroyfwd.H>
#include <x/ondestroyobj.H>
#include <x/mpobj.H>
#include <x/namespace.h>

#include <list>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for a helper callback for on_any_destroyed().

class onAnyDestroyBase {

public:
	//! Callback
	virtual bool more()=0;

	//! Callback
	virtual ref<obj> next()=0;

	template<typename iter_type> class impl;
};

//! Wrapper for a functor

template<typename iter_type>
class onAnyDestroyBase::impl : public onAnyDestroyBase {
public:

	//! Beginning iterator value.
	iter_type b;

	//! Ending iterator value.
	iter_type e;

	//! Constructor
	impl(iter_type bArg, iter_type eArg) : b(bArg), e(eArg) {}

	//! Compare two iterators
	bool more() override
	{
		return b != e;
	}

	//! Return next mcguffin
	virtual ref<obj> next() override
	{
		return *b++;
	}
};

//! Return the mcguffin used by on_any_destroyed()

ref<obj> on_any_destroyed_mcguffin(onAnyDestroyBase &);

//! Invoke a functor when any one of multiple objects gets destroyed.

//! The iterators define an input sequence of objects. The functor
//! gets invoked when the first one of these objects gets destroyed.
//! The functor gets invoked only once. The destruction of remaining
//! objects does not result in additional invocations of the functor.
//!
//! A strong reference to this destructor callback object remains only
//! until functor gets invoked.
//!
//! If the input sequence is empty, the functor gets invoked before
//! the functor returns.

template<typename functor_type, typename iter_type>
void on_any_destroyed(functor_type &&functor,
		      iter_type b, iter_type e)
{
	onAnyDestroyBase::impl<iter_type> wrapper(b, e);

	on_any_destroyed_mcguffin(wrapper)
		->ondestroy(std::forward<functor_type>(functor));
}

//! Base class for \ref ondestroy "destructor references."

class ondestroyBase {

public:

	//! Real create() implementation takes a generic destroy callback

	static ondestroy do_create(const ref<obj::destroyCallbackObj> &cb,
				   const ref<obj> &objArg,
				   bool autodestroy);

	//! Custom create() method implementation.

	template<typename ptrrefType> class objfactory {

	public:
		//! Custom create() method.

		//! Wraps the lambda, and calls do_create().

		template<typename functor_type>
			static inline ptrrefType create(functor_type &&functor,
							const ref<obj> &objArg,
							bool autodestroy=false)
		{
			return do_create(obj::create_ondestroy
					 (std::forward<functor_type>(functor)),
					 objArg, autodestroy);
		}
	};
};

#if 0
{
#endif
}

#endif
