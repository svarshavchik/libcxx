/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_run_H
#define x_run_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/threads/runfwd.H>
#include <x/threads/runthreadobj.H>
#include <x/threads/runthread_invoke.H>
#include <x/threads/runthreadname.H>
#include <x/threads/runthreadsingleton.H>
#include <tuple>
#include <mutex>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Objects local in scope to the thread

class run_async::localscope {

public:
	//! The thread's mcguffin.

	ref<obj>      mcguffin;

	//! Objects to destroy when the thread terminates
	std::list<ref<obj> > strongobjects;

	//! Pointer to the instance of localscope on the thread's stack

	static __thread localscope *localscopeptr;

	//! The main() scope
	static localscope *mainscope LIBCXX_HIDDEN;

	class mainscope_destructor;

	//! Push an object to mainscope->strongobjects.

	static void mainscope_pushstrongobjects(const ref<obj> &p);

	localscope();
	~localscope();
};

//! Parameters for a new thread that invokes an object's run() method.

//! A reference to an instance of this object gets passed from the parent
//! thread to the child thread.
//!
//! This object holds the pointer to the thread object, a tuple with
//! the arguments to the thread object's run() method, and the promised
//! destination where run()'s return value gets stored.
//!
//! An instance of this object also serves as a mcguffin. When the new thread
//! terminates, it releases the last reference on this object. Its destructor
//! callback places the thread's runthreadObj object into the cleanup queue,
//! to be join()ed.

template<typename refobj_type, typename ret_type, typename ...Args>
class run_async::runthreadparamObj : virtual public obj {

public:

	//! A lock on the thread's singleton mutex, if the thread has one.
	runthreadsingleton::get<typename refobj_type::obj_type> singleton_lock;

	//! The thread whose %run() method gets invoked
	refobj_type refobj;

	//! The arguments to the %run() method.
	std::tuple<Args...> args;

	//! The return value from the thread is promised here
	std::promise<ret_type> promise;

	//! Constructor

	//! Note that the parameter types are specified by an
	//! independent parameter pack. That's because the template
	//! for runthreadparamObj gives the decayed types, and this is the
	//! real deal.

	template<typename ...paramTypes>
	runthreadparamObj(const refobj_type &refobjArg, paramTypes && ...argsArgs)
		: singleton_lock(*refobjArg),
		  refobj(refobjArg),
		  args(argsArgs...)
	{
	}

	//! Invoke the thread object's run() method.
	inline void invoke()
	{
		// Set the thread's name
		run_async::name n(runthreadname
				  ::name<typename std::remove_reference
					 <decltype(*refobj)>::type
					 >::get(*refobj));

		run_invoke_set_promise<ret_type>::invoke(promise, refobj, args);
	}

	//! Destructor
	~runthreadparamObj() noexcept
	{
	}
};

//! Define the method invoked by std::thread

//! The method takes one parameter, a reference to an instance
//! of \ref runthreadparamObj "runthreadparamObj", which the parent thread already constructed.

template<typename args_type> class run_async::run_method_impl {

public:

	//! The method invoked by std::thread

	//! The method parameter is a reference to a \ref runthreadparamObj "runthreadparamObj".
	//!
	//! Call its invoke() method, and capture any exception.

	static
	void run_method(//! An instance of \ref runthreadparamObj "runthreadparamObj".

			const args_type &args)
	{
		localscope thread_local_variables;

		try {
			args->invoke();
		} catch (...) {
			args->promise.set_exception(std::current_exception());
		}
	}
};

template<typename objClass, typename baseType,
	 typename ...Args>
auto run_async::run_thread(const ref<objClass, baseType> &p,
			   Args && ...args)
	-> ref<runthreadObj<decltype(p->run(invoke_run_param_helper<Args>()...))> >
{
	// The return value from the run() method.

	typedef decltype(p->run(invoke_run_param_helper<Args>()...)) ret_type;

	// The parameter to the new thread.
	typedef runthreadparamObj< ref<objClass, baseType>,
				   ret_type,
				   typename std::decay<Args>::type...
				   > runthreadparamObj_t;

        // Create the new parameter.
	auto run_param=ref<runthreadparamObj_t>::create(p, args...);

	// Create where the return value goes.

	auto thr=ref<runthreadObj<ret_type> >
		::create(run_param->promise.get_future());

        // Make run_param a mcguffin that hands over the runthreadobj to
	// the cleanup thread, when it goes out of scope.

        auto cb=ref<runthreadbaseObj::threadCleanupObj>::create(thr);
        run_param->addOnDestroy(cb);

	std::lock_guard<std::mutex> lock(thr->objmutex);

        thr->thr=std::thread(run_method_impl<decltype(run_param)>
			     ::run_method, run_param);

	thr->thr_id=thr->thr.get_id();

        //! Pass back the return value.

        return thr;
}

