/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_threads_singleton_H
#define x_threads_singleton_H

#include <x/threads/runfwd.H>
#include <x/weakptr.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Private class of thread-scoped singletons

class run_async::singletonbase {

protected:

	//! Put the singleton object on the local thread's stack

	static bool savestrongobject(const ref<obj> &refobj);

	//! Two singleton objects declared in scope
	static void morethanone() __attribute__((noreturn));
};

//! threadsingleton is only defined if the template parameter is a \ref ptrAnchor "ptr".

//! The default class is undefined, and a specialization for a \c ptr
//! contains the actual implementation.

template<typename ref_type>
class threadsingleton;

//! A singleton at the thread level

//! \code
//! class myObject : virtual public INSERT_LIBX_NAMESPACE::obj {
//!
//! // ...
//! };
//!
//! INSERT_LIBX_NAMESPACE::threadsingleton<ptr<myObject> > instance;
//!
//! // ...
//!
//! instance.get()->method();
//! \endcode
//!
//! An instance of \c INSERT_LIBX_NAMESPACE::threadsingleton must be
//! declared in static scope. It may not be declared on the stack.
//!
//! Only one instance of a INSERT_LIBX_NAMESPACE::threadsingleton with the
//! same class parameter may exist.
//!
//! The template parameter to \c INSERT_LIBX_NAMESPACE::thread::singleton
//! must be a \ref ptr "reference pointer". Each execution thread that invokes
//! get() receives a pointer reference to a single, per-thread, instance of the
//! object.
//!
//! get() should not be invoked from constructors or destructors, as they may
//! be invoked when an execution thread is in the middle of being initialized,
//! or destroyed, and the underlying local thread object pool is not
//! available. In those cases, get() returns a null pointer reference.
//!
//! The template class must have a default constructor, and the object gets
//! constructed the first time each execution thread invokes get().
//!

template<typename obj_type, typename base_class>
class threadsingleton<ptr<obj_type, base_class> >
	: private run_async::singletonbase {

	//! Catch duplicate declarations in static scope
	static __thread bool initialized;

	//! Flag: destructor already ran, thread must be winding down.
	static __thread bool destroyed;

public:

	//! Deleted copy constructor

	threadsingleton(const threadsingleton &)=delete;

	//! Deleted assignment operator
	threadsingleton &operator=(const threadsingleton &)=delete;

	//! The pointer type

	typedef ptr<obj_type, base_class> ptr_t;

private:

	//! A thread-local instance of this class
	static __thread obj_type *instance;

	//! This subclass gets instantiated, and it updates instance accordingly
	class subclass : public obj_type {

	public:
		//! The constructor points the instance pointer to this.
		subclass()
		{
			instance=this;
		}

		//! The destructor sets instance to NULL, and destroyed to true.
		~subclass()
		{
			instance=0;
			destroyed=true;
		}
	};

public:
	//! Constructor at static scope
	threadsingleton()
	{
		if (initialized)
			morethanone();

		initialized=true;
	}

	//! Destructor
	~threadsingleton()
	{
	}

	//! Return this thread's singleton value.

	static ptr_t get()
	{
		if (destroyed)
			return ptr_t();

		if (!instance)
		{
			auto p=ref<subclass>::create();

			if (!run_async::singletonbase::savestrongobject(p))
				return ptr_t();
		}

		return ptr_t(instance);
	}
};

template<typename obj_type, typename base_class>
__thread bool threadsingleton<ptr<obj_type, base_class> >::initialized=false;

template<typename obj_type, typename base_class>
__thread bool threadsingleton<ptr<obj_type, base_class> >::destroyed=false;

template<typename obj_type, typename base_class>
__thread obj_type *threadsingleton<ptr<obj_type, base_class> >::instance=NULL;

#if 0
{
#endif
}
#endif
