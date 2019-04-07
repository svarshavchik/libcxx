/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_httportmapobj_H
#define x_httportmapobj_H

#include <x/httportmapfwd.H>
#include <x/http/useragent.H>
#include <x/fd.H>
#include <x/fdtimeoutconfig.H>
#include <x/ptr.H>
#include <x/csv.H>
#include <x/pwd.H>
#include <x/netaddr.H>
#include <x/uuid.H>
#include <x/namespace.h>
#include <map>
#include <vector>
#include <iterator>
#include <set>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! An HTTP portmapper object

//! \see httportmap
//!

class httportmapObj : virtual public obj {

public:

	class daemonConnObj;

private:
	//! HTTP useragent used by this portmapper
	http::useragent ua;

	//! Server's name
	std::string server;

	//! The connection to the local portmapper daemon

	ptr<daemonConnObj> daemon;

	class clock;

	//! Server's port.

	int server_port;

	//! This httportmap client's unique identifier
	x::uuid uuid;

public:
	friend class lock;

	//! Service registration info

	struct reginfo {

		//! Service name

		std::string name;

		//! Service port

		std::string port;

		//! Registration flags
		int flags;
	};

	//! Application service information

	//! This class should be referenced as "httportmap::base::service".
	//! Services are keyed by a name, and the running user.
	//! A comparator is defined for both the entire service object and
	//! its key part, allowing these objects to be used with associative
	//! container.

	class service {

		//! Service key

		struct key_t {

			//! Application name
			std::string name;

			//! Running user
			uid_t user;

			//! Comparison operator

			bool operator<(const key_t &s) const noexcept
			{
				int val=name.compare(s.name);

				if (val)
					return val < 0;

				return user < s.user;
			}
		};

		//! The key for this application
		key_t key;

		//! Application's pid
		pid_t pid;

		//! Application's port
		std::string port;

		//! Executable name, if published
		std::string path;

	public:
		friend class httportmapObj;

		//! Comparison operator
		bool operator<(const service &s) const noexcept
		{
			return key < s.key;
		}

		//! Return the service's application name

		const std::string &getName() const noexcept
		{
			return key.name;
		}

		//! Return the running userid
		const uid_t getUser() const noexcept
		{
			return key.user;
		}

		//! Return the application's pid

		pid_t getPid() const noexcept
		{
			return pid;
		}

		//! Return the application's port
		const std::string &getPort() const noexcept
		{
			return port;
		}

		//! Return the application's executable filename
		const std::string &getPath() const noexcept
		{
			return path;
		}
	};

	//! Create a portmapper object

	httportmapObj(//! Server's name (empty string for this server)
		      const std::string &server="",

		      //! Server's HTTP port
		      int server_port=getDefaultPort());

	//! Create a portmapper object

	httportmapObj(//! A non-default user agent to use
		      const http::useragent &uaArg,

		      //! Server's name (empty string for this server)
		      const std::string &server="",

		      //! Server's HTTP port
		      int server_port=getDefaultPort());

	//! Return default port

	static int getDefaultPort();

	//! Destructor
	~httportmapObj();

	//! Enumerate applications on the referenced server

	//! This template function takes an output iterator for
	//! \ref httportmapBase::service "httportmap::base::service"
	//! objects and puts the enumerated services into the output iterator
	//!
	//! \return the output iterator value after all the services have been
	//! enumerated.

	template<typename output_iter_t>
	output_iter_t list(//! httportmap::service output iterator
			   output_iter_t iter,

			   //! Enumerate only the following set of services
			   const std::set<std::string> &service_list,

			   //! Enumerate only services of the following users
			   const std::set<uid_t> &users_list,

			   //! Enumerate only services from these pids
			   const std::set<pid_t> &pid_list,

			   //! Optional timeout file descriptor
			   const fdptr &timeoutfd=fdptr()
			   ) const
	{
		http::useragent::base::response resp=
			this->request_csv_list(timeoutfd, service_list,
					       users_list, pid_list);

		auto b=resp->begin(), e=resp->end();

		if (b == e)
			return iter;

		std::map<std::string, size_t> colmap;

		b=csvheaderrow(b, e, colmap);

		if (b == e || *b++ != '\n')
			badresponse();

		while (b != e)
		{
			std::vector<std::string> columns;

			b=fromcsv(b, e, columns);

			if (b == e || *b++ != '\n')
				badresponse();

			*iter++=this->init_service(columns, colmap);
		}

		return iter;
	}

	//! Enumerate applications on the referenced server

