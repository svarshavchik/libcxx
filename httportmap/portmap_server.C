/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "portmap_server.H"
#include "httportmap_server.H"
#include "reexecfd.H"
#include "x/pwd.H"
#include "x/strtok.H"
#include "x/property_properties.H"
#include "x/serialize.H"
#include "x/deserialize.H"
#include <poll.h>
#include <unistd.h>
#include <algorithm>
#include <pwd.h>

LOG_CLASS_INIT(portmap_server);

static LIBCXX_NAMESPACE::property::value<size_t>
maxregs(LIBCXX_NAMESPACE_WSTR L"::httportmap::maxclient", 10);

struct portmap_server::clientinfoObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	clientinfoObj(const LIBCXX_NAMESPACE::fd &fdArg,
		      size_t pollfd_indexArg,
		      bool need_userArg);
	~clientinfoObj() noexcept;

	LIBCXX_NAMESPACE::fd fd;
	size_t pollfd_index;
	uid_t user;
	pid_t pid;
	std::string exe;

	bool need_user;
	std::vector<char> readbuf;
	size_t readbufp;

	std::multimap<std::string,
		      httportmap_server::s_iter> registered_services;

	std::vector<httportmap_server::entry> pending_registrations;

	template<typename iter_type>
	void serialize_toiter(iter_type &iter)
	{
		iter(user);
		iter(pid);
		iter(exe);
		iter(need_user);
		iter(readbuf);
		iter(readbufp);

		size_t s=registered_services.size();

		iter(s);

		for (auto service : registered_services)
		{
			iter(service.first);
			iter(*service.second);
		}
	}

	template<typename iter_type>
	void serialize_fromiter(iter_type &iter,
				httportmap_server::services_obj_t::lock &l)
	{
		iter(user);
		iter(pid);
		iter(exe);
		iter(need_user);
		iter(readbuf);
		iter(readbufp);

		size_t s;

		iter(s);

		while (s)
		{
			std::string connuuid;
			httportmap_server::entry connentry;

			iter(connuuid);
			iter(connentry);

			registered_services
				.insert(std::make_pair(connuuid,
						       l->insert(connentry)));
			--s;
		}
	}
};

portmap_server::clientinfoObj::clientinfoObj(const LIBCXX_NAMESPACE::fd &fdArg,
					     size_t pollfd_indexArg,
					     bool need_userArg)

	: fd(fdArg), pollfd_index(pollfd_indexArg), user(0), pid(0),
	  need_user(need_userArg)
{
	readbuf.resize(1024);
	readbufp=0;
}

portmap_server::clientinfoObj::~clientinfoObj() noexcept
{
}

portmap_server::portmap_server(httportmap_server &portmapArg)
	: portmap(portmapArg),
	  stop_pipe(LIBCXX_NAMESPACE::fd::base::pipe())
{
}

portmap_server::~portmap_server() noexcept
{
}

void portmap_server::run(reexec_fd_recvptr &restore)
{
	try {
		run2(restore);
	} catch (const x::exception &e)
	{
		LOG_FATAL(e);
	}
}

