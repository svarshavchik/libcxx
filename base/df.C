/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/df.H"
#include "x/fd.H"
#include "x/exception.H"

#include <sys/statvfs.h>
#include <unistd.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

dfObj::dfObj()
{
}

dfObj::~dfObj() noexcept
{
}

class dfObj::dfvfsObj : public dfObj {

protected:
	struct statvfs s;

	uint64_t commited_free;
	uint64_t commited_inodes;

public:
	dfvfsObj();
	~dfvfsObj() noexcept;

	size_t allocSize();
	uint64_t allocFree();
	uint64_t inodeFree();
	void commit(long, long);
};

dfObj::dfvfsObj::dfvfsObj()
{
}

dfObj::dfvfsObj::~dfvfsObj() noexcept
{
}

size_t dfObj::dfvfsObj::allocSize()
{
	std::lock_guard<std::mutex> lock(objmutex);

	return s.f_frsize;
}

void dfObj::dfvfsObj::commit(long s, long i)
{
	std::lock_guard<std::mutex> lock(objmutex);

	commited_free += s;
	commited_inodes += i;

	res_blks.refadd(-s);
	res_inodes.refadd(-i);
}

uint64_t dfObj::dfvfsObj::allocFree()
{
	std::lock_guard<std::mutex> lock(objmutex);

	uint64_t n=s.f_bavail;

	uint64_t r=res_blks.refget() + commited_free;

	return (r > n ? 0:n-r);
}

uint64_t dfObj::dfvfsObj::inodeFree()
{
	std::lock_guard<std::mutex> lock(objmutex);

	uint64_t n=s.f_favail;

	uint64_t r=res_inodes.refget() + commited_inodes;

	return (r > n ? 0:n-r);
}

class dfObj::dfvfspathObj : public dfObj::dfvfsObj {

	std::string pathname;
public:

	dfvfspathObj(const std::string &pathnameArg);
	~dfvfspathObj() noexcept;
	void refresh();
};

dfObj::dfvfspathObj::dfvfspathObj(const std::string &pathnameArg)
 : pathname(pathnameArg)
{
}

dfObj::dfvfspathObj::~dfvfspathObj() noexcept
{
}

void dfObj::dfvfspathObj::refresh()
{
	std::lock_guard<std::mutex> lock(objmutex);

	commited_free=0;
	commited_inodes=0;

	if (statvfs(pathname.c_str(), &s) < 0)
		throw EXCEPTION(pathname);
}

class dfObj::dfvfsfdObj : public dfObj::dfvfsObj {

	fd filedesc;
public:

	dfvfsfdObj(const fd &filedescArg);
	~dfvfsfdObj() noexcept;
	void refresh();
};

dfObj::dfvfsfdObj::dfvfsfdObj(const fd &filedescArg)
 : filedesc(filedescArg)
{
}

dfObj::dfvfsfdObj::~dfvfsfdObj() noexcept
{
}

void dfObj::dfvfsfdObj::refresh()
{
	std::lock_guard<std::mutex> lock(objmutex);

	commited_free=0;
	commited_inodes=0;

	if (fstatvfs(filedesc->getFd(), &s) < 0)
		throw EXCEPTION("fstatvfs");
}

df dfBase::newobj::create(const std::string &pathname)

{
	auto d(ptrrefBase::objfactory< ref<dfObj::dfvfspathObj> >
	       ::create(pathname));

	d->refresh();

	return d;
}

df dfBase::newobj::create(const fd &filedesc)

{
	auto d(ptrrefBase::objfactory< ref<dfObj::dfvfsfdObj> >
	       ::create(filedesc));

	d->refresh();

	return d;
}

dfObj::reservationObj::reservationObj(long res_blksArg,
				      long res_inodesArg,
				      const df &spaceArg)
	: res_blks(res_blksArg),
	  res_inodes(res_inodesArg),
	  space(spaceArg)
{
	dfObj &r= *space;

	r.res_blks.refadd(res_blks);
	r.res_inodes.refadd(res_inodes);
}

dfObj::reservationObj::~reservationObj() noexcept
{
	dfObj &r= *space;

	r.res_blks.refadd(-res_blks);
	r.res_inodes.refadd(-res_inodes);
}

df::base::reservation dfObj::reserve(long alloc,
			       long inodes)
{
	return df::base::reservation::create(alloc, inodes, df(this));
}

#if 0
{
#endif
}
