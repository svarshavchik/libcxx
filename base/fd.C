/*
** Copyright 2012-2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "fd.H"
#include "epoll.H"
#include "attr.H"
#include "uuid.H"
#include "sysexception.H"
#include "serialization.H"
#include "gettext_in.h"
#include <sys/eventfdsys.h>
#include <sys/mman.h>
#include <cstring>

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
#ifdef O_CLOEXEC
	int nfd= ::open(filename, flags | O_CLOEXEC, mode);
#else
	int nfd;

	{
		globlock lock;

		nfd= ::open(filename, flags, mode);

		if (nfd >= 0)
		{
			if (fcntl(nfd, F_SETFD, FD_CLOEXEC) < 0)
			{
				::close(nfd);
				nfd= -1;
			}
		}
	}
#endif

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
#ifdef SOCK_CLOEXEC
	int nfd= ::socket(domain,
			  SOCK_CLOEXEC |
			  type,
			  protocol);
#else
	int nfd;

	{
		globlock lock;

		nfd = ::socket(domain,
			       type,
			       protocol);

		if (nfd >= 0)
		{
			if (fcntl(nfd, F_SETFD, FD_CLOEXEC) < 0)
			{
				::close(nfd);
				nfd=-1;
			}
		}
	}
#endif

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

#if HAVE_PIPE2
	if (::pipe2(pipefd, O_CLOEXEC) < 0)
		throw SYSEXCEPTION("pipe");
#else

	{
		globlock lock;

		if (::pipe(pipefd) < 0)
			throw SYSEXCEPTION("pipe");

		if (fcntl(pipefd[0], F_SETFD, FD_CLOEXEC) < 0 ||
		    fcntl(pipefd[1], F_SETFD, FD_CLOEXEC) < 0)
		{
			::close(pipefd[0]);
			::close(pipefd[1]);
			throw SYSEXCEPTION("fcntl");
		}
	}
#endif
	return pipesocket_common(pipefd);
}

std::pair<fd, fd> fdBase::socketpair()
{
	int pipefd[2];

#ifdef SOCK_CLOEXEC
	if (::socketpair(AF_UNIX,
			 SOCK_CLOEXEC |
			 SOCK_STREAM,
			 0, pipefd) < 0)
		throw SYSEXCEPTION("socketpair");
#else

	{
		globlock lock;

		if (::socketpair(AF_UNIX,
				 SOCK_STREAM,
				 0, pipefd) < 0)
			throw SYSEXCEPTION("socketpair");

		if (fcntl(pipefd[0], F_SETFD, FD_CLOEXEC) < 0 ||
		    fcntl(pipefd[1], F_SETFD, FD_CLOEXEC) < 0)
		{
			::close(pipefd[0]);
			::close(pipefd[1]);
			throw SYSEXCEPTION("fcntl");
		}
	}
#endif

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

#ifndef O_CLOEXEC
		globlock lock;
#endif

		nfd=::open(fn,
			   O_CREAT | O_TRUNC | O_EXCL | O_RDWR
#ifdef O_LARGEFILE
			   | O_LARGEFILE
#endif

#ifdef O_NOATIME
			   | O_NOATIME
#endif

#ifdef O_CLOEXEC
			   | O_CLOEXEC
#endif
			   , 0600);

		if (nfd >= 0)
		{
			unlink(fn);
#ifndef O_CLOEXEC
			if (fcntl(nfd, F_SETFD, FD_CLOEXEC) < 0)
			{
				::close(nfd);
				throw SYSEXCEPTION("fcntl");
			}
#endif
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
#if HAVE_DUP3
	int nfiledesc=::open("/dev/null", O_RDWR | O_CLOEXEC);

	if (nfiledesc < 0)
		nfiledesc=::eventfd(0, EFD_CLOEXEC); // chroot jail hack

	if (nfiledesc < 0)
		throw SYSEXCEPTION("eventfd");

	ofiledesc=dup3(ofiledesc, nfiledesc, O_CLOEXEC);

	if (ofiledesc < 0)
		::close(nfiledesc);

#else

	{
		globlock lock;

		ofiledesc=::dup(ofiledesc);

		if (ofiledesc >= 0)
		{
			if (fcntl(ofiledesc, F_SETFD, FD_CLOEXEC) < 0)
			{
				::close(ofiledesc);
				ofiledesc= -1;
			}
		}
	}
#endif
	
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
