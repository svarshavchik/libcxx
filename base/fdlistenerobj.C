/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fdlistenerobj.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

fdlistenerObj::fdlistenerObj(const fdlistenerImplObj::listenon &listenonArg,
			     size_t maxthreadsArg,
			     size_t minthreadsArg,
			     const std::string &jobnameArg,
			     const std::string &propnameArg
			     )
	: impl(ref<fdlistenerImplObj>::create(listenonArg,
					      maxthreadsArg, minthreadsArg,
					      jobnameArg,
					      propnameArg))
{
}

fdlistenerObj::~fdlistenerObj()
{
	stop();
	wait();
}

void fdlistenerObj::stop()
{
	impl->stop();
}

void fdlistenerObj::wait()
{
	impl->wait();
}

#if 0
{
#endif
}
