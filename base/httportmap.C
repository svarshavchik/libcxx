/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/httportmap.H"
#include "x/sysexception.H"
#include "x/sockaddr.H"
#include "x/messages.H"
#include "x/http/cgiimpl.H"
#include "x/property_value.H"
#include "x/http/useragent.H"
#include "x/servent.H"
#include "x/pidinfo.H"
#include "x/to_string.H"
#include "x/locale.H"
#include "gettext_in.h"

#include <sstream>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

property::value<int> LIBCXX_HIDDEN default_portmaport_prop(LIBCXX_NAMESPACE_STR
							   "::httportmap::port",
							   80);

const char httportmapBase::portmap_service[]="httportmap.libcxx";
const char httportmapBase::pid2exe_service[]="pid2exe.libcxx";

#define LIBCXX_TEMPLATE_DECL
#include "x/httportmap_t.H"
#undef LIBCXX_TEMPLATE_DECL

int httportmapObj::getDefaultPort()
{
	return default_portmaport_prop.get();
}

class LIBCXX_HIDDEN httportmapBase::regpid2exeObj : virtual public obj {

 public:

	httportmap instance;

	regpid2exeObj() : instance(httportmap::create())
	{
		instance->regpid2exe();
	}

	~regpid2exeObj() {}
};

singleton<httportmapBase::regpid2exeObj> LIBCXX_HIDDEN local_regpid2exe_instance;

httportmap httportmapBase::regpid2exe()
{
	return local_regpid2exe_instance.get()->instance;
}

// A singleton that contains a socket connection to the local portmapper's
// registration service.

class LIBCXX_HIDDEN httportmapObj::daemonConnObj : virtual public obj {

public:
	fdptr clientfd;

	friend class httportmapObj;

	daemonConnObj()
	{
	}

	~daemonConnObj()
	{
	}
};

singleton<httportmapObj::daemonConnObj> LIBCXX_HIDDEN local_daemon;

// A lock on the connection with the portmapper registration singleton

// Before instantating this object, acquire a mutex lock on the singleton.
// Instantiating this object copies the connection file descriptor
// object into this structure, then removes the file descriptor in the
// singleton. Before this object goes out of scope, the user must invoke
// check(), this puts the file descriptor back into the singleton.
//
// Otherwise, if an exception gets thrown, indicating some problem talking with
// the daemon, the singleton is left without a file descriptor for the
// connection, its left here, and gets closed.

class LIBCXX_HIDDEN httportmapObj::clock {

 public:
	fdptr origfd;

	iostream io;

	clock(const ref<daemonConnObj> &conn,
	      const fdptr &timeoutfd,
	      httportmapObj &obj)
		: origfd(conn->clientfd),
		io(
		   ({
			   fdptr t(origfd);

			   if (!timeoutfd.null())
			   {
				   fdtimeout timeout(fdtimeout::create(t));

				   timeout->set_terminate_fd(timeoutfd);

				   t=timeout;
			   }

			   t->getiostream();
		   })
		   )
		{
			conn->clientfd=fdptr();

			// An exception occuring after construction results in
			// automatically closing the client connection, until
			// succesfull completion, which restores origfd.
		}

	void check(const ref<daemonConnObj> &conn)
	{
		if (!io->good())
			throw SYSEXCEPTION("portmapper");

		conn->clientfd=origfd;
	}

	~clock()
	{
	}
};

httportmapObj::httportmapObj(const std::string &serverArg,
			     int server_portArg)
	: ua{http::useragent::base::global()},
	  server{serverArg}, server_port{server_portArg},
	  uuid_s{to_string(uuid{})}
{
}

httportmapObj::httportmapObj(const http::useragent &uaArg,
			     const std::string &serverArg,
			     int server_portArg)
	: ua(uaArg), server(serverArg), server_port(server_portArg)
{
}

httportmapObj::~httportmapObj()
{
	if (daemon.null())
		return;

	try {
		ref<daemonConnObj> conn(daemon);

		std::lock_guard<std::mutex> connlock(conn->objmutex);

		if (conn->clientfd.null())
			return;

		clock connection_lock(conn, fdptr(), *this);

		(*connection_lock.io) << "DROP\t" << uuid_s
				      << "\n" << std::flush;

		std::string resp;

		if (connection_lock.io->good())
			std::getline(*connection_lock.io, resp);

		connection_lock.check(conn);
	} catch (...)
	{
	}
}