	//! \overload
	//!
	template<typename output_iter_t>
	output_iter_t list(output_iter_t iter,
			   const std::string &service,
			   uid_t user,
			   const fdptr &timeoutfd=fdptr()
			   )
		const
	{
		std::set<std::string> s;
		std::set<uid_t> u;
		std::set<pid_t> p;

		s.insert(service);
		u.insert(user);
		return list(iter, s, u, p, timeoutfd);
	}

	//! Enumerate applications on the referenced server

	//! \overload
	//!
	template<typename output_iter_t>
	output_iter_t list(output_iter_t iter,
			   const std::string &service,
			   const fdptr &timeoutfd=fdptr()
			   )
		const
	{
		return list(iter, service, getuid(), timeoutfd);
	}

	//! Enumerate applications on the referenced server

	//! \overload
	//!
	template<typename output_iter_t>
	output_iter_t list(output_iter_t iter,
			   const fdptr &timeoutfd=fdptr()
			   )
		const
	{
		std::set<std::string> dummy;
		std::set<uid_t> dummy2;
		std::set<pid_t> p;

		return list(iter, dummy, dummy2, p, timeoutfd);
	}

private:
	//! Send a request for a CSV-formatted service list

	http::useragent::base::response
	request_csv_list(//! Timeout file descriptor for this request
			 const fdptr &timeoutfd,

			 //! Service list requested
			 const std::set<std::string> &service_list,

			 //! User list requested
			 const std::set<uid_t> &users_list,

			 //! PID list requested
			 const std::set<pid_t> &pid_list)
		const;

	//! Parse CSV row into an \c httportmap::service object

	static service
	init_service(//! CSV row
		     const std::vector<std::string>  &,

		     //! CSV header
		     const std::map<std::string, size_t> &);

public:
	//! Register a service with the portmapper

	//! This portmapper object must be instantiated for the current host.
	//! The service remains registered until this portmapper object goes
	//! out of scope.
	//!
	//! \return \c true if the service could be registered, or \c false
	//! if the exclusive service is already registered.

	bool reg(//! Service's name.

		 //! The name of the service cannot contain spaces or control
		 //! characters. By convention, the service name must be
		 //! formatted as a valid domain name or email address,
		 //! "hostname.example.com" or "user@hostname.example.com".
		 //! Implementation that use this portmapper service should
		 //! only use domain names that they own, to avoid clashing with
		 //! other portmapper services that use the same naming
		 //! convention.

		 const std::string &svc,

		 //! Service's port.

		 //! The service port cannot contain spaces or control
		 //! characters. The service port should either be a number,
		 //! referencing a listening socket, or an absolute pathname
		 //! that refers to an \c AF_UNIX listening socket
		 const std::string &port,

		 //! Registration flags

		 int flags,

		 //! Optional timeout file descriptor

		 //! If this file descriptor become readable, this function
		 //! times out.

		 const fdptr &timeoutfd=fdptr()
		 );

	//! Register a service with the portmapper

	//! \overload

	bool reg(//! Service's name

		 const std::string &svc,

		 //! Service's port
		 int port,

		 //! Registration flags

		 int flags,

		 //! Optional timeout file descriptor

		 //! If this file descriptor become readable, this function
		 //! times out

		 const fdptr &timeoutfd=fdptr()
		 );

	//! Register a service with the portmapper

	//! \overload

	bool reg(//! Service's name

		 const std::string &svc,

		 //! The service's port.

		 //! This is a socket file descriptor. The service's port
		 //! is retrieved directly from the socket.

		 const fd &fdArg,

		 //! Registration flags

		 int flags,

		 //! Optional timeout file descriptor

		 //! If this file descriptor become readable, this function
		 //! times out

		 const fdptr &timeoutfd=fdptr()
		 );

	//! Register a service with the portmapper

	//! \overload

	bool reg(//! Service's name

		 const std::string &svc,

		 //! A list of file descriptors being listened on

		 //! This list must contain only AF_INET and AF_INET6
		 //! sockets. Their list of ports is deduped, and
		 //! registered as a comma-separated list.

		 const std::list<fd> &fdList,

		 //! Registration flags
		 int flags,

		 //! Optional timeout file descriptor

		 //! If this file descriptor become readable, this function
		 //! times out.

		 const fdptr &timeoutfd=fdptr()
		 );

	//! Register a service with the portmapper

	//! \overload

	bool reg(//! A list of ports and flags

		 const std::list<reginfo> &ports,

		 //! Optional timeout file descriptor

		 //! If this file descriptor become readable, this function
		 //! times out

		 const fdptr &timeoutfd=fdptr()
		 );

