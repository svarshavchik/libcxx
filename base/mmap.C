/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/mmap.H"
#include "x/fd.H"
#include "x/filestat.H"
#include "x/logger.H"
#include "x/sysexception.H"
#include <errno.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

void mmapbaseObj::mminit(void *addrArg, const fd &filedesc, int prot, int flags,
			 off64_t offset, size_t lengthArg)
{
	if (lengthArg == 0)
	{
		auto l=filedesc->stat()->st_size;

		if ((decltype(l))(lengthArg=l) != l)
		{
			errno=EFBIG;
			throw EXCEPTION("mmap");
		}
	}
	mminit(addrArg, lengthArg, prot, flags, filedesc->getFd(), offset);
}

void mmapbaseObj::mminit(void *addrArg, size_t lengthArg, int prot, int flags)
{
	mminit(addrArg, lengthArg, prot, flags, -1, 0);
}

void mmapbaseObj::mminit(void *addrArg, size_t lengthArg, int prot, int flags,
			 int fd, off64_t offset)
{
#if HAVE_MMAP64
	addr=::mmap64(addrArg, lengthArg, prot, flags, fd, offset);
#else
	if ( (off64_t)(off_t)offset != offset)
	{
		addr=NULL;
		errno=EFBIG;
	}
	else
	{
		addr=::mmap(addrArg, lengthArg, prot, flags, fd, offset);
	}
#endif

	if (addr == MAP_FAILED)
		throw SYSEXCEPTION("mmap");
	length=lengthArg;
}

mmapbaseObj::mmapbaseObj() : addr(MAP_FAILED), length(0)
{
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::mmapbaseObj::~mmapbaseObj,
		    destructor_logger);

mmapbaseObj::~mmapbaseObj() noexcept
{
	LOG_FUNC_SCOPE(destructor_logger);

	if (addr != MAP_FAILED)
	{
		if (munmap(addr, length) < 0)
			LOG_ERROR("munmap: " << strerror(errno));
	}
}

void mmapbaseObj::msync(int flags) const
{
	if (::msync(addr, length, flags) < 0)
		throw SYSEXCEPTION("msync");
}

#if 0
{
#endif
}

