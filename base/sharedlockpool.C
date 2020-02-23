/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/lockpool.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

sharedpool_traits::container_t::container_t() noexcept
	: sharedlockcnt(0), uniquelockflag(false)
{
}

sharedpool_traits::container_t::~container_t()
{
}

bool sharedpool_traits::insert_lock(const lock_t &v,
				container_t &s) noexcept
{
	if (v == sharedlock)
	{
		if (s.uniquelockflag)
			return false;
		++s.sharedlockcnt;
		return true;
	}

	if (s.uniquelockflag || s.sharedlockcnt > 0)
		return false;

	s.uniquelockflag=true;

	return true;
}

void sharedpool_traits::delete_lock(const lock_t &v,
				container_t &s) noexcept
{
	if (v == sharedlock)
		--s.sharedlockcnt;
	else
		s.uniquelockflag=false;
}

template class lockpoolObj<sharedpool_traits::lock_t,
			   std::less<sharedpool_traits::lock_t>,
			   sharedpool_traits, true>;

template class lockpoolObj<sharedpool_traits::lock_t,
			   std::less<sharedpool_traits::lock_t>,
			   sharedpool_traits, false>;

#if 0
{
#endif
}
