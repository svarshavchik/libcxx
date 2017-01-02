/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/lockpool.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

rwpool_traits::container_t::container_t() noexcept
	: readlockcnt(0), writelockflag(false)
{
}

rwpool_traits::container_t::~container_t()
{
}

bool rwpool_traits::insert_lock(const lock_t &v,
				container_t &s) noexcept
{
	if (v == readlock)
	{
		if (s.writelockflag)
			return false;
		++s.readlockcnt;
		return true;
	}

	if (s.writelockflag || s.readlockcnt > 0)
		return false;

	s.writelockflag=true;

	return true;
}

void rwpool_traits::delete_lock(const lock_t &v,
				container_t &s) noexcept
{
	if (v == readlock)
		--s.readlockcnt;
	else
		s.writelockflag=false;
}

template class lockpoolObj<rwpool_traits::lock_t,
			   std::less<rwpool_traits::lock_t>,
			   rwpool_traits, true>;

template class lockpoolObj<rwpool_traits::lock_t,
			   std::less<rwpool_traits::lock_t>,
			   rwpool_traits, false>;

#if 0
{
#endif
}
