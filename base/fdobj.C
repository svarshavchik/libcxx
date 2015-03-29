/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fdobj.H"
#include "x/fd.H"
#include "x/fdbase.H"
#include "x/filestat.H"
#include "x/sockaddr.H"
#include "x/fdstreambufobj.H"
#include "x/epoll.H"
#include "x/sysexception.H"
#include "x/sockaddr.H"
#include "x/uuid.H"
#include "x/property_value.H"
#include "x/timespec.H"
#include "x/fileattr.H"
#include "x/netaddr.H"
#include "x/globlock.H"
#include "x/threads/timer.H"
#include "x/threads/timertask.H"

#include "x/weakptr.H"
#include <cstdio>
#include <poll.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#if HAVE_XATTR
#include <sys/xattr.h>
#endif
#if HAVE_EXTATTR
#include <sys/param.h>
#include <sys/extattr.h>
#endif

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

property::value<size_t> fdbaseObj::buffer_size(LIBCXX_NAMESPACE_WSTR
					       L"::fd::buffer_size",
					       BUFSIZ);

void fdbaseObj::write_full(const char *buffer,
			   size_t cnt)
{
	while (cnt)
	{
		errno=ENOSPC;
		size_t n=pubwrite(buffer, cnt);

		if (n == 0)
			throw SYSEXCEPTION("write");

		buffer += cnt;
		cnt -= n;
	}
}

void fdbaseObj::write(const fd &otherFile)
{
	filestat st(otherFile->stat());

	if (S_ISREG(st->st_mode))
	{
		off64_t n=st->st_size;

		if (n == st->st_size) // Unlikely overflow
		{
			write(otherFile, 0, n);
			return;
		}
	}

	size_t n=get_buffer_size();

	char buf[n];
	size_t i;

	while ((i=otherFile->read(&buf[0], n)) > 0)
		write_full(&buf[0], i);
}

void fdbaseObj::write(const fd &otherFile,
		      off64_t startpos,
		      off64_t cnt)
{
	size_t n=get_buffer_size();

	char buf[n];

	while (cnt > 0)
	{
		size_t i=n;

		if ((off64_t)i > cnt)
			i=cnt;

		i=otherFile->pread(startpos, &buf[0], i);

		if (i == 0)
			throw EXCEPTION("end of file while copying file contents");

		write_full(&buf[0], i);

		startpos += i;
		cnt -= i;
	}
}

std::string fdBase::cwd()
{
	std::vector<char> buffer;

	buffer.resize(256);

	while (getcwd(&buffer[0], buffer.size()) == NULL)
	{
		if (errno != ERANGE)
			throw SYSEXCEPTION("getcwd");

		buffer.resize(buffer.size()*2);
	}

	return &buffer[0];
}

fdbaseObj::adapterObj::adapterObj(const fdbase &ptrArg)
	: ptr(ptrArg)
{
}

fdbaseObj::adapterObj::~adapterObj() noexcept
{
}

int fdbaseObj::adapterObj::getFd() const noexcept
{
	return ptr->getFd();
}

size_t fdbaseObj::adapterObj::pubread(char *buffer,
				      size_t cnt)

{
	return ptr->pubread(buffer, cnt);
}

size_t fdbaseObj::adapterObj::pubread_pending()
	const
{
	return ptr->pubread_pending();
}

size_t fdbaseObj::adapterObj::pubwrite(const char *buffer,
				       size_t cnt)
{
	return ptr->pubwrite(buffer, cnt);
}

off64_t fdbaseObj::adapterObj::pubseek(off64_t offset,
				       int whence)
{
	return ptr->pubseek(offset, whence);
}

void fdbaseObj::adapterObj::pubconnect(const struct ::sockaddr *serv_addr,
				       socklen_t addrlen)
{
	return ptr->pubconnect(serv_addr, addrlen);
}

fdptr fdbaseObj::adapterObj::pubaccept(//! Connected peer's address
				       sockaddrptr &peername)
{
	return ptr->pubaccept(peername);
}

void fdbaseObj::adapterObj::pubclose()
{
	ptr->pubclose();
}

void fdbaseObj::badconnect(const struct ::sockaddr *serv_addr,
			   socklen_t addrlen)
{
	std::string s("unknown");

	sockaddr sabuf=sockaddr::create();

	sabuf->resize(addrlen);

	memcpy(&*sabuf->begin(), (const char *)serv_addr, addrlen);

	try {
		s= *sabuf;
	} catch (...) {

	}

	throw SYSEXCEPTION("connect(" + s + ")");
}

size_t fdbaseObj::get_buffer_size()
{
	return buffer_size.getValue();
}

