/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/threads/workerpoolobj.H"
#include "x/threads/runthreadobj.H"
#include "gettext_in.h"
#include <exception>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::workerpoolbase);

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

workerpoolbase::workerpoolbase(const std::string &prophier,
			       size_t default_minthreadcount,
			       size_t default_maxthreadcount,
			       const std::string &threadnameArg)
	: minthreadcount_prop(getpropname(prophier, "min"),
			      default_minthreadcount),
	  maxthreadcount_prop(getpropname(prophier, "max"),
			      default_maxthreadcount),
	  threadnames_prop(getpropname(prophier, "name"), threadnameArg),
	  workers(workers_t::create())
{
}

workerpoolbase::workerbaseObj::workerbaseObj(const std::string &nameArg)
	: name(nameArg)
{
#ifdef LIBCXX_DEBUG_WORKEROBJ_START
	LIBCXX_DEBUG_WORKEROBJ_START();
#endif
}

workerpoolbase::workerbaseObj::~workerbaseObj()
{
#ifdef LIBCXX_DEBUG_WORKEROBJ_STOP
	LIBCXX_DEBUG_WORKEROBJ_STOP();
#endif
}

std::string workerpoolbase::workerbaseObj::getName() const
{
	return name;
}

std::string
workerpoolbase::getpropname(const std::string &prophier,
			    const std::string &name)
{
	std::list<std::string> hier;

	property::parsepropname(prophier.begin(), prophier.end(), hier);

	if (hier.empty())
		return std::string();

	hier.push_back(name);

	return property::combinepropname(hier);
}

void workerpoolbase::captured_exception(const exception &e)
{
	LOG_ERROR(e);
	LOG_TRACE(e->backtrace);
}

void workerpoolbase::captured_unknown_exception()
{
	LOG_FATAL(_("Unknown exception caught"));
}

#if 0
{
#endif
}
