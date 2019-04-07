/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_threadlocale_H
#define x_threadlocale_H

#include <x/localeobj.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class localeObj;

//! Expose xlocale::thread_locale

class thread_locale : public xlocale::thread_locale {

public:
	//! Constructor
	thread_locale(const localeObj &obj);

	//! Destructor
	~thread_locale();
};

#if 0
{
#endif
}
#endif