ref<fdstreambufObj> fdbaseObj::getStreamBuffer(size_t bufsiz)
{
	size_t filedesc=getFd();

	struct stat stat_buf;

	if (fstat(filedesc, &stat_buf) < 0)
		throw SYSEXCEPTION("fstat");

	return ref<fdstreambufObj>::create(fdbase(this), bufsiz,
					   S_ISREG(stat_buf.st_mode));
}

fdptr fdbaseObj::pubaccept()
{
	sockaddrptr ignore;

	return pubaccept(ignore);
}

istream fdbaseObj::getistream()
{
	return getStream<istream>();
}

ostream fdbaseObj::getostream()
{
	return getStream<ostream>();
}

iostream fdbaseObj::getiostream()
{
	return getStream<iostream>();
}

#pragma GCC visibility push(internal)

class fdObj::closeactionObj : virtual public obj {

 public:
	closeactionObj() {}
	~closeactionObj() noexcept {}

	virtual void beforeclose() {}
	virtual void cancel() {}
	virtual void afterclose() {}

	class rename;
	class lockunlink;
};

class fdObj::closeactionObj::rename : public closeactionObj {

 public:

	std::string renamefrom;
	std::string renameto;
	pid_t p;

	rename(const std::string &renamefromArg,
	       const std::string &renametoArg)
		: renamefrom(renamefromArg), renameto(renametoArg), p(getpid())
	{
	}

	~rename() noexcept
	{
	}

	void afterclose()
	{
		if (p == getpid() &&
		    ::rename(renamefrom.c_str(), renameto.c_str()) < 0)
			throw SYSEXCEPTION("rename(\"" + renamefrom
					   + "\", \"" + renameto + "\")");
	}

	void cancel()
	{
		if (p == getpid())
			unlink(renamefrom.c_str());
	}
};

class fdObj::closeactionObj::lockunlink : public closeactionObj {

 public:

	std::string lockname;

	lockunlink(const std::string &locknameArg)
		: lockname(locknameArg)
	{
	}

	~lockunlink() noexcept
	{
	}

	void beforeclose()
	{
		unlink(lockname.c_str());
	}
};
#pragma GCC visibility pop

void fdObj::clearcloseaction()
{
	std::lock_guard<std::mutex> lock(objmutex);

	closeaction=ptr<closeactionObj>();
}

void fdObj::init() noexcept
{
	filedesc=-1;
	sigpipeFlag=false;
}

fdObj::~fdObj() noexcept
{
	try {
		close();
	} catch (const exception &e)
	{
	}
}

void fdObj::renameonclose(const std::string &tmpname,
			  const std::string &filename)
{
	auto hook=ptr<closeactionObj::rename>::create(tmpname, filename);

	std::lock_guard<std::mutex> lock(objmutex);

	closeaction=hook;
}

void fdObj::unlinkonclose(const std::string &filename)
{
	auto hook=ptr<closeactionObj::lockunlink>::create(filename);

	std::lock_guard<std::mutex> lock(objmutex);
	closeaction=hook;
}

int fdObj::getFd() const noexcept
{
	return filedesc;
}

void fdObj::futimens(const timespec &atime,
		     const timespec &mtime)
{
#if HAVE_FUTIMENS
	::timespec ts[2];

	ts[0]=atime;
	ts[1]=mtime;

	if (::futimens(filedesc, ts) < 0)
		throw SYSEXCEPTION("futimens");
#else
	struct ::timeval tv[2];

	tv[0].tv_sec=atime.tv_sec;
	tv[0].tv_usec=atime.tv_nsec / 1000;

	tv[1].tv_sec=mtime.tv_sec;
	tv[1].tv_usec=mtime.tv_nsec / 1000;

	if (::futimes(filedesc, tv) < 0)
		throw SYSEXCEPTION("futimes");
#endif
}

void fdObj::futimens(const timespec &atime)
{
	futimens(atime, atime);
}

void fdObj::futimens()
{
	futimens(timespec::getclock());
}

fdptr fdObj::pubaccept(sockaddrptr &peername)
{
	return accept(peername);
}

void fdObj::pubconnect(const struct ::sockaddr *serv_addr,
		       socklen_t addrlen)
{
#ifdef LIBCXX_DEBUG_CONNECT
	LIBCXX_DEBUG_CONNECT();
#endif

	connect(serv_addr, addrlen);
}

void fdObj::reuseaddr(bool flag)
{
	int iflag=flag ? 1:0;

	if (setsockopt(filedesc, SOL_SOCKET, SO_REUSEADDR, &iflag,
		       sizeof(iflag)) < 0)
		throw SYSEXCEPTION("setsockopt(SOL_SOCKET, SO_REUSEADDR)");
}

sockaddr fdObj::getsockname() const
{
	return fd::base::getsockname(filedesc);
}

