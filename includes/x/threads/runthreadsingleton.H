/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_runthreadsingleton_H
#define x_runthreadsingleton_H

#include <x/namespace.h>
#include <x/mutex.H>
#include <x/mlock.H>
#include <type_traits>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Inherit from runthreadsingleton to keep run() from starting more than one execution thread for an object

//! If an object inherits from runthreadsingleton, and run() gets invoked for
//! an object instance that's already running an execution thread started by
//! another run(), the second one throws an exception and terminates
//! immediately.

class runthreadsingleton {

	//! A mutex protecting this executing thread.

	mutex runthreadsingleton_mutex;

public:
	//! Constructor
	runthreadsingleton();

	//! Destructor
	~runthreadsingleton();

	//! Default implementation of get() is a noop
	template<typename T,
		 bool flag=std::is_base_of<runthreadsingleton, T>::value>
	class get {

	public:
		//! No-op constructor
		get(T &) {}

		//! Destructor
		~get() {}
	};

	//! Implementation of get for classes derived from runthreadsingleton

	class get_impl {

		//! The acquired lock

		mlock lock;

		//! Invoked by the constructor

		static mlock
		get_lock(runthreadsingleton &once_object) LIBCXX_HIDDEN;

	public:

		//! Constructor tries to acquire a lock
		get_impl(runthreadsingleton &once_object);

		//! Destructor
		~get_impl();
	};

	friend class get_impl;
};

//! Specialization for subclasses of runthreadsingleton

template<typename T> class runthreadsingleton::get<T, true>
	: public runthreadsingleton::get_impl {

public:

	//! Constructor tries to acquire a lock
	get(runthreadsingleton &once_object) : get_impl(once_object) {}
	~get() {}
};

#if 0
{
#endif
}
#endif
