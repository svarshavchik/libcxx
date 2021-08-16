/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/dirwalkobj.H"

#include <cerrno>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

dirwalkObj::dirwalkObj(const std::string &dirnameArg,
		       bool ispostorderArg)
	: dirname(dirnameArg), ispostorder(ispostorderArg)
{
}

dirwalkObj::~dirwalkObj()
{
}

dirwalkObj::iterator dirwalkObj::begin() const
{
	auto n=ref<dirwalktreeObj>::create(dirname, ispostorder);

	if (n->initial())
		return end();

	return iterator(n);
}

dirwalkObj::iterator dirwalkObj::end() const noexcept
{
	return iterator();
}

dirwalkObj::iterator::iterator() noexcept
{
}

dirwalkObj::iterator::~iterator()
{
}

dirwalkObj::iterator::iterator(const ptr<dirwalktreeObj> &rArg)
	: r(rArg)
{
	val=r->current();
}

dirwalkObj::iterator::iterator(const dirwalkObj::iterator &o) noexcept
	: r(o.r), val(o.val)
{
}

dirwalkObj::iterator &dirwalkObj::iterator::operator=(const dirwalkObj::iterator &o)
	noexcept
{
	r=o.r;
	val=o.val;

	return *this;
}

dirwalkObj::iterator &dirwalkObj::iterator::operator++()
{
	if (r->increment())
		r=ptr<dirwalktreeObj>();
	else
		val=r->current();

	return *this;
}

dirwalkObj::iterator dirwalkObj::iterator::operator++(int)
{
	dirwalkObj::iterator copy(*this);

	++*this;

	return copy;
}

bool dirwalkObj::iterator::operator==(const iterator &o) const noexcept
{
	return !r.null() && !o.r.null() ?
		val.fullpath() == o.val.fullpath()
		: r.null() && o.r.null();
}

#if 0
{
#endif
}