	//! Deregister a previously-registered service

	void dereg(//! Previously-registered service name

		   const std::string &svc,

		   //! Previously-registered port

		   const std::string &port,

		   //! Optional timeout file descriptor

		   //! If this file descriptor become readable, this function
		   //! times out

		   const fdptr &timeoutfd=fdptr()
		   )
;

	//! Deregister a previously-registered service

	//! \overload
	//!
	void dereg(//! Previously-registered service name

		   const std::string &svc,

		   //! Previously-registered port.
		   int port,

		   //! Optional timeout file descriptor

		   //! If this file descriptor become readable, this function
		   //! times out

		   const fdptr &timeoutfd=fdptr()
		   );

	//! Deregister a previously-registered service

	//! \overload
	//!
	void dereg(//! Previously-registered service name.

		   const std::string &svc,

		   //! Previously-regsitered socket.

		   //! The service's port is retrieved directly from the socket.

		   const fd &fdArg,

		   //! Optional timeout file descriptor

		   //! If this file descriptor become readable, this function
		   //! times out

		   const fdptr &timeoutfd=fdptr()
		   );

	//! Deregister a previously-registered service

	//! \overload
	//!
	void dereg(//! Previously-registered service name

		   const std::string &svc,

		   //! Previously-registered sockets.

		   //! This list must contain only AF_INET and AF_INET6
		   //! sockets. Their list of ports is deduped, and
		   //! deregistered.

		   const std::list<fd> &fdList,

		   //! Optional timeout file descriptor

		   //! If this file descriptor become readable, this function
		   //! times out

		   const fdptr &timeoutfd=fdptr()
		   );

	//! Announce which pathname this process is running

	//! This is implemented by registering an internal service
	//!
	//! \note
	//! Long-running daemons should invoke this when they start, and
	//! keep the portmapper connection running. A prelink(1) may result
	//! in the process /proc/self/exe becoming undefined, when prelink(1)
	//! replaces the binary, so this must be done during startup.

	void regpid2exe(//! Optional timeout file descriptor

			//! If this file descriptor become readable, this function
			//! times out

			const fdptr &timeoutfd=fdptr());

	//! Stop announcing which pathname this process is running

	//! The cat is out of the bag, however.

	void deregpid2exe(//! Optional timeout file descriptor

			  //! If this file descriptor become readable, this function
			  //! times out

			  const fdptr &timeoutfd=fdptr());

	//! Retrieve a registered exe for the given pid

	//! \return the pathname the given process is running. An empty string
	//! gets returned if the other process did not call regpidexe().

	std::string pid2exe(//! PID to map to an path
			    pid_t pid,

			    //! Optional timeout file descriptor

			    //! If this file descriptor become readable, this function
			    //! times out

			    const fdptr &timeoutfd=fdptr());

private:
	//! Throw an %exception
	static void nosuchuser(uid_t u)
		__attribute__((noreturn));

	//! Throw an %exception
	static void badresponse()
		__attribute__((noreturn));
public:

	//! Connect to the specified service

	//! The connection timeout is given using a timeout file descriptor.
	//! The timeout file descriptor is used for both the connection to the
	//! portmap service.

	fd connect(//! Service's name
		   const std::string &name,

		   //! Service must be running as this user
		   uid_t user,

		   //! The connection times out when this file descriptor becomes readable
		   const fdptr &timeoutfd=fdptr());


	//! Connect to the specified service which is running as root

	fd connect_root(//! Service name
			const std::string &name,

			//! The connection times out when this file descriptor becomes readable
			const fdptr &timeoutfd=fdptr())
	{
		return connect(name, 0, timeoutfd);
	}

	//! Connect to the specified service running as any user

	//! \overload

	fd connect_any(//! Service name
		       const std::string &name,

		       //! The connection times out when this file descriptor becomes readable
		       const fdptr &timeoutfd=fdptr());

private:

	//! Throw an internal %exception

	//! \internal
	//!
	static void badclient() __attribute__((noreturn));

	//! Throw an internal %exception

	//! \internal
	//!
	static void badsvcport() __attribute__((noreturn));
private:
	//! Make a connection attempt after obtaining an enumerated list of services

	//! \internal
	//!
	fd do_connect(std::vector<service> &entries,
		      const fdptr &timeoutfd,
		      const std::string &servicename)
;
};

#ifndef DOXYGEN
#define LIBCXX_TEMPLATE_DECL extern
#include <x/httportmap_t.H>
#undef LIBCXX_TEMPLATE_DECL
#endif

#if 0
{
#endif
}
#endif
