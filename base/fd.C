/*
** Copyright 2012-2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fd.H"
#include "x/fdbase.H"
#include "x/epoll.H"
#include "x/attr.H"
#include "x/uuid.H"
#include "x/sysexception.H"
#include "x/serialization.H"
#include "gettext_in.h"
#include "x/sys/eventfdsys.h"
#include <sys/mman.h>
#include <cstring>

#if HAVE_KQUEUE
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#endif

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

extern property::value<unsigned int> lockf_attempts LIBCXX_HIDDEN;

property::value<unsigned int>
lockf_attempts(LIBCXX_NAMESPACE_WSTR L"::fd::lockf::count", 1000);

fd fdBase::open(const char *filename,
		int flags,
		mode_t mode)
{
	int nfd= ::open(filename, flags | O_CLOEXEC, mode);

	if (nfd < 0)
		throw SYSEXCEPTION(filename);

	try {
		return adopt(nfd);
	} catch (...) // Shouldn't happen
	{
		::close(nfd);
		throw;
	}
}

fd fdBase::open(const std::string &filename, int flags,
		mode_t mode)
{
	return open(filename.c_str(), flags, mode);
}

fd fdBase::newobj::create(const std::string &filename, mode_t mode)
{
	std::string tmpname=filename + ".tmp";

	fd newfd=open(tmpname, O_CREAT|O_TRUNC|O_RDWR, mode);

	newfd->renameonclose(tmpname, filename);
	return newfd;
}

std::string fdBase::realpath(const std::string &pathname)
{
	std::string ret;

	char *p=::realpath(pathname.c_str(), NULL);

	if (p)
	{
		try {
			ret=p;
		} catch (...) {
			free(p);
			throw;
		}
		free(p);
	}

	return ret;
}

fdptr fdBase::lockf(const std::string &filename, int lockmode,
		    mode_t mode)
{
	struct stat s1, s2;
	unsigned int cnt=lockf_attempts.getValue();

	while (1)
	{
		if (cnt-- == 0)
			throw EXCEPTION("filename: cannot acquire a reliable lock");

		fd lockfile(open(filename, O_RDWR|O_CREAT, mode));

		if (!lockfile->lockf(lockmode))
			return fdptr();

		if (fstat(lockfile->getFd(), &s1) < 0)
			throw SYSEXCEPTION("fstat");

		if (::stat(filename.c_str(), &s2) < 0)
			continue;

		if (s1.st_dev != s2.st_dev ||
		    s1.st_ino != s2.st_ino)
			continue;

		lockfile->unlinkonclose(filename);
		return lockfile;
	}
}

fd fdBase::socket(int domain, int type, int protocol)
{
	int nfd= ::socket(domain,
			  SOCK_CLOEXEC |
			  type,
			  protocol);

	if (nfd < 0)
		throw SYSEXCEPTION("socket");

	try {
		return adopt(nfd);
	} catch (...) // Shouldn't happen
	{
		::close(nfd);
		throw;
	}
}

std::pair<fd, fd> fdBase::pipe()
{
	int pipefd[2];

	if (::pipe2(pipefd, O_CLOEXEC) < 0)
		throw SYSEXCEPTION("pipe");

	return pipesocket_common(pipefd);
}

std::pair<fd, fd> fdBase::socketpair()
{
	int pipefd[2];

	if (::socketpair(AF_UNIX,
			 SOCK_CLOEXEC |
			 SOCK_STREAM,
			 0, pipefd) < 0)
		throw SYSEXCEPTION("socketpair");

	return pipesocket_common(pipefd);
}

std::pair<fd, fd> fdBase::pipesocket_common(int *pipefd)
{
	try {
		fd pipe0(adopt(pipefd[0]));
		pipefd[0]= -1;

		fd pipe1(adopt(pipefd[1]));
		pipefd[1]= -1;

		return std::make_pair(pipe0, pipe1);
	} catch (...)
	{
		if (pipefd[0] >= 0)
			::close(pipefd[0]);

		if (pipefd[1] >= 0)
			::close(pipefd[1]);
		throw;
	}
}

fd fdBase::adopt(int filedesc)
{
	return ptrrefBase::objfactory<fd>::create(filedesc);
}

fd fdBase::shm_open(const char *filename,
		    int flags,
		    mode_t mode)
{
	int filedesc=::shm_open(filename, flags, mode);

	if (filedesc < 0)
		throw EXCEPTION("shm_open");
	try {
		return adopt(filedesc);
	} catch (...)
	{
		::close(filedesc);
		throw;
	}
}

fd fdBase::shm_open(const std::string &filename,
		    int flags,
		    mode_t mode)
{
	return shm_open(filename.c_str(), flags, mode);
}

void fdBase::shm_unlink(const std::string &filename)
{
	::shm_unlink(filename.c_str());
}

fd fdBase::tmpfile()
{
	const char *tmpdir=getenv("TMPDIR");

	if (!tmpdir)    tmpdir="/tmp";

	return tmpfile(tmpdir);
}

fd fdBase::tmpfile(const std::string &dirname)
{
	int nfd;

	while (1)
	{
		uuid::charbuf tmpfilename;

		uuid().asString(tmpfilename);

		char fn[dirname.size()+sizeof(tmpfilename)+2];

		strcat(strcat(strcpy(fn, dirname.c_str()), "/"), tmpfilename);

		nfd=::open(fn,
			   O_CREAT | O_TRUNC | O_EXCL | O_RDWR
#ifdef O_LARGEFILE
			   | O_LARGEFILE
#endif

#ifdef O_NOATIME
			   | O_NOATIME
#endif

			   | O_CLOEXEC

			   , 0600);

		if (nfd >= 0)
		{
			unlink(fn);
			break;
		}

		if (errno != EEXIST)
			throw EXCEPTION(&fn[0]);
	}

	try {
		return adopt(nfd);
	} catch (...)
	{
		::close(nfd);
		throw;
	}
}

fd fdBase::dup(const fd &ofiledesc)
{
	return dup(ofiledesc->getFd());
}

fd fdBase::dup(int ofiledesc)
{
	int nfiledesc=::open("/dev/null", O_RDWR | O_CLOEXEC);

#if HAVE_LINUXSYSCALLS
	if (nfiledesc < 0)
		nfiledesc=::eventfd(0, EFD_CLOEXEC); // chroot jail hack
#else
	if (nfiledesc < 0)
		nfiledesc=kqueue(); // chroot jail hack
#endif
	if (nfiledesc < 0)
		throw SYSEXCEPTION("eventfd");

	ofiledesc=dup3(ofiledesc, nfiledesc, O_CLOEXEC);

	if (ofiledesc < 0)
		::close(nfiledesc);

	if (ofiledesc < 0)
	{
		throw SYSEXCEPTION("dup");
	}

	try {
		return adopt(ofiledesc);
	} catch (...) // Shouldn't happen
	{
		::close(ofiledesc);
		throw;
	}
}

void fdBase::listen(std::list<fd> &fdList, int backlog)
{
	typedef std::list<std::pair<std::list<fd>::iterator, sockaddr> >
		fdaddrlist_t;

	std::list<std::pair<std::list<fd>::iterator, sockaddr> > fdaddrlist;

	for (std::list<fd>::iterator b(fdList.begin()), e(fdList.end());
	     b != e; ++b)
	{
		fdaddrlist.push_back(std::make_pair(b, (*b)->getsockname()));
	}

	for (fdaddrlist_t::iterator b(fdaddrlist.begin()), e(fdaddrlist.end());
	     b != e; ++b)
	{
		if (b->second->family() == AF_INET6)
			(*b->first)->listen(backlog);
	}

	for (fdaddrlist_t::iterator b(fdaddrlist.begin()), e(fdaddrlist.end());
	     b != e; ++b)
	{
		if (b->second->family() == AF_INET6)
			continue;

		try {
			(*b->first)->listen(backlog);
		} catch (sysexception &e)
		{
			if (e.getErrorCode() != EADDRINUSE)
				throw;

			fdaddrlist_t::iterator lb, le;
			for (lb=fdaddrlist.begin(),
				     le=fdaddrlist.end();
			     lb != le; ++lb)
			{
				if (lb->second->is46(*b->second))
					break;
			}

			if (lb != le)
			{
				fdList.erase(b->first);
				continue;
			}
			throw;
		}
	}
}

template<>
void serialization::default_traits::classcreate<fdptr>(fdptr &ptr)

{
	if (ptr.null())
		throw EXCEPTION(libmsg(_txt("Internal error: did not create file descriptor for deserialization")));
}

#if 0
{
#endif
}