sockaddr fdBase::getsockname(int filedesc)
{
	struct sockaddr_storage ss;
	socklen_t ss_len=sizeof(ss);

	if (::getsockname(filedesc, (struct ::sockaddr *)&ss, &ss_len) < 0)
		throw SYSEXCEPTION("getsockname");

	sockaddr sa=sockaddr::create();

	sa->resize(ss_len);

	std::copy((const char *)&ss,
		  (const char *)&ss+ss_len, sa->begin());
	return sa;
}

sockaddr fdObj::getpeername() const
{
	return fd::base::getpeername(filedesc);
}

sockaddr fdBase::getpeername(int filedesc)
{
	struct sockaddr_storage ss;
	socklen_t ss_len=sizeof(ss);

	if (::getpeername(filedesc, (struct ::sockaddr *)&ss, &ss_len) < 0)
		throw SYSEXCEPTION("getpeername");

	sockaddr sa=sockaddr::create();

	sa->resize(ss_len);

	std::copy((const char *)&ss,
		  (const char *)&ss+ss_len, sa->begin());
	return sa;
}

std::string fdObj::getpeername_str() const
{
	std::string n(*getpeername());

	if (n == sockaddrObj::anonlocalsocket)
		return *getsockname();

	return n;
}

void fdObj::bind(const const_sockaddr &addr)
{
	bind( (const struct ::sockaddr *)&*addr->begin(), addr->size());
}

void fdObj::bind(const struct ::sockaddr *serv_addr,
		 socklen_t addrlen)
{
	if (::bind(filedesc, serv_addr, addrlen) < 0)
	{
		std::string s("unknown");

		sockaddr sabuf=sockaddr::create();

		sabuf->resize(addrlen);

		memcpy(&*sabuf->begin(), (const char *)serv_addr, addrlen);

		try {
			s= *sabuf;
		} catch (...) {

		}

		throw SYSEXCEPTION("bind(" + s + ")");
	}
}

void fdObj::listen(int backlog)
{
	if (::listen(filedesc, backlog))
		throw SYSEXCEPTION("listen");
}

fdptr fdObj::accept(sockaddrptr &peername)
{
	struct sockaddr_storage ss;
	socklen_t sslen=sizeof(ss);

	fdptr newFd;

	int nfd;


	if ((nfd=accept4(filedesc, (struct ::sockaddr *)&ss,
			 &sslen, SOCK_CLOEXEC)) < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return newFd;

		throw SYSEXCEPTION("accept");
	}

	try {
		newFd=fd::base::adopt(nfd);
	} catch (...) {
		::close(filedesc);
		throw;
	}

	newFd->setsigpipe(nfd);
	newFd->nonblock(false);

	peername=sockaddr::create();

	peername->resize(sslen);

	std::copy( (const char *)&ss, (const char *)&ss + sslen,
		   peername->begin());

	return newFd;
}

void fdObj::connect(const const_sockaddr &addr)
{
	connect( (const struct ::sockaddr *)&*addr->begin(), addr->size());
}

void fdObj::connect(const struct ::sockaddr *serv_addr,
		    socklen_t addrlen)
{
 again:
	if (::connect(filedesc, serv_addr, addrlen) < 0)
	{
		if (errno == EINTR)
			goto again;

		badconnect(serv_addr, addrlen);
	}
}

fdptr fdObj::accept()
{
	sockaddrptr dummy;

	return accept(dummy);
}

void fdObj::cancel()
{
	ptr<closeactionObj> hook;

	{
		std::lock_guard<std::mutex> lock(objmutex);

		hook=closeaction;
		closeaction=ptr<closeactionObj>();
	}

	if (!hook.null())
		hook->cancel();
}

void fdObj::pubclose()
{
	close();
}

void fdObj::close()
{
	int rc;

	if (filedesc < 0)
		return;

	if (!epoll_set.null())
	{
		try {
			epoll_set->epoll_ctl(EPOLL_CTL_DEL, this, 0);
		} catch (exception &e)
		{
		}
		epoll_set=epollptr();
		epoll_callback=epoll::base::callback();
	}

	ptr<closeactionObj> hook;

	{
		std::lock_guard<std::mutex> lock(objmutex);

		hook=closeaction;
		closeaction=ptr<closeactionObj>();
	}

	if (!hook.null())
		hook->beforeclose();

	rc=::close(filedesc);
	filedesc= -1;

	init();

	if (rc < 0)
	{
		if (!hook.null())
			hook->cancel();
		throw SYSEXCEPTION("close");
	}

	if (!hook.null())
	    hook->afterclose();
}

void fdObj::shutdown(int how)
{
	if (::shutdown(filedesc, how) < 0)
		throw SYSEXCEPTION("shutdown()");
}

void fdObj::epoll_mod(uint32_t newEvents)
{
	if (newEvents)
	{
		epoll_set->epoll_ctl(EPOLL_CTL_MOD, this, newEvents);
	}
	else
	{
		epoll_set->epoll_ctl(EPOLL_CTL_DEL, this, 0);
		epoll_set=epollptr();
		epoll_callback=epoll::base::callback();
	}
}

