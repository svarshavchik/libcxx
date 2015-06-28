/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_timerobj_H
#define x_timerobj_H

#include <x/namespace.h>
#include <x/threads/timerfwd.H>
#include <x/threads/timertaskfwd.H>
#include <x/threads/run.H>
#include <x/hms.H>
#include <x/weakptr.H>
#include <x/property_value.H>
#include <x/mpobj.H>
#include <map>
#include <deque>
#include <chrono>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class timerbaseObj;

//! A timer implementation

//! The timerObj instance maintains a list of scheduled timer tasks
//! and runs them, when their time comes up.
//!

class timerObj : virtual public obj {

#include <x/sys/clockt.h>

public:
	//! The clock the timer thread runs off

	typedef sys_clock_t clock_t;

	//! The clock's ticker

	typedef clock_t::time_point time_point_t;

	//! The clock's granularity

	typedef clock_t::duration duration_t;

private:
	//! The timer's name, for logging purposes.

	std::string timername;

	//! Specify the periodic time using a property, with a default value

	//! \internal

	class repeatinfo : public notcopyableObj {

	public:

		//! Property that specifies the repeat interval

		property::value<hms> repeatProperty;

		//! If the property is not set, the default value.
		duration_t defaultRepeatDuration;

		//! Constructor
		repeatinfo(//! Property that specifies the repeat interval
			   const std::string &repeatPropertyArg,
			   //! If the property is not set, the default value.
			   const duration_t &defaultRepeatDurationArg,
			   //! The locale the property is parsed with
			   const const_locale &localeArg);

		//! Constructor
		repeatinfo(//! Property that specifies the repeat interval
			   const std::string &repeatPropertyArg,
			   //! If the property is not set, the default value.
			   const duration_t &defaultRepeatDurationArg);

		//! Destructor
		~repeatinfo() noexcept;

		//! Retrieve the current repeat interval
		duration_t getDuration()
		{
			time_t n=repeatProperty.getValue().seconds();

			return n == 0 ? defaultRepeatDuration
				: std::chrono::duration_cast<duration_t>
				(std::chrono::seconds(n));
		}
	};

	//! All ways of scheduling a new task eventually wind up here.

	//! \internal

	void do_schedule(const timertask &task,
			 const time_point_t &run_time,
			 const std::string &repeatProperty,
			 const duration_t &default_duration,
			 const const_locale &locale);

public:

	class repeatinfoObj;
	class implObj;

	friend class timerbaseObj;

	//! The default constructor
	timerObj();

	//! The default destructor
	~timerObj() noexcept;

	//! Schedule a task to run at a given time

	//! The task given by the first parameter gets scheduled to run
	//! at the time specified by the second parameter.

	template<typename Clock, typename Duration>
	void schedule(//! The task to start
		      const timertask &task,

		      //! Run at the given system time
		      const std::chrono::time_point<Clock, Duration>
		      &start_time)
	{
		schedule(task, start_time, Duration::zero());
	}

	//! Schedule a task to run at a given time, and then repeat.

	//! The task given by the first parameter gets scheduled to run
	//! at the time specified by the second parameter, then every period
	//! specified by the third parameter.

	template<typename Clock, typename Duration> void
	schedule(//! The task to start
		 const timertask &task,

		 //! Run at the given system time
		 const std::chrono::time_point<Clock, Duration> &start_time,

		 //! Repeat the task every so often
		 const Duration &repeat_time)
	{
		schedule(task, start_time, std::string(),
			 repeat_time);
	}

	//! Schedule a task to run at a given time

	//! \overload
	//!

	template<typename Clock, typename Duration> void
	schedule(//! The task to start
		 const timertask &task,

		 //! Run at the given system time
		 const std::chrono::time_point<Clock, Duration> &start_time,

		 //! Repeat the task every so often
		 const std::string &repeatProperty,

		 //! Default value if the property is not set
		 const Duration &default_repeat_time,

		 //! Locale for parsing the repeat property
		 const const_locale &locale=locale::base::global())
	{
		typename Clock::time_point given_now=Clock::now();

		do_schedule(task,
			    clock_t::now() +
			    std::chrono::duration_cast<duration_t>
			    ((given_now >= start_time ? Duration::zero()
			      : start_time - given_now)),
			    repeatProperty,
			    std::chrono::duration_cast<duration_t>
			    (default_repeat_time), locale);
	}

