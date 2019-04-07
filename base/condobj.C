/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/condobj.H"
#include <iostream>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

condObj::condObj()
{
}
#if 0
condObj::condObj(const cond_attr &attr) : cond(attr)
{
}
#endif
condObj::~condObj()
{
}

void condObj::wait(const mutex::base::mlock &m)
{
	mutex m_save(m->m);

	std::unique_lock<std::mutex> lock(m_save->objmutex);

	m_save->cond.notify_all();
	m_save->acquired=false;

	try {
		cond.wait(lock);

		while (m_save->acquired)
			m_save->cond.wait(lock);
	} catch (...) {
		m_save->acquired=true;
		throw;
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