void fdObj::epoll_add(uint32_t newEvents,
		      const epoll &new_epoll_set,
		      const epoll::base::callback &new_epoll_callback)
{
	if (!epoll_set.null())
	{
		epoll_set->epoll_ctl(EPOLL_CTL_DEL, this, 0);
		epoll_set=epollptr();
		epoll_callback=epoll::base::callback();
	}

	new_epoll_set->epoll_ctl(EPOLL_CTL_ADD, this, newEvents);
	epoll_set=new_epoll_set;
	epoll_callback=new_epoll_callback;
}

off64_t fdObj::seek(off64_t offset,
		    int whence)
{
#if HAVE_LSEEK64
	ssize_t n=lseek64(filedesc, offset, whence);
#else
	ssize_t n=lseek(filedesc, offset, whence);
#endif

	if (n < 0)
		throw SYSEXCEPTION("seek");

	return n;
}

void fdObj::truncate(off64_t offset)
{
#if HAVE_FTRUNCATE64
	if (ftruncate64(filedesc, offset) < 0)
		throw SYSEXCEPTION("ftruncate");
#else
	if (sizeof(off64_t) != sizeof(off_t) &&
	    (off64_t)(off_t)offset != offset)
	{
		errno=ERANGE;
		throw SYSEXCEPTION("ftruncate");
	}
	if (ftruncate(filedesc, offset) < 0)
		throw SYSEXCEPTION("ftruncate");
#endif
}

void fdObj::ipv6only(bool flag)
{
	int on=flag ? 1:0;

	if (setsockopt(filedesc, IPPROTO_IPV6, IPV6_V6ONLY,
		       (char *)&on, sizeof(on)) < 0)
		throw SYSEXCEPTION("setsockopt(IPV6_V6ONLY, IPV6_V6ONLY)");

}

bool fdObj::ipv6only()
{
	int flagint=0;
	socklen_t flaglen=sizeof(flagint);

	if (getsockopt(filedesc, IPPROTO_IPV6, IPV6_V6ONLY, &flagint, &flaglen)
	    < 0)
		throw SYSEXCEPTION("getsockopt(IPPROTO_IPV6, IPV6_V6ONLY)");

	return flagint ? true:false;
}

void fdObj::closeonexec(bool flag)
{
	if (fcntl(filedesc, F_SETFD, flag ? FD_CLOEXEC:0) < 0)
		throw SYSEXCEPTION("F_SETFD(closeonexec)");
}

bool fdObj::closeonexec()
{
	int flags=fcntl(filedesc, F_GETFD, 0);

	if (flags < 0)
		throw SYSEXCEPTION("F_GETFD");

	return flags & FD_CLOEXEC ? true:false;
}

void fdObj::nonblock(bool flag)
{
	int fm=fcntl(filedesc, F_GETFL);

	if (fm < 0)
		throw SYSEXCEPTION("fcntl");

	fm &= ~O_NONBLOCK;

	if (flag)
		fm |= O_NONBLOCK;

	if (fcntl(filedesc, F_SETFL, fm) < 0)
		throw SYSEXCEPTION("fcntl");
}

bool fdObj::nonblock()
{
	int fm=fcntl(filedesc, F_GETFL);

	if (fm < 0)
		throw SYSEXCEPTION("fcntl");

	return (fm & O_NONBLOCK) != 0;
}

void fdObj::keepalive(bool flag)
{
	int flagint=flag ? 1:0;

	if (setsockopt(filedesc, SOL_SOCKET, SO_KEEPALIVE, &flagint,
		       sizeof(flagint)) < 0)
		throw SYSEXCEPTION("setsockopt(SOL_SOCKET, SO_KEEPALIVE)");
}

bool fdObj::keepalive()
{
	int flagint=0;
	socklen_t flaglen=sizeof(flagint);

	if (getsockopt(filedesc, SOL_SOCKET, SO_KEEPALIVE, &flagint, &flaglen)
	    < 0)
		throw SYSEXCEPTION("getsockopt(SOL_SOCKET, SO_KEEPALIVE)");

	return flagint != 0;
}

fdObj::fdObj(int nfiledesc)
{
	init();
	setsigpipe(nfiledesc);
	filedesc=nfiledesc;
}

