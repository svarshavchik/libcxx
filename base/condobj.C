/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/cond.H"
#include "x/mlock.H"
#include <iostream>
#include <stdlib.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

condObj::condObj()=default;

#if 0
condObj::condObj(const cond_attr &attr) : cond(attr)
{
}
#endif
condObj::~condObj()=default;

void condObj::caught_exception()
{
	std::cout << "Caught a fatal exception while waiting for a condition variable"
		  << std::endl;
	abort();
}

void condObj::wait(const mlock &m)
{
	auto &m_save=m->m;

	std::unique_lock<std::mutex> lock(m_save->objmutex);

	m_save->cond.notify_all();
	m_save->acquired=false;

	try {
		cond.wait(lock);

		while (m_save->acquired)
			m_save->cond.wait(lock);
	} catch (...) {
		caught_exception();
	}

	m_save->acquired=true;
}

void condObj::notify_one()
{
	cond.notify_one();
}

void condObj::notify_all()
{
	cond.notify_all();
}

#if 0
{
#endif
}
