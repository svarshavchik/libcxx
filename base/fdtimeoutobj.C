/*
** Copyright 2012-2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "fdtimeout.H"
#include "sysexception.H"
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Constructor

fdtimeoutObj::fdtimeoutObj(const fdbase &ptr)
	: fdbaseObj::adapterObj(ptr),
	  timedout_read(false),
	  timedout_write(false),
	  read_timer_set(false),
	  write_timer_set(false),
	  periodic_read_count(0),
	  periodic_read_counter(0),
	  periodic_write_count(0),
	  periodic_write_counter(0)
{
}

fdtimeoutObj::~fdtimeoutObj() noexcept
{
}

void fdtimeoutObj::set_read_timeout(const timespec &howlong)
{
	if (howlong == 0)
		cancel_read_timer();
	else
	{
		read_timer=timespec::getclock(CLOCK_MONOTONIC);

		read_timer += howlong;
		read_timer_set=true;
	}
}

void fdtimeoutObj::set_read_timeout(size_t bytecnt, const timespec &howlong)

{
	if (howlong == 0)
		cancel_read_timer();
	else
	{
		periodic_read_timeout=howlong;
		periodic_read_count=bytecnt;
		reset_periodic_read_timer();
	}
}

void fdtimeoutObj::reset_periodic_read_timer()
{
	set_read_timeout(periodic_read_timeout);
	periodic_read_counter=periodic_read_count;
}

void fdtimeoutObj::reset_periodic_write_timer()
{
	set_write_timeout(periodic_write_timeout);
	periodic_write_counter=periodic_write_count;
}

void fdtimeoutObj::set_write_timeout(const timespec &howlong)
{
	cancel_write_timer();

	if (howlong != 0)
	{
		write_timer=timespec::getclock(CLOCK_MONOTONIC);

		write_timer += howlong;
		write_timer_set=true;
	}
}

void fdtimeoutObj::set_write_timeout(size_t bytecnt, const timespec &howlong)

{
	cancel_write_timer();

	if (howlong != 0)
	{
		periodic_write_timeout=howlong;
		periodic_write_count=bytecnt;
		reset_periodic_write_timer();
	}
}

void fdtimeoutObj::cancel_read_timer()
{
	read_timer_set=false;
	timedout_read=false;
}

void fdtimeoutObj::cancel_write_timer()
{
	write_timer_set=false;
	timedout_write=false;
}

size_t fdtimeoutObj::pubread(char *buffer, size_t cnt)
{
	size_t n=0;

	if (!timedout_read)
	{
		n=pubread_pending();

		if (n > 0)
		{
			if (n < cnt)
				cnt=n;

			return fdptr->pubread(buffer, cnt);
		}

		if (!read_timer_set && terminatefdref.null())
			return fdptr->pubread(buffer, cnt);

		while(1)
		{
			if (wait_timer(read_timer_set, read_timer, POLLIN))
			{
				timedout_read=true;
				break;
			}

			if ((n=fdptr->pubread(buffer, cnt)) > 0)
				break;

			if (errno == 0)
				break; // EOF
		}
	}

	if (timedout_read)
	{
		errno=ETIMEDOUT;
		throw SYSEXCEPTION("read");
	}

	if (periodic_read_timeout != 0)
	{
		if (periodic_read_counter <= (size_t)n)
			reset_periodic_read_timer();
		else
			periodic_read_counter -= n;
	}

	return n;

}

size_t fdtimeoutObj::pubwrite(const char *buffer, size_t cnt)
{
	size_t n=0;

	if (!timedout_write)
	{
		if (!write_timer_set && terminatefdref.null())
			return fdptr->pubwrite(buffer, cnt);

		while (1)
		{
			if (wait_timer(write_timer_set, write_timer, POLLOUT))
			{
				timedout_write=true;
				break;
			}

			if ((n=fdptr->pubwrite(buffer, cnt)) > 0)
				break;
		}
	}

	if (timedout_write)
	{
		errno=ETIMEDOUT;
		throw SYSEXCEPTION("write");
	}

	if (periodic_write_timeout != 0)
	{
		if (periodic_write_counter <= (size_t)n)
			reset_periodic_write_timer();
		else
			periodic_write_counter -= n;
	}
	return n;
}

void fdtimeoutObj::pubconnect(const struct ::sockaddr *serv_addr,
			      socklen_t addrlen)
{
	if (!write_timer_set && terminatefdref.null())
	{
		fdptr->pubconnect(serv_addr, addrlen);
		return;
	}

	if (timedout_write)
	{
		errno=ETIMEDOUT;
		badconnect(serv_addr, addrlen);
	}

	int fm=fcntl(getFd(), F_GETFL);

	if (fm < 0)
		throw SYSEXCEPTION("fcntl(F_GETFL)");

	if (fcntl(getFd(), F_SETFL, fm | O_NONBLOCK) < 0)
		throw SYSEXCEPTION("fcntl(F_SETFL)");

	try {
		try {

			fdptr->pubconnect(serv_addr, addrlen);
		} catch (const sysexception &e)
		{
			if (e.getErrorCode() != EINPROGRESS)
				throw;

			if (wait_timer(write_timer_set, write_timer,
				       POLLOUT))
			{
				errno=ETIMEDOUT;
				timedout_write=true;
				badconnect(serv_addr, addrlen);
			}

			int so_error=0;
			socklen_t so_error_l;

			so_error_l=sizeof(so_error);

			if (getsockopt(getFd(), SOL_SOCKET, SO_ERROR,
				       &so_error, &so_error_l)
			    < 0)
				throw SYSEXCEPTION("getsockopt(SOL_SOCKET, SO_ERROR)");

			if (so_error)
			{
				errno=so_error;
				badconnect(serv_addr, addrlen);
			}
		}

		if (fcntl(getFd(), F_SETFL, fm) < 0)
			throw SYSEXCEPTION("fcntl(F_SETFL)");
	} catch (...) {
		fcntl(getFd(), F_SETFL, fm);
		throw;
	}
}

#ifndef HAVE_PPOLL

#define ppoll(fds, nfds, ts, sigmask)					\
	poll(fds, nfds, ts ? ( (ts)->tv_sec * 1000 + (ts)->tv_nsec / 1000000) \
	     : -1)

#endif

bool fdtimeoutObj::wait_timer(bool timer_set,
			      const timespec &timer,
			      int events)
{
	struct pollfd pfd[2];

	pfd[0].fd=getFd();
	pfd[0].events=events;

	size_t n=1;

	if (!terminatefdref.null())
	{
		pfd[n].fd=terminatefdref->getFd();
		pfd[n].events=POLLIN;
		++n;
	}

	int rc;

	while (1)
	{
		timespec now;

		timespec *now_ptr=NULL;

		if (timer_set)
		{
			now=timespec::getclock(CLOCK_MONOTONIC);

			if (now >= timer)
				now=0;
			else
				now=timer-now;

			now_ptr= &now;
		}

		if ((rc=ppoll(pfd, n, now_ptr, NULL)) < 0)
		{
			if (errno != EINTR)
				throw EXCEPTION("poll");
			continue;
		}

		if (rc == 0)
			break; // Timed out

		for (size_t i=1; i<n; i++)
			if (pfd[i].revents & (POLLIN | POLLHUP | POLLERR))
				return true;

		if (pfd[0].revents & (events | POLLHUP | POLLERR))
			return false;
	}

	return true;
}


#if 0
{
#endif
}
