/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

class timerObj::repeatinfoObj : public repeatinfo, virtual public obj {

public:

	repeatinfoObj(const std::string &repeatPropertyArg,
		      const duration_t &defaultRepeatDurationArg,
		      const const_locale &localeArg) LIBCXX_HIDDEN
		: repeatinfo(repeatPropertyArg,
			     defaultRepeatDurationArg,
			     localeArg)
	{
	}

	~repeatinfoObj() LIBCXX_HIDDEN
	{
	}
};

class timerObj::implObj
	: public threadmsgdispatcherObj, public runthreadname{

	LOG_CLASS_SCOPE;

	std::string timername;

	std::string getName() const override
	{
		return timername;
	}

	std::thread::id tid;


public:
	typedef std::multimap<time_point_t, timertaskentry> jobs_t;

	jobs_t jobs;

	bool samethread()
	{
		std::thread::id id=std::this_thread::get_id();

		if (id == std::thread::id())
			return false; // Main execution thread

		std::lock_guard<std::mutex> lock(objmutex);

		return tid == id;
	}

	class taskinfoObj : virtual public obj {
	public:
		timertask task;
		time_point_t run_time;
		ref<repeatinfoObj> repeat;

		taskinfoObj(const timertask &taskArg,
			    const time_point_t &run_timeArg,
			    const ref<repeatinfoObj> &repeatArg) LIBCXX_HIDDEN
			: task(taskArg),
			  run_time(run_timeArg),
			  repeat(repeatArg) {}
		~taskinfoObj() LIBCXX_HIDDEN=default;
	};

	typedef ref<taskinfoObj> taskinfo;

	class installedObj : virtual public obj {
	public:
		bool processed;

		installedObj() LIBCXX_HIDDEN : processed(false)
		{
		}

		~installedObj() LIBCXX_HIDDEN =default;
	};

	implObj(const std::string &timernameArg) LIBCXX_HIDDEN;
	~implObj() LIBCXX_HIDDEN;

	void run(x::ptr<x::obj> &threadmsgdispatcher_mcguffin,
		 msgqueue_obj &msgqueue) LIBCXX_HIDDEN;
	clock_t::time_point drain() LIBCXX_HIDDEN;
	bool runjob() LIBCXX_HIDDEN;

#include "timerobj.msgs.H"

public:

	void canceltask(const timertaskentryptr &taskentry_arg,
			const ref<obj> &mcguffin_arg);
};
