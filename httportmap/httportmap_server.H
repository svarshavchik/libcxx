/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef httportmap_server_H
#define httportmap_server_H

#include <string>
#include <pwd.h>
#include <set>
#include <map>

#include "x/mpobj.H"
#include "x/fd.H"
#include "x/fdlistener.H"
#include "x/pwd.H"
#include "x/uuid.H"
#include "x/rwlock.H"
#include "x/threads/run.H"
#include "reexecfd.H"

class httpserver;

class portmap_server;

#define HTTPORTMAP_SERVER_VERSION "1"

class httportmap_server {

	std::mutex stop_mutex;

	LIBCXX_NAMESPACE::const_fdptr stop_filedesc;

public:
	class startup_metainfo {

	public:
		std::string portmapclient_socket;

		std::map<int, std::list<LIBCXX_NAMESPACE::fd> > sockets;

		static const int sock_lockfile=0;
		static const int sock_pub_listen=1;
		static const int sock_priv_listen=2;
		static const int sock_http=3;
		static const int sock_portmap=4;

		void serialize(reexec_fd_send &iter);
		void serialize(reexec_fd_recvObj &iter);
	};

	startup_metainfo meta;

	LIBCXX_NAMESPACE::ptr<httpserver> intern_server, extern_server,
		intern_priv_server;
	LIBCXX_NAMESPACE::ptr<portmap_server> portmap_thread;
	LIBCXX_NAMESPACE::runthreadptr<void> portmap_thread_run;

	LIBCXX_NAMESPACE::fdlistenerptr intern_priv_listener, intern_listener,
		extern_listener;

private:
	void startup(const std::string &sockname,
		     const std::string &portmapdatadirArg,
		     int httpport,
		     bool daemonize);

	void startup_frommeta(bool daemonize,
			      const reexec_fd_recvptr &reexec_ptr);

	static x::iostream pid2exe_thread();

public:
	std::string portmapdatadir;

	typedef LIBCXX_NAMESPACE::mpobj<
		LIBCXX_NAMESPACE::iostream> pid2exe_proc_t;

	pid2exe_proc_t pid2exe_proc;

	// Lock used during reexec to hold processing of all requests.
	LIBCXX_NAMESPACE::rwlock request_lock;
				
	httportmap_server(const std::string &sockname,
			  const std::string &portmapdatadir,
			  int httpport,
			  bool daemonize);
	void wait();
	~httportmap_server() noexcept;

public:

	void stop(const LIBCXX_NAMESPACE::const_fd &filedescArg)
	{
		// This file descriptor sent the stop command

		// Save a reference to it. This last reference will get
		// destroyed after the lockfile is closed as part of this
		// object's destructor. Therefore, the network connection that
		// sent the stop command will be closed only after the lock file
		// is released and, for all practical purposes, the portmapper
		// is terminated.

		{
			std::lock_guard<std::mutex> lock(stop_mutex);

			stop_filedesc=filedescArg;
		}

		if (!extern_listener.null())
			extern_listener->stop();

		if (!intern_listener.null())
			intern_listener->stop();

		if (!intern_priv_listener.null())
			intern_priv_listener->stop();
	}

	class entry {

	public:
		std::string service;
		uid_t user;
		pid_t pid;
		mutable std::string exe;

		std::string port;
		bool exclusive;

		bool operator<(const entry &k) const
		{
			int n=service.compare(k.service);

			if (n < 0)
				return true;

			if (n > 0)
				return false;

			return user < k.user;
		}

		template<typename output_iter>
		output_iter values(output_iter iter) const
		{
			*iter++ = service;

			{
				std::ostringstream o;

				o << user;

				*iter++ = o.str();
			}

			{
				std::ostringstream o;

				o << pid;

				*iter++ = o.str();
			}

			*iter++ = exe;
			*iter++ = port;
			return iter;
		}

		template<typename output_iter>
		static output_iter names(output_iter iter)
		{
			*iter++ = "service";
			*iter++ = "user";
			*iter++ = "pid";
			*iter++ = "prog";
			*iter++ = "port";
			return iter;
		}

		template<typename iter_type> void serialize(iter_type &iter)
		{
			iter(service);
			iter(user);
			iter(pid);
			iter(exe);
			iter(port);
			iter(exclusive);
		}
	};

	typedef std::multiset<entry> services_t;

	typedef LIBCXX_NAMESPACE::mpobj<services_t> services_obj_t;

	services_obj_t services;

	typedef services_t::iterator s_iter;

	class iterator : private services_t::const_iterator,
			 public std::iterator<std::bidirectional_iterator_tag,
					      entry> {

		services_obj_t::lock &l;

	public:
		friend class httportmap_server;

	private:
		iterator(services_obj_t::lock &lArg,
			 services_t::iterator iterArg)
			noexcept : services_t::const_iterator(iterArg), l(lArg)
		{
		}
	public:

		iterator(const iterator &o) noexcept :
			services_t::const_iterator(o), l(o.l)
		{
		}

		iterator &operator=(const iterator &o) noexcept
		{
			services_t::const_iterator::operator=(o);
			return *this;
		}

		using services_t::const_iterator::operator*;
		using services_t::const_iterator::operator->;

		bool operator==(const iterator &iter)
		{
			return services_t::const_iterator::operator==(iter);
		}

		bool operator!=(const iterator &iter)
		{
			return !operator==(iter);
		}

		iterator &operator++() noexcept
		{
			services_t::const_iterator::operator++();
			return *this;
		}

		const entry &operator++(int) noexcept
		{
			const entry &r(**this);

			operator++();
			return r;
		}

	};

	iterator begin(services_obj_t::lock &lArg) noexcept
	{
		return iterator(lArg, lArg->begin());
	}

	iterator end(services_obj_t::lock &lArg) noexcept
	{
		return iterator(lArg, lArg->end());
	}

	template<typename iter_type>
	bool regsvcs(iter_type beg_iter, iter_type end_iter,
		     std::multimap<std::string, s_iter> &iterlist_ret,
		     services_obj_t::lock &l,
		     const std::string &connuuid)
	{
		for (iter_type p=beg_iter; p != end_iter; ++p)
		{
			services_t::iterator iter=l->find(*p);

			if (iter != l->end() &&
			    (p->exclusive || iter->exclusive))
				return false;
		}

		for (iter_type p=beg_iter; p != end_iter; ++p)
		{
			if (p->exe.size() == 0)
				continue;

			// If an exe gets registered for a pid, and something
			// else has this pid registered to a different exe,
			// nuke it. It must be another process that forked,
			// and it has no claim to a different exe for its
			// former pid.

			for (auto &svc: *l)
			{
				if (svc.pid == p->pid &&
				    svc.exe.size() > 0 &&
				    svc.exe != p->exe)
				{

					svc.exe="";
				}
			}
		}

		for (iter_type p=beg_iter; p != end_iter; ++p)
			iterlist_ret
				.insert(std::make_pair(connuuid,
						       l->insert(*p)));

		return true;
	}

	void deregistersvc(s_iter iter, services_obj_t::lock &l)
	{
		l->erase(iter);
	}
};

#endif
