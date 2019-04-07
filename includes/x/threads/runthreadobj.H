/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_runthreadobj_H
#define x_runthreadobj_H

#include <x/namespace.h>
#include <x/ptrfwd.H>
#include <x/mpobj.H>
#include <x/threads/runfwd.H>
#include <x/threads/runthreadbaseobj.H>

#include <future>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! The return value from a run() method of an object that was invoked in another thread.

//! Use \c INSERT_LIBX_NAMESPACE::runthread<T>::ref_t to declare a reference
//! to this object (ptr_t, const_ref_t, and const_ptr_t are also available).
//! \c T is the return type from a thread object's %run() method that gets
//! started by \ref run "run()" in a new execution thread.
//!
//! wait() waits for run() to return, in the other thread, if it hasn't, yet.
//! get() returns what run() returned in the other thread. If the other
//! thread threw an exception, get() rethrows it.

template<typename ret_type>
class run_asyncthreadObj : public runthreadbaseObj {

	//! Where the thread's run() result is promised.
	std::future<ret_type> future;

	//! The shared future result, and the terminated flag.

	class meta_t {

	public:
		//! The return value from run(), for anyone who wants to know.
		std::shared_future<ret_type> shared_future;

		//! Set by joined(), to indicate that the thread has terminated.
		bool terminated;

		meta_t() : terminated(false) {}
		~meta_t() {}
	};

	//! The container for \c meta_t is a mutex-protected object, with a condition variable.

	typedef	mpcobj<meta_t> meta_container_t;

	//! A flag whether the thread has terminated, and what the return value was.

	mutable meta_container_t meta;

	//! The thread was terminated, and joined. Save its result.

	//! The cleanup thread invokes
	//! joined() after it joins the terminated thread.

	void joined() noexcept override
	{
		typename meta_container_t::lock lock(meta);

		lock->shared_future=std::move(future);
		lock->terminated=true;
		lock.notify_all();
	}

	//! Common code for get() and wait

	class getwait : public meta_container_t::lock {

	public:
		//! Constructor
		getwait(meta_container_t &metaArg)
			: meta_container_t::lock(metaArg)
		{
		}

		using meta_container_t::lock::operator*;
		using meta_container_t::lock::operator->;

		//! Wait for the thread to terminate.

		void wait()
		{
			meta_container_t::lock
				::wait( [this]
					{
						return (*this)->terminated;
					});
		}

		//! Return the terminated flag

		operator bool()
		{
			return (*this)->terminated;
		}
	};

public:

	//! Constructed by run().

	run_asyncthreadObj(std::future<ret_type> &&futureArg)
		: future(std::move(futureArg))
	{
	}

	//! Default constructor
	run_asyncthreadObj()
	{
		// A default constructor pretends that the thread has already
		// terminated.

		typename meta_container_t::lock lock(meta);

		lock->terminated=true;
	}

	//! Destructor
	~run_asyncthreadObj()=default;

	//! Return \c true if the thread has terminated, \c false otherwise.

	//! A return of \c true means that the thread which invokes the thread
	//! object's %run() method terminated and was joined.

	bool terminated() const override
	{
		typename meta_container_t::lock lock(meta);

		return lock->terminated;
	}

	//! Return the value returned by %run() in another thread.

	//! If the other thread threw an %exception, it gets rethrown here.
	//! If the thread has not yet terminated, get() waits for it.
	ret_type get() const
	{
		getwait lock(meta);

		lock.wait();

		return lock->shared_future.get();
	}

	//! Wait for the thread to terminate

	//! After returning from wait(), terminate() will be true.

	void wait() override
	{
		getwait lock(meta);

		lock.wait();
	}

	//! Wait for the thread to terminate, with a timeout

	template<typename timeout_type>
	bool wait_for(//! A \c std::chrono::duration instance.
		      const timeout_type &t)
	{
		typename meta_container_t::lock lock(meta);

		lock.wait_for(t, [&lock] { return lock->terminated; });
		return lock->terminated;
	}

	//! Wait for the thread to terminate, with a timeout

	template<typename timeout_type>
	bool wait_until(//! A \c std::chrono::time_point instance.
		      const timeout_type &t)
	{
		typename meta_container_t::lock lock(meta);

		lock.wait_until(t, [&lock] { return lock->terminated; });
		return lock->terminated;
	}

};

#ifndef DOXYGEN
//! Specialization for a run() that returns a void.

template<> void run_asyncthreadObj<void>::get() const;
#endif

extern template class run_asyncthreadObj<void>;

#if 0
{
#endif
}
#endif