http::useragent::base::response
httportmapObj::request_csv_list(const fdptr &timeoutfd,
				const std::set<std::string> &service_list,
				const std::set<uid_t> &users_list,
				const std::set<pid_t> &pid_list)
	const
{
	http::form::parameters params(http::form::parameters::create());

	for (auto service: service_list)
		params->insert(std::make_pair("service", service));

	for (auto uid: users_list)
	{
		params->insert(std::make_pair("user",
					      to_string(uid,
							locale::base::c())));
	}

	for (auto pid: pid_list)
	{
		params->insert(std::make_pair("pid",
					      to_string(pid,
							locale::base::c())));
	}

	http::useragent::base::response resp=
		ua->request(timeoutfd, http::POST,
			    ({
				    std::string o="http://"
					    + (server.size()
					       ? server:"localhost");

				    if (server_port !=
					servent("http", "tcp")->s_port)
				    {
					    o += ":";
					    o += to_string(server_port,
							   locale::base::c());
				    }
				    o += "/portmap";
				    o;
			    }),
			    "Connection", "close",
			    "Accept", "text/csv",
			    params);

	if (resp->message.get_status_code() != 200)
		throw EXCEPTION(resp->message.get_reason_phrase());

	return resp;
}

httportmap::base::service httportmapObj
::init_service(const std::vector<std::string> &row,
	       const std::map<std::string, size_t> &colmap)

{
	httportmap::base::service serv;

	std::map<std::string, size_t>::const_iterator
		service_val(colmap.find("service")),
		user_val(colmap.find("user")),
		pid_val(colmap.find("pid")),
		port_val(colmap.find("port")),
		path_val(colmap.find("prog"));

	if (service_val != colmap.end())
		serv.key.name=row[service_val->second];

	serv.key.user= (uid_t)-1;

	if (user_val != colmap.end())
	{
		std::istringstream i(row[user_val->second]);
		i >> serv.key.user;
	}

	serv.pid= (pid_t)-1;
	if (pid_val != colmap.end())
	{
		std::istringstream i(row[pid_val->second]);

		i >> serv.pid;
	}

	if (port_val != colmap.end())
		serv.port=row[port_val->second];

	if (path_val != colmap.end())
		serv.path=row[path_val->second];

	return serv;
}

bool httportmapObj::reg(const std::string &svc, const fd &fdArg,
			int flags, const fdptr &timeoutfd)

{
	std::string o;

	{
		sockaddr name(fdArg->getsockname());

		switch (name->family()) {
		case AF_INET:
		case AF_INET6:
			o = to_string(name->port(), locale::base::c());
			break;
		case AF_UNIX:
			o = name->address();
			break;
		default:
			return false;
		}
	}

	return reg(svc, o, flags, timeoutfd);
}

bool httportmapObj::reg(const std::string &svc,
			const std::list<fd> &fdList,
			int flags, const fdptr &timeoutfd)

{
	std::list<reginfo> ports;

	std::set<std::string> portset;

	for (std::list<fd>::const_iterator
		     b(fdList.begin()), e(fdList.end()); b != e; ++b)
	{
		reginfo newreg;

		newreg.name=svc;
		newreg.flags=flags;

		sockaddr name((*b)->getsockname());

		switch (name->family()) {
		case AF_INET:
		case AF_INET6:
			newreg.port=to_string(name->port(),
					      locale::base::c());
			break;
		case AF_UNIX:
			newreg.port=name->address();
			break;
		default:
			return false;
		}

		if (!portset.insert(newreg.port).second)
			continue; // Dupe

		ports.push_back(newreg);
	}

	return reg(ports, timeoutfd);
}

bool httportmapObj::reg(const std::string &svc, int port,
			int flags, const fdptr &timeoutfd)

{
	return reg(svc, to_string(port, locale::base::c()), flags, timeoutfd);
}

bool httportmapObj::reg(const std::string &svc, const std::string &port,
			int flags, const fdptr &timeoutfd)

{
	reginfo newreg;

	newreg.name=svc;
	newreg.port=port;
	newreg.flags=flags;

	std::list<reginfo> ports;

	ports.push_back(newreg);

	return reg(ports, timeoutfd);
}

