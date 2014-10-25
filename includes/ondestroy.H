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


//! Proxy destructor callback used by onAnyDestroyed().

class onAnyDestroyCallbackObj : virtual public obj {

public:

	//! Cancellable destructor callbacks that were set up.

	class cbListObj : public std::list<LIBCXX_NAMESPACE::ondestroy>,
			  virtual public obj {

	public:
		cbListObj();
		~cbListObj() noexcept;
	};

	//! Constructor
	onAnyDestroyCallbackObj(const ref<cbListObj> &cb);

	//! Destructor
	~onAnyDestroyCallbackObj() noexcept;

	//! One of the mcguffins went out of scope. We're done, cancel the whole show.
	void destroyed() noexcept;

private:

	//! Cancellable destructor callbacks that were set up

	//! They get cancelled by the virtue of
	mpobj<ptr<cbListObj> > callbacks;
};


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
	auto cblist=ref<onAnyDestroyCallbackObj::cbListObj>::create();

	auto mcguffin=ref<onAnyDestroyCallbackObj>::create(cblist);

	while (b != e)
	{
		cblist->push_back(ondestroy::create([mcguffin]
						    {
							    mcguffin->destroyed();
						    }, *b, true));
		++b;
	}

	mcguffin->ondestroy([functor] { functor(); });
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