//! The return value from an object's run() method.

template<typename ret_type>
using runthread=ref< run_async::runthreadObj<ret_type> >;

//! A nullable pointer reference to a return value from an object's run().

//! \see runthread

template<typename ret_type>
using runthreadptr=ptr< run_async::runthreadObj<ret_type> >;

//! A reference to a constant return value from an object's run().

//! \see runthread

template<typename ret_type>
using const_runthread=const_ref< run_async::runthreadObj<ret_type> >;

//! A nullable pointer reference to a constant return value from an object's run().

//! \see runthread

template<typename ret_type>
using const_runthreadptr=const_ptr< run_async::runthreadObj<ret_type> >;

//! Invoke an object's %run() method in another thread.

//! Starts a new thread, which calls an object's %run() method. This run()
//! returns, and the thread object's %run() continues in another thread.
//!
//! The first argument to run() is a reference to an object with a %run()
//! method. The remaining arguments, if any, get forwarded to the %run() method.
//! The arguments must be copy-constructible. The arguments get copied for the
//! new thread, and the copies get given to %run(), in the new thread.
//!
//! The new thread holds its own reference on the thread object whose %run()
//! method it invokes. If all references to the
//! object in the original thread go out of scope, the thread object does not
//! get destroyed until its %run() finishes and returns, and the last reference
//! on the thread object goes out of scope at that time.
//!
//! run() handles baby-sitting of the \c std::thread which executes the thread,
//! making sure that the spawned thread gets joined before the \c std::thread
//! gets destroyed. When the spawned thread terminates, an internal thread
//! takes care of joining it. One internal thread joins all threads that get
//! started by run().
//!
//! run() returns a
//! \ref runthread "INSERT_LIBX_NAMESPACE::runthread<T>", where
//! T is the thread object's %run()'s return value. When the thread object's
//! %run() returns, its return value becomes accessible by invoking that
//! object's get(). Other methods in this object provide means for waiting until
//! %run() returns in the other thread.
//! See the documentation for the object
//! \ref run_async::runthreadObj "referenced by the returned runthread" for
//! more information.
//!
//! This run()'s return value can be discarded, if it's not necessary to know
//! when the new thread's %run() returns, or what its return value is. The new
//! thread, in additional to keeping its own reference to the thread object,
//! has its own reference to the \c INSERT_LIBX_NAMESPACE::runthread, and
//! if no other references to its instance remain, the object gets destroyed
//! by the cleanup thread. The returned \c INSERT_LIBX_NAMESPACE::runthread
//! can be used as a mcguffin, but very carefully. The mcguffin's callbacks
//! should not be very time consumnig. If no other references to it remain,
//! its destructor callbacks get invoked in the cleanup thread, and until
//! the destructor callbacks return, the cleanup thread cannot resume joining
//! any other threads whose %run() methods have returned.

template<typename objClass, typename baseType,
	 typename ...Args>
inline auto run(//! The thread object with a run() method.
		const ref<objClass, baseType> &p,

		//! run() method's parameters.
		Args && ...args)
	-> ref<run_async::runthreadObj<decltype(p->run(invoke_run_param_helper
						       <Args>()...))> >
{
	return run_async::run_thread(p, std::forward<Args>(args)...);
}

//! Overload of run(const ref<objClass, baseType> &, Args &&) that takes a ptr argument.

//! \overload

template<typename objClass, typename baseType,
	 typename ...Args>
inline auto run(//! The thread object with a run() method.
		const ptr<objClass, baseType> &p,

		//! run() method's parameters.
		Args && ...args)
	-> ref<run_async::runthreadObj<decltype(p->run(invoke_run_param_helper
						       <Args>()...))> >
{
	return run_async::run_thread(ref<objClass, baseType>(p),
				     std::forward<Args>(args)...);
}

#if 0
{
#endif
}
#endif
