/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_timertask_H
#define x_timertask_H

#include <x/threads/timertaskfwd.H>
#include <x/threads/timertaskobj.H>
#include <x/ref.H>
#include <type_traits>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for timertaskObj.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::timertask::base or
//! \c INSERT_LIBX_NAMESPACE::timertaskptr::base.

class timertaskBase : public ptrrefBase {

	template<typename tuple_args> class invoke;

public:

	//! A helper object instantiated by make_timer_task

	template<typename Functor> class redirectObj : public timertaskObj {

		//! The saved functor
		Functor functor;

	public:
		//! Constructor
		redirectObj(const Functor &functorArg) : functor(functorArg)
		{
		}
		//! Move constructor
		redirectObj(Functor &&functorArg)
			: functor(std::move(functorArg))
		{
		}
		//! Destructor
		~redirectObj() noexcept
		{
		}

		//! Run the functor
		void run()
		{
			functor();
		}
	};

	//! Construct a timer task from a functor

	//! This function creates a timer task on the fly, that invokes some
	//! functor.

	template<typename Functor>
	static inline ref<redirectObj<Functor> >
	make_timer_task(const Functor &functor)
	{
		return ref<redirectObj<Functor> >::create(functor);
	}

	//! Construct a timer task from a functor

	//! This function creates a timer task on the fly, that invokes some
	//! functor.

	template<typename Functor>
	static inline ref<redirectObj<typename std::decay<Functor>::type> >
	make_timer_task(Functor &&functor)
	{
		return ref<redirectObj<typename std::decay<Functor>::type> >
			::create(std::move(functor));
	}
};

#if 0
{
#endif
}

#endif
