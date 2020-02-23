/*
** Copyright 2012-2020 Double Precision, Inc.
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

dfObj::dfObj() : res_blks{0}, res_inodes{0}
{
}

dfObj::~dfObj()=default;

class dfObj::dfvfsObj : public dfObj {

protected:
	struct statvfs s;

	uint64_t commited_free;
	uint64_t commited_inodes;

public:
	dfvfsObj();
	~dfvfsObj();

	size_t allocSize() override;
	uint64_t allocFree() override;
	uint64_t inodeFree() override;
	void commit(long, long) override;
};

dfObj::dfvfsObj::dfvfsObj()
{
}

dfObj::dfvfsObj::~dfvfsObj()
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

	res_blks.fetch_add(-s);
	res_inodes.fetch_add(-i);
}

uint64_t dfObj::dfvfsObj::allocFree()
{
	std::lock_guard<std::mutex> lock(objmutex);

	uint64_t n=s.f_bavail;

	uint64_t r=res_blks.load() + commited_free;

	return (r > n ? 0:n-r);
}

uint64_t dfObj::dfvfsObj::inodeFree()
{
	std::lock_guard<std::mutex> lock(objmutex);

	uint64_t n=s.f_favail;

	uint64_t r=res_inodes.load() + commited_inodes;

	return (r > n ? 0:n-r);
}

class dfObj::dfvfspathObj : public dfObj::dfvfsObj {

	std::string pathname;
public:

	dfvfspathObj(const std::string &pathnameArg);
	~dfvfspathObj();
	void refresh() override;
};

dfObj::dfvfspathObj::dfvfspathObj(const std::string &pathnameArg)
 : pathname(pathnameArg)
{
}

dfObj::dfvfspathObj::~dfvfspathObj()
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
	~dfvfsfdObj();
	void refresh() override;
};

dfObj::dfvfsfdObj::dfvfsfdObj(const fd &filedescArg)
 : filedesc(filedescArg)
{
}

dfObj::dfvfsfdObj::~dfvfsfdObj()
{
}

void dfObj::dfvfsfdObj::refresh()
{
	std::lock_guard<std::mutex> lock(objmutex);

	commited_free=0;
	commited_inodes=0;

	if (fstatvfs(filedesc->get_fd(), &s) < 0)
		throw EXCEPTION("fstatvfs");
}

df dfBase::newobj::create(const std::string &pathname)

{
	auto d(ptrref_base::objfactory< ref<dfObj::dfvfspathObj> >
	       ::create(pathname));

	d->refresh();

	return d;
}

df dfBase::newobj::create(const fd &filedesc)

{
	auto d(ptrref_base::objfactory< ref<dfObj::dfvfsfdObj> >
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

	r.res_blks.fetch_add(res_blks);
	r.res_inodes.fetch_add(res_inodes);
}

dfObj::reservationObj::~reservationObj()
{
	dfObj &r= *space;

	r.res_blks.fetch_add(-res_blks);
	r.res_inodes.fetch_add(-res_inodes);
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
