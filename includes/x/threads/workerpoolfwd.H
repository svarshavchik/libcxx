/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_workerpoolfwd_H
#define x_workerpoolfwd_H

#include <x/ref.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class simpleWorkerThreadObj;

template<typename workerThreadType> class workerpoolObj;

//! Define a managed pool of thread workers

//! The template parameter is some reference-counted object that implements
//! run() method(s) with optional parameters.
//!
//! This template defines a reference-counted object that starts a pool of
//! threads, with a defined minimum and a maximum number.
//!
//! Invoking run() forwards the run() call to one of the available worker
//! threads. The run() calls get queued up, if all the thread workers are
//! busy executed other run() calls, and no more are left, the run() calls
//! returns immediately, and the next available thread picks up the run()
//! call, whenever it happens.
//!
//! \note
//! Typically, the template class parameter instances get instantiated in
//! advance of any run() calls. The thread pool object tries to maintain between
//! the specified maximum and minimum number of class instances of the
//! template parameter class, instantiating them and destroying them, as needed.
//!
//! \note
//! When the last reference to the worker pool object goes out of scope and
//! gets destroyed,
//! the destructor stops, and waits for all existing thread workers to
//! terminate, and returns after all thread worker class instances have been
//! destroyed. If a thread worker is in a middle of a run(), the destructor
//! waits for it to finish.

template<typename workerThreadType=simpleWorkerThreadObj>
using workerpool=ref<workerpoolObj<workerThreadType> >;

//! A nullable reference to a worker pool implementation.

//! \see workerpool

template<typename workerThreadType=simpleWorkerThreadObj>
using workerpoolptr=ptr<workerpoolObj<workerThreadType> >;

//! A reference to a constant worker pool implementation object.

//! \see workerpool

template<typename workerThreadType=simpleWorkerThreadObj>
using const_workerpool=const_ref<workerpoolObj<workerThreadType> >;

//! A nullable pointer reference to a constant worker pool implementation object.

//! \see workerpool

template<typename workerThreadType=simpleWorkerThreadObj>
using const_workerpoolptr=const_ptr<workerpoolObj<workerThreadType> >;

#if 0
{
#endif
}
#endif
