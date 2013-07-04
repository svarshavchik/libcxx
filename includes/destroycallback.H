/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_destroycallback_H
#define x_destroycallback_H

#include <x/destroycallbackfwd.H>
#include <x/destroycallbackobj.H>
#include <x/exception.H>
#include <x/namespace.h>

#include <type_traits>

	//! 
namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::destroyCallback::base

class destroyCallbackBase
{
	//! An exception occured in the destructor callback functor

	static void report_exception(const exception &);

	//! Helper class created by create()

	template<typename F> class functorCallbackObj
		: public destroyCallbackObj {

		//! The functor
		F functor;

	public:
		//! Constructor
		template<typename FArg>
		functorCallbackObj(FArg && functorArg)
			: functor(std::forward<FArg>(functorArg))
		{
		}

		//! Destructor
		~functorCallbackObj() noexcept
		{
		}

		//! The destructor callback

		void destroyed() noexcept override
		{
			try {
				functor();
			} catch (const exception &e)
			{
				destroyCallbackBase::report_exception(e);
			}
		}
	};

public:
	//! The create() factory.

	template<typename ptrrefType> class objfactory {

	public:
		//! Construct a destructor callback from a functor

		//! This is a convenience function that takes a functor and
		//! constructs a destructor callback, suitable to be passed
		//! directly to obj::addOnDestroy(), that invokes the given
		//! functor in its destroyed().

		template<typename F>
		static ptrrefType create(//! Callback functor
					 F && functor)
		{
			typedef typename std::decay<F>::type functor_type;

			// If the passed functor is itself a reference, it is
			// a near certainty that this is wrong.

			typedef typename std::enable_if<
				! std::is_lvalue_reference<functor_type>::value &&
				! std::is_rvalue_reference<functor_type>::value,
				functor_type>::type functor_cannot_be_a_reference __attribute__((unused));

			return ref<functorCallbackObj< functor_type > >
				::create(std::forward<F>(functor));
		}

		//! The default constructor

		static ptrrefType create()
		{
			return ptrrefBase::objfactory<ptrrefType>::create();
		}
	};
};

#if 0
{
#endif
}
#endif
