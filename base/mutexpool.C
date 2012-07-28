/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "lockpool.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template class mutexpool_traits<bool, std::less<bool>, true>;
template class mutexpool_traits<bool, std::less<bool>, false>;
template class lockpoolObj<bool, std::less<bool>,
			   mutexpool_traits<bool, std::less<bool>, true>,
			   true>;
template class lockpoolObj<bool, std::less<bool>,
			   mutexpool_traits<bool, std::less<bool>, false>,
			   false>;

#if 0
{
#endif
}