void httportmapBase::connect_service(fd &newfd)
{
	char dummy[1];

	if (!newfd->send_credentials())
	{
		errno=ETIMEDOUT;
		throw SYSEXCEPTION("connect");
	}

#if HAVE_SYSCTL_KERN_PROC
	std::string me=exename();

	if (me.find('\n') != std::string::npos)
		throw EXCEPTION("My executable name contains newlines?");
	newfd->read(dummy, 1);
	me += "\n";
	newfd->write_full(me.c_str(), me.size());
#endif

	std::vector<fdptr> replfd;

	replfd.resize(1);
	newfd->recv_fd(replfd);

	if (replfd.empty())
		throw EXCEPTION("Failed to receive a socket from portmapper");

	newfd=replfd[0];
	if (!newfd->send_credentials())
	{
		errno=ETIMEDOUT;
		throw SYSEXCEPTION("connect");
	}

	newfd->read(dummy, 1);
}

bool httportmapObj::reg(const std::list<reginfo> &ports, const fdptr &timeoutfd)
{
	if (ports.empty())
		return true;

	for (std::list<reginfo>::const_iterator
		     b(ports.begin()), e(ports.end()); b != e; ++b)
	{
		std::string n=b->name + b->port;
		std::string::const_iterator cb, ce;

		for (cb=n.begin(), ce=n.end(); cb != ce; ++cb)
			if ((unsigned char)*cb < ' ')
				badsvcport();

		if (b->name.size() == 0 || b->port.size() == 0)
			badsvcport();
	}

	std::lock_guard<std::mutex> lock(objmutex);

	if (daemon.null() && (daemon=local_daemon.get()).null())
		return true; // App shutdown, most likely. Punt.

	ref<daemonConnObj> conn(daemon);

	std::lock_guard<std::mutex> connlock(conn->objmutex);

	if (conn->clientfd.null())
	{
		fd newfd(connect(httportmap::base::portmap_service, 0, timeoutfd));

		httportmapBase::connect_service(newfd);
		conn->clientfd=newfd;
	}

	clock connection_lock(conn, timeoutfd, *this);

	std::string resp;

	for (std::list<reginfo>::const_iterator
		     b(ports.begin()), e(ports.end()); b != e; ++b)
	{
		(*connection_lock.io)
			<< "SVC\t"
			<< (b->flags & httportmap::base::pm_exclusive ? "X":"")
			<< (b->flags & httportmap::base::pm_public ? "P":"")
			<< "-"
			<< '\t' << b->name
			<< '\t' << b->port << '\n' << std::flush;

		if (!connection_lock.io->good()
		    || !std::getline(*connection_lock.io, resp).good())
			badclient();

		if (resp.substr(0, 1) != "+")
			badclient();
	}

	(*connection_lock.io)
		<< "REG\t" << uuid_s << "\n" << std::flush;

	if (!connection_lock.io->good()
	    || !std::getline(*connection_lock.io, resp).good())
		badclient();

	connection_lock.check(conn);
	return resp.substr(0, 1) == "+";
}

void httportmapObj::dereg(const std::string &svc,
			  const std::string &port,
			  const fdptr &timeoutfd)

{
	{
		std::string n=svc+port;
		std::string::const_iterator cb, ce;

		for (cb=n.begin(), ce=n.end(); cb != ce; ++cb)
			if ((unsigned char)*cb < ' ')
				badsvcport();
	}

	std::lock_guard<std::mutex> lock(objmutex);

	if (daemon.null())
		badclient();

	ref<daemonConnObj> conn(daemon);

	std::lock_guard<std::mutex> connlock(conn->objmutex);

	if (conn->clientfd.null())
		badclient();

	clock connection_lock(conn, timeoutfd, *this);

	std::string resp;

	(*connection_lock.io) << "DEREG\t" << uuid_s << '\t'
			      << svc << '\t' << port << "\n" << std::flush;

	if (!connection_lock.io->good() ||
	    !std::getline(*connection_lock.io, resp).good() ||
	    resp.substr(0, 1) != "+")
		badclient();

	connection_lock.check(conn);
}

void httportmapObj::dereg(const std::string &svc,
			  int port,
			  const fdptr &timeoutfd)

{
	dereg(svc, to_string(port, locale::base::c()), timeoutfd);
}

void httportmapObj::dereg(const std::string &svc,
			  const fd &fdArg,
			  const fdptr &timeoutfd)

