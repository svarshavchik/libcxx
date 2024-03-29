/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_condobj_H
#define x_condobj_H


#include <time.h>
#include <x/obj.H>
#include <x/mutexobj.H>
#include <x/mlockfwd.H>
#include <x/mlockobj.H>
#include <x/exceptionfwd.H>
#include <x/namespace.h>

#include <condition_variable>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A reference-counted object with condition variable-like semantics.

class condObj : virtual public obj {

	//! The underlying condition variable.
	std::condition_variable cond;

	//! Fatal exception.
	void caught_exception() __attribute__((noreturn));

public:

	//! Create a new condition variable
	condObj();

	//! Destroy the condition variable
	~condObj();

	//! Wait on the condition variable
	void wait(//! An existing mutex lock
		  const mlock &m);

	//! Wait on a condition variable, with a timeout
	template<typename timeout_type>
	bool wait_until(//! An existing mutex lock
			const mlock &m,

			//! Timeout.
			const timeout_type &abstime)
	{
		auto &m_save=m->m;

		std::unique_lock<std::mutex> lock(m_save->objmutex);

		m_save->cond.notify_all();
		m_save->acquired=false;

		bool flag;

		try {
			flag=cond.wait_until(lock, abstime)
				== std::cv_status::no_timeout;

			while (m_save->acquired)
				m_save->cond.wait(lock);
		} catch (...) {
			caught_exception();
		}
		m_save->acquired=true;
		return flag;
	}

	//! Signal the condition variable.
	void notify_one();

	//! Broadcast the condition variable.
	void notify_all();

};

#if 0
{
#endif
}
#endif
