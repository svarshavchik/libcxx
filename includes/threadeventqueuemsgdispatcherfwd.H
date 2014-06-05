/*
** Copyright 2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_threadeventqueuemsgdispatcherfwd_H
#define x_threadeventqueuemsgdispatcherfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<typename objClass>
class threadeventqueuemsgdispatcherObj;


//! An event file descriptor queue-based message dispatcher, tied to a thread.

//! This is a version of \ref eventqueuemsgdispatcherObj "eventqueuemsgdispatcherObj"
//! where the event queue gets destroyed when the thread terminates.
//!
//! eventqueuemsgdispatcherObj's internal message queue is persistent. After
//! the thread stops it still accumulates messages, and restarting the thread
//! will pick them up.
//!
//! But sometimes its desired to destroy the message queue and all remaining
//! messages, when the thread stops.
//!
//! \code
//! class myThreadObj : public threadeventqueuethreadmsgdispatcherObj<myThread> {
//!
//! public:
//!       void run(const INSERT_LIBX_NAMESPACE::ref<INSERT_LIBX_NAMESPACE::obj> &mcguffin, const msgqueue_t &msgqueue, int additional_arguments);
//!
//! };
//! 
//! INSERT_LIBX_NAMESPACE::threadeventqueuemsgdispatcher<myThreadObj> thr=INSERT_LIBX_NAMESPACE::ref<myThreadObj>::create();
//!
//! thr->run_thread(4);
//! \endcode
//!
//! run_thread() invokes the subclass's run() method in a new thread. The first
//! parameter is a mcguffin, the second parameter is a msgqueue_t, the event
//! queue. run_thread() forwards any remaining arguments to run().
//!
//! run() ignores the mcguffin. Both the mcguffins and the event queue should
//! be kept on the thread's execution stack only, so when the thread
//! terminates they go out of scope, and get destroyed.

template<typename objClass>
using threadeventqueuemsgdispatcher
=ref<threadeventqueuemsgdispatcherObj<objClass>>;

//! A nullable pointer reference to a \ref threadeventqueuemsgdispatcher "event file descriptor queue-based message dispatcher thread".

template<typename objClass>
using threadeventqueuemsgdispatcherptr
=ptr<threadeventqueuemsgdispatcherObj<objClass>>;

#if 0
{
#endif
}
#endif
