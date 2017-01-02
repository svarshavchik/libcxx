/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mmapobj_H
#define x_mmapobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/fdfwd.H>
#include <sys/mman.h>
#include <utility>
#include <x/sys/offt.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Base class for memory-mapped files or segments

//! Subclassed by the mmapObj template, which uses the protected functions.
//! Owns the memory-mapped segment, unmapped by the destructor.

class mmapbaseObj : virtual public obj {

protected:

	//! mmaped memory.
	void *addr;

	//! Its size
	size_t length;

	//! Common constructor code, mmap file descriptor
	void mminit(void *addrArg, const fd &filedesc, int prot,
		    int flags=MAP_SHARED,
		    off64_t offset=0, size_t lengthArg=0);

	//! Common constructor code, anonymous mapping
	void mminit(void *addrArg, size_t lengthArg, int prot,
		    int flags=MAP_SHARED);

	//! Common constructor code
	void mminit(void *addrArg, size_t lengthArg, int prot, int flags,
		    int fd, off64_t offset);

public:
	//! Constructor
	mmapbaseObj();

	//! Destructor
	~mmapbaseObj();

	//! msync() the shared memory segment
	void msync(int flags=MS_SYNC) const;
};

//! A memory-mapped object.

template<typename typeObj>
class mmapObj : public mmapbaseObj {
public:

	//! Constructor, first parameter is a suggested map address
	template<typename ...Args>
	mmapObj(void *addrArg, Args && ...args)
	{
		mminit(addrArg, std::forward<Args>(args)...);
	}

	//! Constructor, first parameter is not a suggested map address
	template<typename ...Args>
	mmapObj(const fd &filedesc, Args && ...args)
	{
		mminit(nullptr, filedesc, std::forward<Args>(args)...);
	}

	//! Constructor, first parameter is not a suggested map address
	template<typename ...Args>
	mmapObj(size_t lengthArg, Args && ...args)
	{
		mminit(nullptr, lengthArg, std::forward<Args>(args)...);
	}

	//! Destructor
	~mmapObj() {}

	//! Return the mmaped object.
	typeObj *object()
	{
		return reinterpret_cast<typeObj *>(addr);
	}

	//! Return the mmaped address.
	const typeObj *object() const
	{
		return reinterpret_cast<typeObj *>(addr);
	}
};

#if 0
{
#endif
}
#endif