bool fdObj::send_fd(const int *fdesc_array,
		    size_t fdesc_array_size)
{
	if (fdesc_array_size == 0)
		return true;

	for (size_t i=0; i<fdesc_array_size; i++)
		if (fdesc_array[i] < 0)
		{
			errno=EBADF;
			throw SYSEXCEPTION("send_fd");
		}

	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct iovec iov;
	char dummy;

	size_t buf_s=CMSG_SPACE(sizeof(int)*fdesc_array_size);

	char buf[buf_s];

	memset(buf, 0, buf_s);

	memset(&msg, 0, sizeof(msg));
	msg.msg_control=buf;
	msg.msg_controllen=buf_s;

	cmsg=CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level=SOL_SOCKET;
	cmsg->cmsg_type=SCM_RIGHTS;
	cmsg->cmsg_len=CMSG_LEN(sizeof(int)*fdesc_array_size);

	memcpy((char *)CMSG_DATA(cmsg), (const char *)fdesc_array,
	       sizeof(int)*fdesc_array_size);

	memset(&iov, 0, sizeof(iov));
	msg.msg_iov=&iov;
	msg.msg_iovlen=1;
	iov.iov_base=&dummy;
	iov.iov_len=1;
	dummy=0;

	if (sendmsg(filedesc, &msg, 0) < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK
#ifdef EMSGSIZE
		    || errno == EMSGSIZE
#endif
		    )
			return false;

		throw SYSEXCEPTION("sendmsg");
	}
	return true;
}

bool fdObj::send_fd(fd *fdesc_array,
		    size_t fdesc_array_size)
{
	if (fdesc_array_size == 0)
		return true;

	int buf[fdesc_array_size];

	size_t i=0;

	for (i=0; i<fdesc_array_size; i++)
	{
		buf[i]=fdesc_array[i]->filedesc;
	}

	return send_fd(buf, fdesc_array_size);
}

bool fdObj::send_fd(fdptr *fdesc_array,
		    size_t fdesc_array_size)
{
	if (fdesc_array_size == 0)
		return true;

	int buf[fdesc_array_size];

	size_t i=0;

	for (i=0; i<fdesc_array_size; i++)
	{
		buf[i]=fdesc_array[i]->filedesc;
	}

	return send_fd(buf, fdesc_array_size);
}

bool fdObj::send_fd(const std::vector<fd > &fdesc_array)

{
	if (fdesc_array.size() == 0)
		return true;

	int buf[fdesc_array.size()];

	size_t n=0;

	std::vector<fd >::const_iterator
		b=fdesc_array.begin(), e=fdesc_array.end();

	while (b != e)
	{
		buf[n]=(*b)->filedesc;
		++n;
		++b;
	}

	return send_fd(buf, n);
}

bool fdObj::send_fd(const std::vector<int> &fdesc_array)

{
	return send_fd(&fdesc_array[0], fdesc_array.size());
}

bool fdObj::send_fd(const fd &fDesc)
{
	return send_fd(&fDesc->filedesc, 1);
}

bool fdObj::send_fd(int fDesc)
{
	return send_fd(&fDesc, 1);
}

class fdObj::recvmsg_helper {

	int *fdesc_array;
	size_t fdesc_array_size;

	size_t n_fds;

	fd::base::credentials_t *c;
	bool c_received;

public:

	size_t operator()(struct msghdr *msg,
			  int *fdesc_arrayArg,
			  size_t fdesc_array_sizeArg);

	bool operator()(struct msghdr *msg, fd::base::credentials_t *cArg)
;

private:
	void go(struct msghdr *msg);
};

size_t fdObj::recvmsg_helper::operator()(struct msghdr *msg,
					 int *fdesc_arrayArg,
					 size_t fdesc_array_sizeArg)

{
	fdesc_array=fdesc_arrayArg;
	fdesc_array_size=fdesc_array_sizeArg;
	n_fds=0;
	c=NULL;
	c_received=false;

	go(msg);

	return n_fds;
}

bool fdObj::recvmsg_helper::operator()(struct msghdr *msg,
				       fd::base::credentials_t *cArg)

{
	fdesc_array=NULL;
	fdesc_array_size=0;
	n_fds=0;
	c=cArg;
	c_received=false;

	go(msg);

	return c_received;
}

void fdObj::recvmsg_helper::go(struct msghdr *msg)
{
	for (struct cmsghdr *cmsg=CMSG_FIRSTHDR(msg); cmsg != NULL;
	     cmsg=CMSG_NXTHDR(msg, cmsg))
	{
                if (cmsg->cmsg_level == SOL_SOCKET
		    && cmsg->cmsg_type == SCM_RIGHTS)
		{
			int *fd_recv=(int *)CMSG_DATA(cmsg);
			size_t cnt=(cmsg->cmsg_len - CMSG_LEN(0))
				/ sizeof(int);

			while (cnt)
			{
				if (fdesc_array && fdesc_array_size)
				{
					*fdesc_array++= *fd_recv;
					--fdesc_array_size;
					++n_fds;
				}
				else
				{
					::close(*fd_recv);
				}
				++fd_recv;
				--cnt;
			}
		}

#ifdef SCM_CREDENTIALS

		if (cmsg->cmsg_level == SOL_SOCKET &&
		    cmsg->cmsg_type == SCM_CREDENTIALS && c)
		{
			struct ucred *cp=(struct ucred *)CMSG_DATA(cmsg);
			size_t cp_len=cmsg->cmsg_len - CMSG_LEN(0);

			if (cp_len > sizeof(*c))
				cp_len=sizeof(*c);

			memset(c, 0, sizeof(*c));
			memcpy(c, cp, cp_len);
			c_received=true;
		}
#else
		if (cmsg->cmsg_level == SOL_SOCKET &&
		    cmsg->cmsg_type == SCM_CREDS && c)
		{
			struct cmsgcred *cp=(struct cmsgcred *)CMSG_DATA(cmsg);
			size_t cp_len=cmsg->cmsg_len - CMSG_LEN(0);

			if (cp_len >= sizeof(*cp))
			{
				memset(c, 0, sizeof(*c));

				c->uid=cp->cmcred_uid;
				c->gid=cp->cmcred_gid;
				c->pid=cp->cmcred_pid;
			}
			c_received=true;
		}
#endif
	}
}

