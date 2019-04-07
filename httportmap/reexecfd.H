/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef reexec_fd_H
#define reexec_fd_H

#include "x/fd.H"
#include "x/fditer.H"
#include "x/serialize.H"
#include "x/deserialize.H"

class reexec_fd {

public:
	LIBCXX_NAMESPACE::fdbase fd;
	LIBCXX_NAMESPACE::fd socket;

	LIBCXX_NAMESPACE::fdinputiter in_iter, in_iter_end;

	LIBCXX_NAMESPACE::fdoutputiter out_iter;

	void send_fd(const LIBCXX_NAMESPACE::fd &sfd);
	LIBCXX_NAMESPACE::fd recv_fd();

	reexec_fd(const LIBCXX_NAMESPACE::fdbase &fdArg,
		  const LIBCXX_NAMESPACE::fd &socketArg);
	~reexec_fd() {}
};

class reexec_fd_send : public reexec_fd {

public:
	LIBCXX_NAMESPACE::serialize::iterator<LIBCXX_NAMESPACE::fdoutputiter> iter;

	reexec_fd_send(const LIBCXX_NAMESPACE::fd &fdArg,
		       const LIBCXX_NAMESPACE::fd &socketArg);
	~reexec_fd_send() {}

	reexec_fd_send(const reexec_fd_send &)=delete;
	reexec_fd_send &operator=(const reexec_fd_send &)=delete;
};

class reexec_fd_recvObj : public reexec_fd, virtual public LIBCXX_NAMESPACE::obj {

public:
	LIBCXX_NAMESPACE::deserialize::iterator<LIBCXX_NAMESPACE::fdinputiter> iter;

	reexec_fd_recvObj(const LIBCXX_NAMESPACE::fdbase &fdArg,
			  const LIBCXX_NAMESPACE::fd &socketArg);
	~reexec_fd_recvObj() {}

	reexec_fd_recvObj(const reexec_fd_recvObj &)=delete;
	reexec_fd_recvObj &operator=(const reexec_fd_recvObj &)=delete;
};

typedef LIBCXX_NAMESPACE::ref<reexec_fd_recvObj> reexec_fd_recv;

typedef LIBCXX_NAMESPACE::ptr<reexec_fd_recvObj> reexec_fd_recvptr;

#endif
