/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

class timerObj::repeatinfoObj : public repeatinfo, virtual public obj {

public:

	repeatinfoObj(const property::propvalue &repeatPropertyArg,
		      const duration_t &defaultRepeatDurationArg,
		      const const_locale &localeArg)
		: repeatinfo(repeatPropertyArg,
			     defaultRepeatDurationArg,
			     localeArg)
	{
	}

	~repeatinfoObj() noexcept
	{
	}
};

class timerObj::implObj
	: public dequemsgdispatcherObj, public runthreadname{

	LOG_CLASS_SCOPE;

	std::string timername;

	std::string getName() const
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

	class LIBCXX_HIDDEN taskinfoObj : virtual public obj {
	public:
		timertask task;
		time_point_t run_time;
		ref<repeatinfoObj> repeat;

		taskinfoObj(const timertask &taskArg,
			    const time_point_t &run_timeArg,
			    const ref<repeatinfoObj> &repeatArg)
			: task(taskArg),
			  run_time(run_timeArg),
			  repeat(repeatArg) {}
		~taskinfoObj() noexcept {}
	};

	typedef ref<taskinfoObj> taskinfo;

	class LIBCXX_HIDDEN installedObj : virtual public obj {
	public:
		bool processed;

		installedObj() : processed(false)
		{
		}
	};

	implObj(const std::string &timernameArg) LIBCXX_HIDDEN;
	~implObj() noexcept LIBCXX_HIDDEN;

	void run() LIBCXX_HIDDEN;
	clock_t::time_point drain() LIBCXX_HIDDEN;
	bool runjob() LIBCXX_HIDDEN;

#include "timerobj.msgs.decl.H"

};