size_t fdObj::recv_fd(int *fdesc_array,
		      size_t fdesc_array_size)
{
	if (fdesc_array_size == 0)
		return (0);

	struct msghdr msg;
	struct iovec iov;
	char dummy;

	size_t buf_s=CMSG_SPACE(sizeof(int)*fdesc_array_size)+1024;
	char buf[buf_s];

	memset(&msg, 0, sizeof(msg));
	msg.msg_control=buf;
	msg.msg_controllen=buf_s;

	msg.msg_iov=&iov;
	msg.msg_iovlen=1;
	iov.iov_base=&dummy;
	iov.iov_len=1;

	ssize_t rc=recvmsg(filedesc, &msg, MSG_CMSG_CLOEXEC);

	if (rc < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
		return 0;

	if (rc <= 0)
	{
		if (rc == 0)
			errno=ESHUTDOWN;
		throw SYSEXCEPTION("recvmsg");
	}

	return recvmsg_helper()(&msg, fdesc_array, fdesc_array_size);
}

size_t fdObj::recv_fd(fdptr *fdesc_array,
		      size_t fdesc_array_size)
{
	int fdesc_buf[fdesc_array_size];

	size_t n=recv_fd(fdesc_buf, fdesc_array_size);

	for (size_t i=0; i<fdesc_array_size; ++i)
		fdesc_array[i]=fdptr();

	for (size_t i=0; i<n; i++)
	{
		fdptr p;

		try {
			p=fd::base::adopt(fdesc_buf[i]);
		} catch (...) {
			while (++i < n)
				::close(fdesc_buf[i]);
			throw;
		}

		fdesc_array[i]=p;
	}
	return n;
}

void fdObj::recv_fd(std::vector<fdptr> &fdesc_array)
{
	size_t n=recv_fd(&fdesc_array[0], fdesc_array.size());

	fdesc_array.resize(n);
}

void fdObj::recv_fd(std::vector<int> &fdesc_array)
{
	size_t n=recv_fd(&fdesc_array[0], fdesc_array.size());

	fdesc_array.resize(n);
}

bool fdObj::send_credentials(pid_t p, uid_t u, gid_t g)
{
#ifdef SCM_CREDENTIALS
	struct ucred c;

	memset(&c, 0, sizeof(c));

	c.pid=p;
	c.uid=u;
	c.gid=g;
#else
	struct cmsgcred c;

	memset(&c, 0, sizeof(c));
#endif

	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct iovec iov;
	char dummy;

	size_t buf_s=CMSG_SPACE(sizeof(c));

	char buf[buf_s];

	memset(buf, 0, buf_s);

	memset(&msg, 0, sizeof(msg));
	msg.msg_control=buf;
	msg.msg_controllen=buf_s;

	cmsg=CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level=SOL_SOCKET;

#ifdef SCM_CREDENTIALS
	cmsg->cmsg_type=SCM_CREDENTIALS;
#else
	cmsg->cmsg_type=SCM_CREDS;
#endif

	cmsg->cmsg_len=CMSG_LEN(sizeof(c));

	memcpy((char *)CMSG_DATA(cmsg), (const char *)&c, sizeof(c));

	memset(&iov, 0, sizeof(iov));
	msg.msg_iov=&iov;
	msg.msg_iovlen=1;
	iov.iov_base=&dummy;
	iov.iov_len=1;
	dummy=0;

	if (sendmsg(filedesc, &msg, 0) < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return false;

		throw SYSEXCEPTION("sendmsg");
	}
	return true;
}

void fdObj::recv_credentials(bool flag)
{
	int cflag=flag ? 1:0;

	if (setsockopt(filedesc, SOL_SOCKET,
#ifdef SO_PASSCRED
		       SO_PASSCRED,
#else
		       LOCAL_PEERCRED,
#endif
		       &cflag,
		       sizeof(cflag)) < 0)
		throw SYSEXCEPTION("setsockopt(SOL_SOCKET, SO_PASSCRED)");
}

bool fdObj::recv_credentials(fd::base::credentials_t &c)
{
	struct msghdr msg;
	struct iovec iov;
	char dummy;

#ifdef SCM_CREDENTIALS

	char buf[CMSG_SPACE(sizeof(struct ucred))];
#else
	char buf[CMSG_SPACE(sizeof(struct cmsgcred))];
#endif

	memset(&msg, 0, sizeof(msg));
	msg.msg_control=buf;
	msg.msg_controllen=sizeof(buf);

	msg.msg_iov=&iov;
	msg.msg_iovlen=1;
	iov.iov_base=&dummy;
	iov.iov_len=1;

	ssize_t rc=recvmsg(filedesc, &msg, 0);

	if (rc < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
		return false;

	if (rc <= 0)
	{
		if (rc == 0)
			errno=ESHUTDOWN;
		throw SYSEXCEPTION("recvmsg");
	}

	if (!recvmsg_helper()(&msg, &c))
		throw EXCEPTION("recvmsg: did not receive credentials");

	return true;
}

void fdObj::setsigpipe(int fd) noexcept
{
	int flag;
	socklen_t flag_s=sizeof(flag);

	sigpipeFlag= fd >= 0 &&
		getsockopt(fd, SOL_SOCKET, SO_TYPE, &flag, &flag_s) == 0
		? false:true;
}

int fdObj::stat_internal(struct ::stat *buf) noexcept
{
	if (filedesc < 0)
	{
		errno=EBADF;
		return -1;
	}

	return fstat(filedesc, buf);
}

#if HAVE_XATTR

ssize_t fdObj::getattr_internal(const char *name,
				void *value,
				size_t size)
	noexcept
{
	if (filedesc < 0)
	{
		errno=EBADF;
		return -1;
	}

	return fgetxattr(filedesc, name, value, size);
}

int fdObj::setattr_internal(const char *name,
			    const void *value,
			    size_t size,
			    int flags)
	noexcept
{
	if (filedesc < 0)
	{
		errno=EBADF;
		return -1;
	}

	return fsetxattr(filedesc, name, value, size, flags);
}

int fdObj::removeattr_internal(const char *name)
	noexcept
{
	if (filedesc < 0)
	{
		errno=EBADF;
		return -1;
	}

	return fremovexattr(filedesc, name);
}
#else
#if HAVE_EXTATTR

extern int extattr_parsens(int &ns, const char * &name) noexcept;

ssize_t fdObj::getattr_internal(const char *name,
				void *value,
				size_t size)
	noexcept
{
	int ns;

	if (extattr_parsens(ns, name) < 0)
		return -1;

	return extattr_get_fd(filedesc, ns, name, value, size);
}

int fdObj::setattr_internal(const char *name,
			    const void *value,
			    size_t size,
			    int flags)
	noexcept
{
	int ns;

	if (extattr_parsens(ns, name) < 0)
		return -1;

	if (flags)
	{
		if (extattr_get_fd(filedesc, ns, name, NULL, 0) < 0)
		{
			if (flags & XATTR_REPLACE)
			{
				errno=ENOATTR;
				return -1;
			}
		}
		else
		{
			if (flags & XATTR_CREATE)
			{
				errno=EEXIST;
				return -1;
			}
		}
	}

	return extattr_set_fd(filedesc, ns, name, value, size) < 0 ? -1:0;
}

int fdObj::removeattr_internal(const char *name)
	noexcept
{
	int ns;

	if (extattr_parsens(ns, name) < 0)
		return -1;

	return extattr_delete_fd(filedesc, ns, name);
}

#else
ssize_t fdObj::getattr_internal(const char *name,
				void *value,
				size_t size)
	noexcept
{
	errno=ENODEV;
	return -1;
}

int fdObj::setattr_internal(const char *name,
			    const void *value,
			    size_t size,
			    int flags)
	noexcept
{
	errno=ENODEV;
	return -1;
}

int fdObj::removeattr_internal(const char *name)
	noexcept
{
	errno=ENODEV;
	return -1;
}
#endif
#endif

int fdObj::chmod_internal(mode_t mode) noexcept
{
	if (filedesc < 0)
	{
		errno=EBADF;
		return -1;
	}

	return fchmod(filedesc, mode);
}

int fdObj::chown_internal(uid_t uid, gid_t group) noexcept
{
	if (filedesc < 0)
	{
		errno=EBADF;
		return -1;
	}

	return fchown(filedesc, uid, group);
}

std::string fdObj::whoami()
{
	std::ostringstream o;

	o << "/dev/fd/";

	if (filedesc < 0)
	{
		o << "/unknown";
		return o.str();
	}

	o << filedesc;

	int save_err=errno;

	try {
		return fileattr::create(o.str())->readlink();
	} catch (...)
	{
		errno=save_err;
	}
	return o.str();
}

bool fdObj::lockf(int cmd, off64_t len)
{
	if ( ::lockf(filedesc, cmd, len) == 0)
		return true;

	if ((cmd == F_TLOCK || cmd == F_TEST) &&
	    (errno == EACCES || errno == EAGAIN || errno == EDEADLK))
		return false;

	throw SYSEXCEPTION("lockf()");
}

size_t fdObj::pubread(char *buffer, size_t cnt)
{
	return read(buffer, cnt);
}

size_t fdObj::pubread_pending() const
{
	return 0;
}

size_t fdObj::pubwrite(const char *buffer, size_t cnt)
{
	return write(buffer, cnt);
}

off64_t fdObj::pubseek(off64_t offset, int whence)
{
	return seek(offset, whence);
}

// =========================================================================

// This is the actual mcguffin returned by fdObj::futimens_interval().
// It holds a reference to a timer-scheduled task that periodically updates
// file times. It also contains definitions of other classes that makes
// this whole thing happen.
//
// The destructor cancels the task.

class fdObj::futimens_intervalObj : virtual public obj {

public:

	// Here's the regularly-scheduled task that updates file times

	class task : public timertaskObj {

	public:
		fd update;

		task(const fd &updateArg) LIBCXX_HIDDEN
			: update(updateArg) {}

		~task() noexcept {}

		void run() LIBCXX_HIDDEN
		{
			update->futimens();
		}
	};

	// The scheduled task

	ptr<task> updatetask;

	futimens_intervalObj(const ptr<task> &updatetaskArg) LIBCXX_HIDDEN;
	~futimens_intervalObj() noexcept LIBCXX_HIDDEN;

};

fdObj::futimens_intervalObj::futimens_intervalObj(const ptr<task>
						  &updatetaskArg)
 : updatetask(updatetaskArg)
{
}

fdObj::futimens_intervalObj::~futimens_intervalObj() noexcept
{
	updatetask->cancel();
}

std::pair<timertask, ref<obj> > fdObj::get_futimens_task()
{
	fd dup(fd::base::dup(fd(this)));

	auto updatetask=ref<futimens_intervalObj::task>::create(dup);
	auto mcguffin=ref<futimens_intervalObj>::create(updatetask);

	return std::make_pair(updatetask, mcguffin);
}

std::string fdBase::mktempdir(mode_t mode)
{
	static const char pfix[]="/tmp/libcxx.app.dir.";

	std::ostringstream o;

	o << pfix << geteuid() << "." << getegid() << "."
	  << std::oct << std::setw(4) << std::setfill('0') << mode;

	std::string n=o.str();

	mkdir(n.c_str(), 0700);
	chmod(n.c_str(), mode);
	if (chown(n.c_str(), geteuid(), getegid()) < 0)
		; // Ignore

	auto s=fileattr::create(n, true)->stat();

	if (s->st_uid == geteuid() && s->st_gid == getegid() &&
	    S_ISDIR(s->st_mode) && (s->st_mode & 0777) == mode)
		return n;

	char pfix_buf[sizeof(pfix)+10];

	strcpy(pfix_buf, "/tmp/libcxx.app.XXXXXX");
	if (mkdtemp(pfix_buf) == nullptr)
		throw SYSEXCEPTION(pfix_buf);

	chmod(pfix_buf, mode);
	return pfix_buf;
}

std::pair<fd, std::string>
fdBase::tmpunixfilesock(const std::string &pfix)
{
	timespec ts=timespec::getclock();

	uint64_t nn=ts.tv_nsec;

	nn ^= ts.tv_sec << 8;

	ts.tv_sec ^= getpid();

	for (size_t i=0; i<1000; ++i)
	{
		std::ostringstream o;

		o << pfix;

		uint64_t nv=nn;

		for (size_t n=0; n<8; ++n)
		{
			o << uuid::base64_t::alphabet_t::alphabet[ nv & 63 ];

			nv /= 64;
		}

		std::string s=o.str();

		try {
			std::list<fd> fds;

			netaddr::create(SOCK_STREAM, "file:" + s)
				->bind(fds, false);

			if (!fds.empty())
			{
				chmod(s.c_str(), 0777);
				return std::make_pair(fds.front(), s);
			}
		} catch (const x::sysexception &e)
		{
			if (e.getErrorCode() != EADDRINUSE)
				throw;

			nn= (nn << 8) | (nn >> 56);

			try {
				nn += fileattr::create(s, true)->stat()->st_ino;
				continue;
			} catch (...) {
			}
			nn += timespec::getclock().tv_nsec;
		}
	}

	errno=ENOSPC;
	throw SYSEXCEPTION(pfix);
}

#if 0
{
#endif
}
