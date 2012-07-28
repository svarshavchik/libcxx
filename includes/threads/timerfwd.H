/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_timerfwd_H
#define x_timerfwd_H

#include <x/ref.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class timerObj;
class timerbaseObj;

//! Schedule a task for execution by a separate %thread

//! This object provides a facility for scheduling tasks for
//! execution in a separate execution %thread. A timer object maintains
//! an internal %thread that executes scheduled tasks.
//!
//! \par Creating a timer object
//! \code
//! timer bTimer=timer::create();
//!
//! bTimer->setTimerName("maintenance task");
//! \endcode
//!
//! \c timer is a reference to a reference-counted
//! timerObj object. After creating, it may be freely passed around.
//!
//! All tasks scheduled by the \c timer are executed in a separate execution
//! %thread. The background %thread does not get started immediately, but
//! rather when the first task gets scheduled. The %thread terminates
//! automatically when no more tasks remain to be executed.
//!
//! Timer threads have \ref sigset "all signals blocked". Timer tasks are
//! expected to be background tasks that should not respond to asynchronous
//! system signals.
//!
//! Timer threads, like other threads have a name for logging and diagnostic
//! purposes. The timer object's thread's name gets set by explicitly
//! invoking \link timerObj::setTimerName() setTimerName()\endlink.
//! This must be done before defining the timer object's first task.
//! The default timer name, unless it's explicitly set, is "unnamed timer".
//!
//! \par Defining tasks
//!
//! \code
//! class myTaskObj : public timertaskObj {
//!
//! public:
//!
//!    void run();
//! };
//!
//! typedef ptr<myTaskObj> myTask;
//!
//! // ....
//!
//! myTask bTask(myTask::create());
//! \endcode
//!
//! Tasks are defined by subclassing timertaskObj, defining its run()
//! method, then creating a reference to a new instance of the subclassed
//! object.
//!
//! The run() methods are expected to be comparatively quick. More than
//! one task can be scheduled by the same timer, but all tasks share the
//! same execution %thread.
//!
//! Other tasks scheduled by the same timer have to wait until each task's
//! run() method finishes, before the next task's run() method gets invoked.
//! However, each timer object has a separate execution %thread, and multiple
//! timer objects may be instantiated at the same time. Different timer objects
//! are independent of each other, and different timer objects can run
//! different tasks at the same time, limited to one task per timer %thread.
//!
//! \par Forwarding arguments to a timer task.
//!
//! make_timer_task() instantiates a timer task that invokes another object's
//! run() method. The first argument is a reference to a reference-counted
//! object with a run() method.
//!
//! make_timer_task() returns a reference to an object that gets passed to any
//! of the timer object's scheduling methods. The remaining arguments to
//! make_timer_task(), if any, get forwarded to the run() method when the timer
//! task gets invoked.
//!
//! \code
//!
//! class myClass : virtual public obj {
//!
//! public:
//!      void run(int a, int b);
//! };
//!
//!
//! mytask_t timerTask(mytask_t::create(someInstance));
//!
//! bTimer->schedule(INSERT_LIBX_NAMESPACE::timertask::base::make_timer_task(timerTask, 2, 4),
//!                  std::chrono::system_clock::now()+std::chrono::seconds(2));
//! \endcode
//!
//! \note
//! The reference-counted object constructed by make_timer_task() holds a
//! a reference on the object it redirects the run() method to.
//!
//! \note
//! The forwarded arguments get copied, and must be copy-constructible. The
//! forwarded run() method has an option of taking those arguments by reference,
//! and modify them. If it's a recurring task, the next call to run() receives
//! a reference to the modified arguments!
//!
//! \par Scheduling tasks
//!
//! \code
//! bTimer->schedule(bTask, std::chrono::system_clock::now()
//!     +std::chrono::seconds(2));
//! \endcode
//!
//! The \link timerObj::schedule() schedule() \endlink method schedules
//! a task to run at the given time.
//!
//! \code
//! bTimer->scheduleAfter(bTask, std::chrono::milliseconds(250),
//!                       std::chrono::milliseconds(250));
//! \endcode
//!
//! \link timerObj::scheduleAfter() scheduleAfter() \endlink is an alternative
//! method that specifies a relative starting time.
//!
//! \code
//! bTimer->scheduleAtFixedRate(bTask, std::chrono::milliseconds(250));
//! \endcode
//!
//! \link timerObj::scheduleAtFixedRate() scheduleAtFixedRate() \endlink
//! is an alternative way to schedule a task to run at regular %intervals.
//!
//! \par Specifying task repeat %interval using a property
//!
//! \code
//! bTimer->schedule(bTask, std::chrono::system_clock::now()
//!     +std::chrono::seconds(2), "maintenance.interval",
//!     std::chrono::seconds(2));
//! \endcode
//!
//! The second parameter specifies the initial starting time of the given
//! task. The third parameter specifies the name of a
//! property that gives the task's repeat %interval. The fourth parameter
//! specifies the default repeat %interval, in seconds,
//! when the property is not set.
//!
//! The property specifies the interval as "# seconds", "# minutes", or
//! "# hours". It's an
//! \ref hms "INSERT_LIBX_NAMESPACE::hms" object.
//!
//! \code
//! bTimer->scheduleAfter(bTask, std::chrono::seconds(1),
//!                       std::chrono::seconds(2),
//!                       L"maintenance.interval");
//! \endcode
//!
//! scheduleAfter() takes an optional fourth parameter that also names a
//! property that specifies the task's repeat interval. In this case,
//! the third parameter gives the default repeat interval value when the
//! property is not set.
//!
//! \note
//! While the property uses the 
//! \ref hms "INSERT_LIBX_NAMESPACE::hms" object to express the interval,
//! the parameters may be any \c std::chrono duration class instance.
//!
//! \code
//! bTimer->scheduleAtFixedRate(bTask, L"maintenance.interval",
//!                             std::chrono::seconds(2));
//! \endcode
//!
//! This version of scheduleAtFixedRate() takes a repeat interval from the
//! given property. The third parameter specifies, the default
//! repeat interval when the property is not set.
//!
//! All methods that take a property name also take one more optional parameter,
//! a locale object reference that specifies the locale for parsing the
//! time interval specified by the property.
//!
//! \par Cancelling tasks
//!
//! \code
//! bTask->cancel();
//! \endcode
//!
//! The \link timertaskObj::cancel() cancel() \endlink method stops a
//! scheduled task from further execution.
//!
//! \par Cancelling all tasks
//!
//! \code
//! bTimer->cancel()
//! \endcode
//!
//! All tasks scheduled by a timer are removed by invoking the timer object's
//! \link timerObj::cancel() cancel() \endlink method.

typedef ref<timerObj, timerbaseObj> timer;

//! A nullable pointer reference to a task execution thread.

//! \see timer

typedef ptr<timerObj, timerbaseObj> timerptr;

#if 0
{
#endif
}
#endif
