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
maxregs(LIBCXX_NAMESPACE_STR "::httportmap::maxclient", 10);

struct portmap_server::clientinfoObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	clientinfoObj(const LIBCXX_NAMESPACE::fd &fdArg,
		      size_t pollfd_indexArg,
		      char stateArg);
	~clientinfoObj();

	LIBCXX_NAMESPACE::fd fd;
	size_t pollfd_index;
	uid_t user;
	pid_t pid;
	std::string exe;
	std::string start_time;

	char state;

	static const char state_client=0;
	static const char state_need_cred=1;
	static const char state_need_exe=2;
	static const char state_need_cred2=3;

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
		iter(start_time);
		iter(state);
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
		iter(start_time);
		iter(state);
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
					     char stateArg)

	: fd(fdArg), pollfd_index(pollfd_indexArg), user(0), pid(0),
	  state(stateArg)
{
	readbuf.resize(1024);
	readbufp=0;
}

portmap_server::clientinfoObj::~clientinfoObj()
{
}

portmap_server::portmap_server(httportmap_server &portmapArg)
	: portmap(portmapArg),
	  stop_pipe(LIBCXX_NAMESPACE::fd::base::pipe())
{
}

portmap_server::~portmap_server()
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

	fds[0].fd=stop_pipe.first->get_fd();
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
		pfd.fd=(*b)->get_fd();
		pfd.events=POLLIN;
		fds.push_back(pfd);
	}

	size_t nlistenSockets=fds.size();

	clientinfos.resize(nlistenSockets,
			   clientinfo::create(stop_pipe.first, 0,
					      // Need a dummy fd here
					      (char)clientinfoObj
					      ::state_client));
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
					  conn->get_fd());
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
					if (handle_client(fds[i],
							  *clientinfos[i],
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

	pfd.fd=conn->get_fd();
	pfd.events=POLLIN;
	pfd.revents=0;

	clientinfo cl(clientinfo::create(conn, fds.size(),
					 (char)(clientinfoObj
						::state_need_cred)));

	fds.push_back(pfd);
	clientinfos.push_back(cl);
}

void portmap_server::install_client(clientinfoObj &cl)
{
	LOG_DEBUG("Client connection "
		  << cl.fd->get_fd()
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
}

void portmap_server::send_client_fd(struct pollfd &pfd,
				    clientinfoObj &cl)
{
	auto newsock=LIBCXX_NAMESPACE::fd::base::socketpair();

	cl.fd->send_fd(newsock.first);

	newsock.second->nonblock(true);
	newsock.second->recv_credentials(true);
	pfd.fd=newsock.second->get_fd();
	cl.fd=newsock.second;
}

bool portmap_server::handle_client(struct pollfd &pfd,
				   clientinfoObj &cl,
				   std::vector<clientinfo> &clientinfos)
{
	switch (cl.state) {
	default:
		break;
	case cl.state_need_cred:
		LOG_DEBUG("Receiving credentials from client connection "
			  << cl.fd->get_fd());

		{
			LIBCXX_NAMESPACE::fd::base::credentials_t uc;

			if (!cl.fd->recv_credentials(uc))
				return true;

			cl.user=uc.uid;
			cl.pid=uc.pid;
		}

#if HAVE_SYSCTL_KERN_PROC
		cl.state=cl.state_need_exe;
		if (cl.fd->write("\n", 1) != 1)
		{
			LOG_DEBUG("Client connection "
				  << cl.fd->get_fd()
				  << ": write failed");
			return false;
		}
#else
		{
			httportmap_server::pid2exe_proc_t::lock
				lock(portmap.pid2exe_proc);

			(**lock) << cl.pid << std::endl << std::flush;

			portmap.parse_pid2exeproc_response(lock, cl.start_time,
							   cl.exe);
		}
		cl.state=cl.state_need_cred2;

		install_client(cl);
		send_client_fd(pfd, cl);
#endif
		return true;
	case cl.state_need_cred2:
		LOG_DEBUG("Receiving credentials from client connection "
			  << cl.fd->get_fd() << " again");

		{
			LIBCXX_NAMESPACE::fd::base::credentials_t uc;

			if (!cl.fd->recv_credentials(uc))
				return true;

			if (cl.pid != uc.pid)
			{
				LOG_ERROR("Client connection changed from pid "
					  << cl.pid << " to "
					  << uc.pid);
				return false;
			}

			httportmap_server::pid2exe_proc_t::lock
				lock(portmap.pid2exe_proc);

			(**lock) << cl.pid
#if HAVE_SYSCTL_KERN_PROC
				 << ' ' << cl.exe
#endif
				 << std::endl << std::flush;

			std::string now_start_time;
			std::string now_exe;

			portmap.parse_pid2exeproc_response(lock, now_start_time,
							   now_exe);

			if (cl.start_time != now_start_time ||
			    cl.exe != now_exe)
			{
				LOG_ERROR("Client connection "
					  << cl.fd->get_fd()
					  << ": " << cl.exe
					  << ": credentials have changed");
				return false;
			}
		}
		cl.state=cl.state_client;
		if (cl.fd->write("\n", 1) != 1)
		{
			LOG_DEBUG("Client connection "
				  << cl.fd->get_fd()
				  << ": write failed");
			return false;
		}
		return true;
	}

	ssize_t n=cl.fd->read(&cl.readbuf[cl.readbufp],
			      cl.readbuf.size() - cl.readbufp);

	LOG_DEBUG("Read " << n << " bytes from client connection "
		  << cl.fd->get_fd());

	if (n == 0)
	{
		if (errno == 0)
			return false;
		return true;
	}

	cl.readbufp += n;

	if (cl.readbufp > 0 && cl.readbuf[cl.readbufp-1] == '\n')
	{
		std::string line(&cl.readbuf[0],
				 &cl.readbuf[cl.readbufp-1]);
		cl.readbufp=0;

#if HAVE_SYSCTL_KERN_PROC
		if (cl.state == cl.state_need_exe)
		{
			httportmap_server::pid2exe_proc_t::lock
				lock(portmap.pid2exe_proc);

			(**lock) << cl.pid << ' ' << line
				 << std::endl << std::flush;

			portmap.parse_pid2exeproc_response(lock, cl.start_time,
							   cl.exe);

			cl.state=cl.state_need_cred2;
			install_client(cl);
			send_client_fd(pfd, cl);
			return true;
		}
#endif
		return handle_client_line(cl, clientinfos, line);
	}

	return (cl.readbufp < cl.readbuf.size());
}

bool portmap_server::handle_client_line(clientinfoObj &cl,
					std::vector<clientinfo> &clientinfos,
					const std::string &line)
{
	LOG_DEBUG("Client connection " << cl.fd->get_fd() << ": " << line);

	std::list<std::string> cmd;

	LIBCXX_NAMESPACE::strtok_str(line, "\t", cmd);
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
		    maxregs.get())
			cl.pending_registrations.push_back(newentry);


		if (cl.fd->write("+Ok\n", 4) != 4)
		{
			LOG_DEBUG("Client connection "
				  << cl.fd->get_fd()
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
		    maxregs.get()
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
				  << cl.fd->get_fd()
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
				  << cl.fd->get_fd()
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
				  << cl.fd->get_fd()
				  << ": deregister "
				  << service
				  << ", port "
				  << port);
		}
		if (cl.fd->write("+Ok\n", 4) != 4)
		{
			LOG_DEBUG("Client connection "
				  << cl.fd->get_fd()
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
				  << cl.fd->get_fd()
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
				LIBCXX_NAMESPACE::sharedlock::base::unique
					unique=portmap.request_lock
					->create_unique();

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
				if (ci->fd->get_fd() == cl.fd->get_fd())
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

void portmap_server::stop()
{
	stop_pipe.second->close();
}