{
	std::string o;

	{
		sockaddr name(fdArg->getsockname());

		switch (name->family()) {
		case AF_INET:
		case AF_INET6:
			o=to_string(name->port(), locale::base::c());
			break;
		case AF_UNIX:
			o=name->address();
			break;
		default:
			throw EXCEPTION(libmsg()
					->get(_txt("Unknown file descriptor name")));
		}
	}

	dereg(svc, o, timeoutfd);
}


void httportmapObj::dereg(const std::string &svc,
			  const std::list<fd> &fdList,
			  const fdptr &timeoutfd)

{
	for (std::list<fd>::const_iterator b(fdList.begin()),
		     e(fdList.end()); b != e; ++b)
		dereg(svc, *b, timeoutfd);
}

void httportmapObj::nosuchuser(uid_t u)
{
	throw EXCEPTION(gettextmsg(_("httportmap: userid %1% not found"), u));
}

void httportmapObj::badresponse()
{
	throw EXCEPTION(_("Unable to parse response from portmapper"));
}

fd httportmapObj::connect_any(const std::string &name,
			      const fdptr &timeoutfd)
{
	std::vector<httportmap::base::service> entries;

	{
		std::set<std::string> services;
		std::set<uid_t> userids;
		std::set<pid_t> pids;

		services.insert(name);

		list(std::back_insert_iterator<std::vector<httportmap::base::service>
					       >(entries),
		     services, userids, pids, timeoutfd);
	}

	return do_connect(entries, timeoutfd, name);
}

fd httportmapObj::connect(const std::string &name,
			  uid_t user,
			  const fdptr &timeoutfd)
{
	std::vector<httportmap::base::service> entries;

	list(std::back_insert_iterator<std::vector<httportmap::base::service> >
	     (entries), name, user, timeoutfd);

	std::string o=name + "." + to_string(user, locale::base::c());

	return do_connect(entries, timeoutfd, o);
}

fd httportmapObj::do_connect(std::vector<httportmap::base::service> &entries,
			     const fdptr &timeoutfd,
			     const std::string &servicename)

{
	if (server.size() > 0)
	{
		for (std::vector<httportmap::base::service>::iterator
			     b(entries.begin()),
			     e(entries.end()), p; (p=b) != e; )
		{
			++b;

			if (p->getPort().substr(0, 1) == "/")
				entries.erase(p);
		}
	}

	for (std::vector<httportmap::base::service>::const_iterator
		     b(entries.begin()),
		     e(entries.end()); b != e; )
	{
		const std::string &port=b->getPort();

		++b;

		try {
			netaddr res=port.substr(0, 1) == "/"
				? netaddr::create(SOCK_STREAM,
							"file:" + port,
							"")
				: netaddr::create(SOCK_STREAM,
							server, port);

			return timeoutfd.null()
				? res->connect()
				: res->connect(fdtimeoutconfig::terminate_fd
					       (timeoutfd));
		} catch (...) {
			if (b == e)
				throw;
		}
	}

	errno=ECONNREFUSED;

	throw SYSEXCEPTION(servicename);
}

void httportmapObj::badclient()
{
	throw EXCEPTION(libmsg()
			->get(_txt("Lost connection with the httportmap service")));
}

void httportmapObj::badsvcport()
{
	throw EXCEPTION(libmsg()
			->get(_txt("Invalid service or port name")));
}

void httportmapObj::regpid2exe(const fdptr &timeoutfd)

{
	if (!reg(httportmap::base::pid2exe_service, "*",
		 httportmap::base::pm_public,
		 timeoutfd))
		throw EXCEPTION("Unexpected failure registering pid2exe service");

}

void httportmapObj::deregpid2exe(const fdptr &timeoutfd)

{
	dereg(httportmap::base::pid2exe_service, timeoutfd);
}

std::string httportmapObj::pid2exe(pid_t pid, const fdptr &timeoutfd)

{
	std::string exe;

	std::set<std::string> service_list;
	std::set<uid_t> users_list;
	std::set<pid_t> pid_list;

	service_list.insert(httportmap::base::pid2exe_service);
	pid_list.insert(pid);

	std::vector<httportmap::base::service> services;

	list(std::back_insert_iterator< std::vector<httportmap::base::service> >(services),
	     service_list, users_list, pid_list, timeoutfd);

	if (services.size() > 0)
		exe=services[0].path;
	return exe;
}

#if 0
{
#endif
}
