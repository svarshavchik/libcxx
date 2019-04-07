/*
** Copyright 2014-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fixed_semaphorefwd_H
#define x_fixed_semaphorefwd_H

#include <x/ref.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class fixed_semaphoreObj;
class fixed_semaphoreBase;

//! A fixed semaphore

//! Implement a semaphore that can either be acquired immediately, or not.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::fixed_semaphore s=INSERT_LIBX_NAMESPACE::fixed_semaphore::create(4);
//!
//! INSERT_LIBX_NAMESPACE::fixed_semaphore::base::acquiredptr acquired=s->acquire(
//!      []
//!      {
//!           return true;
//!      },
//!      []
//!      {
//!           // ...
//1      }, 2);
//! \endcode
//!
//! The constructor takes the number of available semaphore instances, which
//! defaults to 1.
//!
//! acquire() immediately returns a null
//! \ref fixed_semaphoreBase::acquiredptr "acquiredptr"
//! if the number of available semaphores is fewer than its third, optional
//! parameter, which defaults to 1.
//!
//! Otherwise, the first functor gets invoked. If it returns \c false
//! a null \c acquiredptr also gets returned.
//!
//! Otherwise, the number of available semaphores gets decremented by the
//! third parameter (the fixed semaphore object remains locked for the
//! duration of the functor call) and a non-null \c acquiredptr gets
//! returned, a handle for the requested number of acquired semaphores.
//!
//! The handle maintains an internal reference on the semaphore. When
//! the last reference to the handle goes out of scope and it gets destroyed,
//! the second functor that was passed to acquire() gets invoked.
//!
//! The second functor cannot throw exceptions. When the second functor returns
//! the number of available semaphore instances gets incremented by the
//! number of semaphores that were originally acquired.

typedef ref<fixed_semaphoreObj, fixed_semaphoreBase> fixed_semaphore;

//! A nullable pointer reference to a \ref fixed_semaphore "fixed semaphore".
typedef ptr<fixed_semaphoreObj, fixed_semaphoreBase> fixed_semaphoreptr;

//! A reference to a constant \ref fixed_semaphore "fixed semaphore".
typedef const_ref<fixed_semaphoreObj,
		  fixed_semaphoreBase> const_fixed_semaphore;

//! A nullable pointer reference to a constant \ref fixed_semaphore "fixed semaphore".
typedef const_ptr<fixed_semaphoreObj,
		  fixed_semaphoreBase> const_fixed_semaphoreptr;


#if 0
{
#endif
}
#endif

