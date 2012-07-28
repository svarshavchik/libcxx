/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef portmap_server_H
#define portmap_server_H

#include "x/fd.H"
#include "x/logger.H"
#include "x/weakmultimap.H"
#include "reexecfd.H"

#include <vector>

class httportmap_server;

class portmap_server : virtual public LIBCXX_NAMESPACE::obj {

	LOG_CLASS_SCOPE;

	httportmap_server &portmap;

	std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd> stop_pipe;

	struct clientinfoObj;

	typedef LIBCXX_NAMESPACE::ref<clientinfoObj> clientinfo;
	typedef LIBCXX_NAMESPACE::ptr<clientinfoObj> clientinfoptr;

public:

	std::list<LIBCXX_NAMESPACE::fd> listen_sockets;

	portmap_server(httportmap_server &portmapArg);
	~portmap_server() noexcept;

	void stop();

	void run(reexec_fd_recvptr &);

private:
	void run2(reexec_fd_recvptr &);

	void newconn(const x::fd &conn,
		     std::vector<struct pollfd> &fds,
		     std::vector<clientinfo> &clientinfos);

	bool handle_client(clientinfoObj &, std::vector<clientinfo> &);
};


#endif
