/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "reexecfd.H"

reexec_fd::reexec_fd(const LIBCXX_NAMESPACE::fdbase &fdArg,
		     const LIBCXX_NAMESPACE::fd &socketArg)
	: fd(fdArg), socket(socketArg), in_iter(fd), out_iter(fd)
{
}

void reexec_fd::send_fd(const LIBCXX_NAMESPACE::fd &sfd)
{
	out_iter.flush();
	++in_iter;
	socket->send_fd(sfd);
	++in_iter;
}

LIBCXX_NAMESPACE::fd reexec_fd::recv_fd()
{
	*out_iter++=0;
	out_iter.flush();

	std::vector<LIBCXX_NAMESPACE::fdptr> sfd;

	sfd.resize(1);

	bool was_nonblock=socket->nonblock();
	socket->nonblock(false);
	socket->recv_fd(sfd);
	socket->nonblock(was_nonblock);

	*out_iter++=0;
	out_iter.flush();

	sfd.resize(1);

	return sfd[0];
}

reexec_fd_send::reexec_fd_send(const LIBCXX_NAMESPACE::fd &fdArg,
			       const LIBCXX_NAMESPACE::fd &socketArg)
	: reexec_fd(fdArg, socketArg), iter(out_iter)
{
}

reexec_fd_recvObj::reexec_fd_recvObj(const LIBCXX_NAMESPACE::fdbase &fdArg,
				     const LIBCXX_NAMESPACE::fd &socketArg)
	: reexec_fd(fdArg, socketArg), iter(in_iter, in_iter_end)
{
}

