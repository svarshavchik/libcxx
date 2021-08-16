/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/


class timertaskentryObj : virtual public obj {

public:
	timerObj::implObj::jobs_t::iterator jobentry;
	bool installed;
	timertask task;
	ref<timerObj::repeatinfoObj>     repeat;
	weakptr<ptr<timerObj::implObj> > impl;

	timertaskentryObj(const timertask &taskArg,
			  const ref<timerObj::repeatinfoObj> &repeatArg)
		LIBCXX_HIDDEN
		: installed(false), task(taskArg), repeat(repeatArg)
	{
	}
	
	~timertaskentryObj() LIBCXX_HIDDEN
	{
	}
};