void portmap_server::run2(reexec_fd_recvptr &restore)
{
	std::vector<struct pollfd> fds;
	std::vector<clientinfo> clientinfos;
	std::vector<LIBCXX_NAMESPACE::fd> listenfds;

	fds.push_back(pollfd());

	fds[0].fd=stop_pipe.first->getFd();
	fds[0].events=POLLIN;

	listenfds.push_back(stop_pipe.first);

	for (std::list<LIBCXX_NAMESPACE::fd>::iterator
		     b(listen_sockets.begin()),
		     e(listen_sockets.end()); b != e; ++b)
	{
		try {
			(*b)->listen();
			(*b)->nonblock(true);
		}
		catch (const LIBCXX_NAMESPACE::exception &e)
		{
			LOG_ERROR(e);
			continue;
		}
		catch (...)
		{
			continue;
		}

		struct pollfd pfd;

		listenfds.push_back(*b);
		pfd.fd=(*b)->getFd();
		pfd.events=POLLIN;
		fds.push_back(pfd);
	}

	size_t nlistenSockets=fds.size();
	clientinfos.resize(nlistenSockets,
			   clientinfo::create(stop_pipe.first,
					      // Need a dummy fd here
					      0, 0));
	LOG_DEBUG("Listening for client connections");

	try
	{
		if (!restore.null())
		{
			LOG_DEBUG("Deserializing existing clients");

			httportmap_server::services_obj_t::lock
				lock(portmap.services);

			size_t n;

			reexec_fd_recvObj &recv= *restore;

			recv.iter(n);

			LOG_DEBUG("Restoring " << n << " client connections");

			while (n)
			{
				LIBCXX_NAMESPACE::fd fd=recv.recv_fd();

				newconn(fd, fds, clientinfos);

				clientinfo cl= *--clientinfos.end();

				cl->serialize_fromiter(recv.iter, lock);

				--n;
			}
			LOG_DEBUG("Restored all connections");
		}
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		LOG_ERROR(e);
	}

	restore=reexec_fd_recvptr();

	while (1)
	{
		if (poll(&fds[0], fds.size(), -1) < 0)
		{
			if (errno != EINTR)
				sleep(5);
			continue;
		}

		if (fds[0].revents & (POLLIN|POLLHUP|POLLERR))
		{
			LOG_DEBUG("Stop signal received");
			break;
		}

		for (size_t i=1; i < nlistenSockets; ++i)
		{
			if (!fds[i].revents & POLLIN)
				continue;

			try {
				const LIBCXX_NAMESPACE::fdptr
					conn(listenfds[i]->accept());

				if (conn.null())
					continue;
				conn->nonblock(true);
				conn->recv_credentials(true);

				newconn(conn, fds, clientinfos);
				LOG_DEBUG("New client connection: " <<
					  conn->getFd());
			} catch (const LIBCXX_NAMESPACE::exception &e)
			{
				LOG_ERROR(e);
				continue;
			}
		}

		for (size_t i=fds.size(); i > nlistenSockets; )
		{
			--i;

			try {
				if (fds[i].revents & POLLIN)
				{
					if (handle_client(*clientinfos[i],
							  clientinfos))
						continue;
				}
				else if ((fds[i].revents & (POLLHUP|POLLERR))
					 == 0)
					continue;
			} catch (const LIBCXX_NAMESPACE::exception &e)
			{
				LOG_ERROR("Client connection " <<
					  fds[i].fd << ": " << e);
			}

			LOG_DEBUG("Closing client connection " <<
				  fds[i].fd);

			size_t n=fds.size()-1;

			for (std::multimap<std::string,
					   httportmap_server::s_iter>::
				     const_iterator b(clientinfos[i]
						      ->registered_services
						      .begin()),
				     e(clientinfos[i]->registered_services
				       .end()); b != e; ++b)
			{
				httportmap_server::services_obj_t::lock
					lock(portmap.services);

				portmap.deregistersvc(b->second, lock);
			}

			if (n != i)
			{
				clientinfos[n]->pollfd_index=i;
				clientinfos[i]=clientinfos[n];
				fds[i]=fds[n];
			}

			clientinfos.pop_back();
			fds.pop_back();
		}
	}
	LOG_DEBUG("Terminating");
}

void portmap_server::newconn(const x::fd &conn,
			     std::vector<struct pollfd> &fds,
			     std::vector<clientinfo> &clientinfos)
{
	struct pollfd pfd;

	pfd.fd=conn->getFd();
	pfd.events=POLLIN;
	pfd.revents=0;

	clientinfo cl(clientinfo::create(conn, fds.size(), true));

	fds.push_back(pfd);
	clientinfos.push_back(cl);
}

