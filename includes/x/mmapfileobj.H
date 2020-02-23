/*
** Copyright 2015-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mmapfileobj_H
#define x_mmapfileobj_H

#include <x/namespace.h>
#include <x/mmapobj.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A memory-mapped file.

//! \see mmapfile

class mmapfileObj : public mmapObj<char> {

public:

	//! Constructor

	mmapfileObj(const fd &filedesc, int prot);

	//! Destructor

	~mmapfileObj();

	//! Size of the memory mapped file.

	inline auto size() const { return length; }

	//! Pointer to the memory mapped file.

	inline auto buffer() { return object(); }

	//! Pointer to the memory mapped file.

	inline auto buffer() const { return object(); }
};

#if 0
{
#endif
}
#endif
