/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_workerpool_H
#define x_workerpool_H

#include <x/threads/workerpoolfwd.H>
#include <x/threads/workerpoolobj.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A default thread worker.

//! Implements a run() method whose parameter is a reference to some class.
//! The run() call gets forwarded to the class, with any remaining parameters.
//!
//! Use this stock thread worker. Invoke a worker pool run(), passing a
//! reference to some object, together with any arguments for its run().

class simpleWorkerThreadObj {

public:

	//! Forward the run() call to some arbitrary object.

	template<typename objClass, typename baseClass, typename ...Args>
	inline void run(const ref<objClass, baseClass> &obj,
			Args && ...args)
	{
		obj->run(std::forward<Args>(args)...);
	}

	//! Forward the run() call to some arbitrary object.

	template<typename objClass, typename baseClass, typename ...Args>
	inline void run(const const_ref<objClass, baseClass> &obj,
			Args && ...args)
	{
		obj->run(std::forward<Args>(args)...);
	}
};

#if 0
{
#endif
}
#endif