bool portmap_server::handle_client(clientinfoObj &cl,
				   std::vector<clientinfo> &clientinfos)
{
	if (cl.need_user)
	{
		LOG_DEBUG("Receiving credentials from client connection "
			  << cl.fd->getFd());

		LIBCXX_NAMESPACE::fd::base::credentials_t uc;

		if (!cl.fd->recv_credentials(uc))
			return true;

		cl.user=uc.uid;
		cl.pid=uc.pid;
		cl.need_user=false;

		{
			httportmap_server::pid2exe_proc_t::lock
				lock(portmap.pid2exe_proc);

			(**lock) << cl.pid << std::endl << std::flush;

			std::getline(**lock, cl.exe);
		}

		LOG_DEBUG("Client connection "
			  << cl.fd->getFd()
			  << ": pid "
			  << cl.pid
			  << ", userid "
			  << cl.user
			  << ", exe "
			  << cl.exe);

		// If there's an existing entry for the same pid, but it has
		// a different exe, remove that mapping. The other connection
		// must've forked, and is running under a different pid.

		httportmap_server::services_obj_t::lock
			lock(portmap.services);

		for (auto &svc: *lock)
		{
			if (svc.pid == cl.pid &&
			    svc.exe.size() > 0 &&
			    svc.exe != cl.exe)
			{
				svc.exe="";
			}
		}
		return true;
	}

	ssize_t n=cl.fd->read(&cl.readbuf[cl.readbufp],
			      cl.readbuf.size() - cl.readbufp);

	LOG_DEBUG("Read " << n << " bytes from client connection "
		  << cl.fd->getFd());

	if (n == 0)
	{
		if (errno == 0)
			return false;
		return true;
	}

	cl.readbufp += n;

	if (cl.readbufp > 0 && cl.readbuf[cl.readbufp-1] == '\n')
	{
		LOG_DEBUG("Client connection " << cl.fd->getFd() << ": "
			  << std::string(&cl.readbuf[0],
					 &cl.readbuf[cl.readbufp-1]));

		std::list<std::string> cmd;

		LIBCXX_NAMESPACE::strtok_str(std::string(&cl.readbuf[0],
						       &cl.readbuf[cl
								   .readbufp-1]
						       ), "\t",
					   cmd);
		cl.readbufp=0;

		if (cmd.empty())
			return true;

		std::string w(cmd.front());

		cmd.pop_front();

		if (w == "SVC")
		{
			httportmap_server::entry newentry;

			newentry.exclusive=false;
			newentry.user=cl.user;
			newentry.pid=cl.pid;

			if (!cmd.empty())
			{
				std::string word=cmd.front();

				cmd.pop_front();

				if (word.find('X') != std::string::npos)
					newentry.exclusive=true;
				if (word.find('P') != std::string::npos)
					newentry.exe=cl.exe;
			}

			if (!cmd.empty())
			{
				newentry.service=cmd.front();
				cmd.pop_front();
			}

			if (!cmd.empty())
			{
				newentry.port=cmd.front();
				cmd.pop_front();
			}

			if (cl.pending_registrations.size() <=
			    maxregs.getValue())
				cl.pending_registrations.push_back(newentry);


			if (cl.fd->write("+Ok\n", 4) != 4)
			{
				LOG_DEBUG("Client connection "
					  << cl.fd->getFd()
					  << ": write failed");
				return false;
			}
			return true;
		}

		if (w == "REG")
		{
			std::string connuuid;

			if (!cmd.empty())
			{
				connuuid=cmd.front();
				cmd.pop_front();
			}

			std::string resp="-Registration failed\n";

			httportmap_server::services_obj_t::lock
				lock(portmap.services);

			if (cl.pending_registrations.size() +
			    cl.registered_services.size() <=
			    maxregs.getValue()
			    &&
			    portmap.regsvcs(cl.pending_registrations.begin(),
					    cl.pending_registrations.end(),
					    cl.registered_services,
					    lock, connuuid))
			{
				resp="+Registered\n";
			}

			for (std::vector<httportmap_server::entry>
				     ::const_iterator
				     b(cl.pending_registrations.begin())
				     ,
				     e(cl.pending_registrations.end());
			     b != e; ++b)
			{
				LOG_DEBUG("Client connection "
					  << cl.fd->getFd()
					  << ": register "
					  << b->service
					  << ", port "
					  << b->port
					  << (b->exclusive ?
					      " (exclusive)":"")
					  << ": " << resp);
			}

			cl.pending_registrations.clear();

			if (cl.fd->write(resp.c_str(), resp.size())
			    != resp.size())
			{
				LOG_DEBUG("Client connection "
					  << cl.fd->getFd()
					  << ": write failed");
				return false;
			}
		}

		if (w == "DEREG")
		{
			std::string service;
			std::string connuuid;

			if (!cmd.empty())
			{
				connuuid=cmd.front();
				cmd.pop_front();
			}

			if (!cmd.empty())
			{
				service=cmd.front();
				cmd.pop_front();
			}

			if (!cmd.empty())
			{
				std::string port=cmd.front();

				httportmap_server::services_obj_t::lock
					lock(portmap.services);

				typedef std::multimap<std::string,
						      httportmap_server
						      ::s_iter>
					::iterator iter_t;

				for (std::pair<iter_t, iter_t>
					     iter=cl.registered_services
					     .equal_range(connuuid);
				     iter.first != iter.second; )
				{
					iter_t p=iter.first;

					++iter.first;

					if (p->second->port == port &&
					    p->second->service == service)
					{
						portmap.deregistersvc
							(p->second, lock);

						cl.registered_services
							.erase(p);
						break;
					}
				}

				LOG_DEBUG("Client connection "
					  << cl.fd->getFd()
					  << ": deregister "
					  << service
					  << ", port "
					  << port);
			}
			if (cl.fd->write("+Ok\n", 4) != 4)
			{
				LOG_DEBUG("Client connection "
					  << cl.fd->getFd()
					  << ": write failed");
				return false;
			}
		}


		if (w == "DROP")
		{
			std::string connuuid;

			if (!cmd.empty())
			{
				connuuid=cmd.front();
				cmd.pop_front();

				httportmap_server::services_obj_t::lock
					lock(portmap.services);

				typedef std::multimap<std::string,
						      httportmap_server
						      ::s_iter>
					::iterator iter_t;

				for (std::pair<iter_t, iter_t>
					     iter=cl.registered_services
					     .equal_range(connuuid);
				     iter.first != iter.second; )
				{
					iter_t p=iter.first;

					++iter.first;

					portmap.deregistersvc(p->second, lock);
					cl.registered_services.erase(p);
				}
			}

			if (cl.fd->write("+Ok\n", 4) != 4)
			{
				LOG_DEBUG("Client connection "
					  << cl.fd->getFd()
					  << ": write failed");
				return false;
			}
		}

		if (w == "REEXEC")
		{
			if (cl.user && cl.user != geteuid())
				return false;

			if (cmd.empty() || cmd.front() !=
			    HTTPORTMAP_SERVER_VERSION)
				return false;

			try {
				cl.fd->nonblock(false);

				reexec_fd_send reexec(cl.fd, cl.fd);


				{
					LIBCXX_NAMESPACE::rwlock::base::wlock
						wlock=portmap.request_lock
						->writelock();

					portmap.intern_priv_listener->stop();
					portmap.intern_listener->stop();
				}

				portmap.intern_priv_listener->wait();
				portmap.intern_listener->wait();

				if (!portmap.extern_listener.null())
				{
					portmap.extern_listener->stop();
					portmap.extern_listener->wait();
				}

				portmap.meta.serialize(reexec);

				httportmap_server::services_obj_t::lock
					lock(portmap.services);

				// Don't serialize this connection

				size_t n=clientinfos.size()-1;

				reexec.iter(n);

				for (auto ci:clientinfos)
				{
					if (ci->fd->getFd() == cl.fd->getFd())
						continue;
					reexec.send_fd(ci->fd);
					ci->serialize_toiter(reexec.iter);
				}
				reexec.out_iter.flush();
				_exit(0);
			} catch (const LIBCXX_NAMESPACE::exception &e)
			{
				LOG_FATAL(e);
				cl.fd->nonblock(true);
				return false;
			}
		}

		return true;
	}

	return (cl.readbufp < cl.readbuf.size());
}

void portmap_server::stop()
{
	stop_pipe.second->close();
}