	//! Schedule a task to run after a delay

	//! This is similar to schedule(),
	//! except that the second parameter gives a relative time
	//! interval, relative to the current system clock's time.
	template<typename duration_type> void
	scheduleAfter(//! The task to start
		      const timertask &task,

		      //! Run the task after this delay

		      //! This parameter specifies the initial delay in
		      //! running the task.

		      const duration_type &start_time,

		      //! Repeat the task every so often

		      const duration_type &repeat_time
		      =duration_type::zero(),

		      //! Instead of specifying repeat_time, take it from a property

		      const std::string &repeatProperty=
		      std::string(),

		      //! Locale for parsing the repeat property
		      const const_locale &locale=locale::base::global())
	{
		schedule(task,
			 clock_t::now() +
			 std::chrono::duration_cast<duration_t>(start_time),
			 repeatProperty,
			 std::chrono::duration_cast<duration_t>(repeat_time),
			 locale);
	}

	//! Schedule the task to run repeatedly

	//! This method is a convenient
	//! shortcut for setting a task to run repeatedly at fixed intervals.
	//! The first execution occurs after the specified time interval
	//! has elapsed.
	//!
	//! Where possible, the \c timer object tries to schedule the task's
	//! run times at intervals specified by its repeat interval.
	//! This is possible, of course, only if the task's run() method
	//! finishes before it gets scheduled to run again, and any other
	//! tasks scheduled by the same timer also complete their work without
	//! too much delay. If that is not possible, the timer object will
	//! only guarantee that repeat interval will be the
	//! minimum delay before successive task executions.

	template<typename duration_type>
	void scheduleAtFixedRate(//! The task to start
				 const timertask &task,

				 //! Repeat the task every so often
				 const duration_type &repeat_time)
	{
		scheduleAfter(task, repeat_time, repeat_time);
	}

	//! Schedule the task to run repeatedly

	//! \overload

	template<typename duration_type>
	void scheduleAtFixedRate(//! The task to start
				 const timertask &task,

				 //! Repeat the task every so often
				 const std::string &repeatProperty,

				 //! Default value if the property is not set
				 const duration_type &default_repeat_time,

				 //! Locale for parsing the repeat property
				 const const_locale &locale=
				 locale::base::global())
	{
		scheduleAfter(task,
			      repeatinfo(repeatProperty,
					 std::chrono::duration_cast
					 <duration_t>
					 (default_repeat_time),
					 locale).getDuration(),
			      std::chrono::duration_cast<duration_t>
			      (default_repeat_time),
			      repeatProperty, locale);
	}

	//! Cancel all tasks

	//! Remove all tasks scheduled by this timer.
	//!
	//! This is equivalent to invoking all scheduled tasks'
	//! \link timertaskObj::cancel() cancel() \endlink function.
	//! This function returns after all tasks are cancelled. This does not
	//! prevent any new tasks from being scheduled.
	//!
	//! cancel() gets called by the destructor.

	void cancel();

	//! Set this timer's name

	//! This must be done before starting the timer, for this name to
	//! take effect in the logs.

	void setTimerName(const std::string &nameArg) noexcept;

private:

	//! The internal thread that implements the timer functionality.

	//! \internal
	//! The timer thread gets started on demand, when the first task
	//! gets scheduled. When the thread has nothing to do, it stops.

	typedef weakptr<ptr<implObj> > thread_t;

	//! Thread metadata

	class meta_t {

	public:
		//! The current timer thread

		thread_t thread;

		//! The most recently-started thread's return value
		runthreadptr<void> thread_ret;
	};
public:
	//! The mutex-protected metadata container

	typedef mpobj<meta_t> meta_container_t;
private:
	//! The thread metadata
	meta_container_t meta_container;
};

#if 0
{
#endif
}
#endif
