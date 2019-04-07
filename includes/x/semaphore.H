/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_semaphore_H
#define x_semaphore_H

#include <x/property_valuefwd.H>
#include <x/ptr.H>
#include <x/obj.H>
#include <x/sipobj.H>
#include <x/logger.H>
#include <x/namespace.h>

#include <queue>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A reference-counted semaphore object

//! \see semaphore

class semaphoreObj : virtual public obj {

	//! The logger object

	LOG_CLASS_SCOPE;

	//! The property
	property::value<size_t> &semaphore_size;

	//! Semaphore count
	size_t count;

	class acquired_mcguffin;
public:

	friend class acquired_mcguffin;

	//! Constructor
	semaphoreObj(//! Property that sets the maximum semaphore count
		     property::value<size_t> &semaphore_sizeArg);

	//! Destructor

	~semaphoreObj();

	//! Semaphore owners

	//! An owner of the semaphore can either subclass this and
	//! implement process(), or use create() with a functor.
	//!
	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::semaphore::base::ownerObj

	class ownerObj : virtual public obj {

	public:
		//! Constructor
		ownerObj();

		//! Destructor
		~ownerObj();

		//! This object has the semaphore

		virtual void process(//! Semaphore mcguffin
				     const ref<obj> &mcguffin)=0;
	};

	//! Submit a request for the semaphore
	void request(//! Who wants it
			const ref<ownerObj> &req,

			//! How much the semaphore should be incremented by
			size_t howmany);

private:
	//! A mcguffin got released
	void release(size_t cnt) noexcept LIBCXX_INTERNAL;

	class pendingObj;

	//! Give the semaphore to pending requesters, as much as possible
	void process(const ref<ownerObj> *push,
		     size_t npush,
		     size_t orpop) noexcept LIBCXX_INTERNAL;

	//! Waiting queue for the semaphore
	std::queue<ref<pendingObj> > pending_list;
};

class semaphoreBase;

//! A reference-counted semaphore object

//! \code
//! INSERT_LIBX_NAMESPACE::property::value<size_t> max_filedescs(L"max_filedescs");
//!
//! INSERT_LIBX_NAMESPACE::semaphore s=INSERT_LIBX_NAMESPACE::semaphore::create(max_filedescs);
//!
//! class ownerObj : public INSERT_LIBX_NAMESPACE::semaphore::ownerObj {
//!
//! // ...
//!
//!     void process(const INSERT_LIBX_NAMESPACE::ref<INSERT_LIBX_NAMESPACE::obj> &mcguffin) override;
//! };
//!
//! auto p=ref<ownerObj>::create();
//!
//! // ...
//!
//! s->request(p, 1);
//!
//! \endcode
//!
//! The property specified when constructing the semaphore sets the maximum
//! semaphore count. request() invokes the submitted owner's process() method
//! when it's possible to increment the semaphore's counter by the value of the
//! the parameter to request() with the count staying at or below the maximum
//! semaphore count.
//!
//! The semaphore's counter gets incremented before invoking process(). The
//! semaphore's counter gets decremented back when the mcguffin that's passed
//! to process() goes out of scope. That may be immediately upon returning from
//! process(), or, if process() stashes away the mcguffin somewhere, when it
//! goes out of scope and gets destroyed later.
//!
//! The semaphore holds only a weak reference on the requesting owners, which
//! may go out of scope and get destroyed without having their process() method
//! called.
//!
//! Any thread that calls request(), or releases the last reference on a
//! mcguffin, may get hijacked to invoke process() of pending owners. process()
//! needs to be mindful, and not engage in any time-consuming processing.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::semaphore::base::owner owner=
//!    INSERT_LIBX_NAMESPACE::semaphore::base::owner::create(
//!       []
//!       (const INSERT_LIBX_NAMESPACE::ref<INSERT_LIBX_NAMESPACE::obj> &mcguffin)
//!       {
//!       });
//!
//! s->request(owner, 1);
//!
//! \endcode
//!
//! An owner handle may also be create()d. create() takes a functor that
//! gets invoked from process().

typedef ref<semaphoreObj, semaphoreBase> semaphore;

//! A nullable pointer to a reference-counted semaphore object.

//! \see semaphore

typedef ptr<semaphoreObj, semaphoreBase> semaphoreptr;

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::semaphore::base or \c INSERT_LIBX_NAMESPACE::semaphoreptr::base

class semaphoreBase : public ptrref_base {

public:

	class ownerBase;

	//! Convenience typedef
	typedef semaphoreObj::ownerObj ownerObj;

	//! Convenience typedef
	typedef ref<ownerObj, ownerBase> owner;

	//! Convenience typedef
	typedef ptr<ownerObj, ownerBase> ownerptr;

};

//! Base class for semaphore owner objects.

//! Implements a custom create() for \ref semaphoreObj::ownerObj "semaphore owners".

class semaphoreBase::ownerBase : public ptrref_base {

public:

	//! Helper subclass that invokes a functor

	template<typename functor_type>
	class ownerImpl : public semaphoreObj::ownerObj {

	public:
		//! The functor
		functor_type functor;

		//! Constructor
		template<typename argType>
		ownerImpl(argType &&arg)
			: functor(std::forward<argType>(arg))
		{
		}

		//! Destructor
		~ownerImpl()
		{
		}

		void process(const ref<obj> &mcguffin) override
		{
			functor(mcguffin);
		}
	};

	//! Factory with the custom create() method.

	template<typename ptrrefType> class objfactory {
	public:

		//! Custom create() method for owner objects.

		template<typename functor_type>
		static ptrrefType create(//! Who wants it
					 functor_type &&functor)
		{
			return ref<ownerImpl<typename std::decay<functor_type>
					     ::type>>
				::create(std::forward<functor_type>(functor));
		}
	};
};

#if 0
{
#endif
}
#endif
