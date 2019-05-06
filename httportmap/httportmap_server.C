/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "httportmap_server.H"
#include "portmap_server.H"
#include "server.H"
#include "reexecfd.H"
#include "localstatedir.h"
#include "portmapdatadir.h"
#include "x/locale.H"
#include "x/fileattr.H"
#include "x/options.H"
#include "x/httportmap.H"
#include "x/http/fdclientimpl.H"
#include "x/http/cgiimpl.H"
#include "x/timerfd.H"
#include "x/destroy_callback.H"
#include "x/sysexception.H"
#include "x/fditer.H"
#include "x/serialize.H"
#include "x/deserialize.H"
#include "x/pidinfo.H"
#include "x/strtok.H"
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <unistd.h>
#include <string.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifndef DEFAULTSOCKET
#define DEFAULTSOCKET LOCALSTATEDIR "/run/httportmap"
#endif

#include "../base/pidinfo_internal.h"

#if HAVE_SYSCTL_KERN_PROC

class pidinfo {

public:
	static std::string pid2exe(const std::string &line);
};

std::string pidinfo::pid2exe(const std::string &line)
{
	std::string s;

	// Split out the pid from the client-offered executable pathname

	size_t sp=line.find(' ');

	if (sp == std::string::npos)
		return s;

	std::istringstream i(line.substr(0, sp));

	pid_t p=0;

	i >> p;

	if (p == 0)
		return s;

	std::string exe=line.substr(sp+1);

	if (*exe.c_str() != '/')
		return s;

	char *rp=realpath(exe.c_str(), NULL);

	if (!rp)
		return s;

	{
		char buf[strlen(rp)+1];

		strcpy(buf, rp);
		free(rp);
		exe=buf;
	}

	// Look up the pid's TEXT's dev/ino in sysctl

	dev_t dev;
	ino_t ino;

	if (!LIBCXX_NAMESPACE::pid2devino(p, dev, ino))
		return s;

	struct stat stat_buf;

	// Make sure its dev/ino matches the executable's dev/ino

	if (lstat(exe.c_str(), &stat_buf) == 0 &&
	    stat_buf.st_dev == dev &&
	    stat_buf.st_ino == ino)
	{
		// Grab the pid's starting time.

		std::string start_time=LIBCXX_NAMESPACE::exestarttime(p);
		if (start_time.empty())
			return s;

		// Verify that the process's TEXT's dev/ino hasn't changed

		dev_t dev2;
		ino_t ino2;

		if (!LIBCXX_NAMESPACE::pid2devino(p, dev2, ino2))
			return s;

		// And that the startting time is still the same
		if (dev == dev2 && ino == ino2 &&
		    start_time == LIBCXX_NAMESPACE::exestarttime(p))
		{
			// Then we are certain that pid was executing the
			// given process

			s=start_time + " " + exe;
		}
	}
	return s;
}

#else
//! Retrieve the process's executable pathname and start time.

class pidinfo {

public:

	//! The absolute pathname of the executable that started the process.
	std::string exe;

	//! The time that the executable was started, as an opaque string

	//! This should be treated as an opaque blob, with the only defined
	//! operation being a comparison for equality or inequality.
	std::string start_time;

	//! Constructor
	pidinfo(//! Return this process's information
		pid_t p);
	~pidinfo();

	static std::string pid2exe(const std::string &line);
};

pidinfo::pidinfo(pid_t pid)
{
	auto l=LIBCXX_NAMESPACE::locale::base::c();

	if (pid == getpid())
	{
		exe=LIBCXX_NAMESPACE::exename();
	}
	else
	{
		std::string s="/proc/";

		s += LIBCXX_NAMESPACE::to_string(pid, l);

		s += "/exe";

		exe=LIBCXX_NAMESPACE::fileattr::create(s, true)
			->readlink();

		if (exe.substr(0, 1) != "/")
		{
			errno=ENOENT;
			throw SYSEXCEPTION(exe);
		}
	}

	if ((start_time=LIBCXX_NAMESPACE::exestarttime(pid)).empty())
		throw EXCEPTION("Cannot get start time for " + exe);
}

pidinfo::~pidinfo()
{
}

std::string pidinfo::pid2exe(const std::string &line)
{
	std::istringstream i(line);

	pid_t p=0;

	i >> p;

	std::string s;

	if (p)
	{
		// Read the executable name and the pid start
		// time twice. If the start time did not change
		// this means it was the same process, and
		// the executable name is valid.

		pidinfo a(p);

		pidinfo b(p);

		if (a.exe == b.exe &&
		    a.start_time == b.start_time)
		{
			s=a.start_time + " " + a.exe;
		}
	}

	std::replace(s.begin(), s.end(), '\n', ' ');

	return s;
}
#endif

// Fork. The child process returns. The parent waits for the child process
// to call parent_can_exit().

static LIBCXX_NAMESPACE::fd as_child()
{
	std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
		fpipe(LIBCXX_NAMESPACE::fd::base::pipe());

	pid_t p=fork();

	if (p)
	{
		char exitcode=1;

		fpipe.second->close();
		fpipe.first->read(&exitcode, 1);

		exit(exitcode);
	}
	fpipe.first->close();
	return fpipe.second;
}

static void parent_can_exit(const LIBCXX_NAMESPACE::fd &pipe,
			    int exitcode)
{
	char c=exitcode;

	pipe->write(&c, 1);
	pipe->close();
}

httportmap_server::httportmap_server(const std::string &sockname,
				     const std::string &portmapdatadirArg,
				     int httpport,
				     bool daemonize)

	: portmapdatadir(portmapdatadirArg),
	  pid2exe_proc(pid2exe_thread()),
	  request_lock(LIBCXX_NAMESPACE::sharedlock::create())
{
	if (daemonize)
	{
		LIBCXX_NAMESPACE::fd pipe=as_child();

		startup(sockname, portmapdatadirArg, httpport, true);
		if (daemonize)
			parent_can_exit(pipe, 0);
	}
	else
		startup(sockname, portmapdatadirArg, httpport, false);
}

LIBCXX_NAMESPACE::iostream httportmap_server::pid2exe_thread()
{
	std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd> pid2exe_proc=LIBCXX_NAMESPACE::fd::base::socketpair();

	pid_t p=fork();

	if (p < 0)
		throw SYSEXCEPTION("fork");

	if (p)
		return pid2exe_proc.first->getiostream();

	pid2exe_proc.first->close();

	LIBCXX_NAMESPACE::iostream iosref=pid2exe_proc.second->getiostream();

	std::string line;

	while (!std::getline(*iosref, line).eof())
	{
		std::string s;

		try {
			s=pidinfo::pid2exe(line);
		} catch (...) {
		}

		(*iosref) << s << std::endl << std::flush;
	}
	exit(0);
}

void httportmap_server::parse_pid2exeproc_response(pid2exe_proc_t::lock &lock,
						   std::string &start_time,
						   std::string &exename)
{
	std::string s;

	std::getline(**lock, s);

	size_t p=s.find(' ');

	start_time.clear();
	exename.clear();

	if (p == std::string::npos)
		throw EXCEPTION("Unable to look up process information");

	start_time=s.substr(0, p);
	exename=s.substr(p+1);
}

LOG_FUNC_SCOPE_DECL(startup, startup_logger);

void httportmap_server::startup(const std::string &sockname,
				const std::string &portmapdatadirArg,
				int httpport,
				bool daemonize)
{
	LOG_FUNC_SCOPE(startup_logger);

	if (daemonize)
	{
		bool connected=false;

		try {
			LIBCXX_NAMESPACE::timerfd timer=
				LIBCXX_NAMESPACE::timerfd::create();

			timer->set(0, 10);

			LIBCXX_NAMESPACE::fd existing_fd=
				LIBCXX_NAMESPACE::netaddr
				::create(SOCK_STREAM, "file:" + sockname
					 + ".client", "")
				->connect(LIBCXX_NAMESPACE::fdtimeoutconfig
					  ::terminate_fd(timer));

			LIBCXX_NAMESPACE::httportmap::base
				::connect_service(existing_fd);

			existing_fd->nonblock(true);

			LIBCXX_NAMESPACE::fdtimeout
				timeout(LIBCXX_NAMESPACE::fdtimeout
					::create(existing_fd));

			timeout->set_terminate_fd(timer);

			LOG_DEBUG("Connected to the existing portmapper");

			// Ok, retrieve the existing connection.

			connected=true;

			timeout->set_write_timeout(10);

			(*timeout->getostream())
				<< "REEXEC\t" HTTPORTMAP_SERVER_VERSION
				<< std::endl
				<< std::flush;

			LIBCXX_NAMESPACE::sharedlock::base::unique
				unique=request_lock->create_unique();

			LIBCXX_NAMESPACE::destroy_callback flag=
				LIBCXX_NAMESPACE::destroy_callback::create();

			{
				auto recv(reexec_fd_recv::create(timeout,
								 existing_fd));
				recv->ondestroy([flag]{flag->destroyed();});

				meta.serialize(*recv);

				LOG_DEBUG("Deserialized existing listening sockets");

				startup_frommeta(daemonize, recv);
			}
			flag->wait();
			return;

		} catch (const LIBCXX_NAMESPACE::exception &e)
		{
			if (connected)
				throw;
		}
	}

	{
		LIBCXX_NAMESPACE::fdptr lockfile=
			LIBCXX_NAMESPACE::fd::base::lockf(sockname + ".lock",
							F_TLOCK, 0600);

		if (lockfile.null())
			throw EXCEPTION("httportmap is already running");

		meta.sockets[(int)startup_metainfo::sock_lockfile].push_back(lockfile);
	}

	unlink(sockname.c_str());
	mkdir((sockname+".priv").c_str(), 0700);
	unlink((sockname+".priv/socket").c_str());

	LIBCXX_NAMESPACE::netaddr::create(SOCK_STREAM, "file:" + sockname, "")
		->bind(meta.sockets[(int)startup_metainfo::sock_pub_listen], true);
	chmod(sockname.c_str(), 0777);

	LIBCXX_NAMESPACE::netaddr ::create(SOCK_STREAM, "file:" + sockname
					 + ".priv/socket", "")
		->bind(meta.sockets[(int)startup_metainfo::sock_priv_listen], true);
	chmod((sockname+".priv/socket").c_str(), 0777);

	if (httpport >= 0)
	{
		LIBCXX_NAMESPACE::netaddr::create("", httpport)
			->bind(meta.sockets[(int)startup_metainfo::sock_http], true);

		if (meta.sockets[(int)startup_metainfo::sock_http].empty())
			throw EXCEPTION("No sockets?");

		if (httpport == 0)
			std::cout << meta.sockets[(int)startup_metainfo
						  ::sock_http].front()
				->getsockname()->port() << std::endl;
	}

	meta.portmapclient_socket=sockname + ".client";

	unlink(meta.portmapclient_socket.c_str());

	LIBCXX_NAMESPACE::netaddr::create(SOCK_STREAM, "file:" +
					meta.portmapclient_socket,
					"")
		->bind(meta.sockets[(int)startup_metainfo::sock_portmap], true);
	chmod(meta.portmapclient_socket.c_str(), 0777);

	startup_frommeta(daemonize, reexec_fd_recvptr());
}

void httportmap_server::startup_frommeta(bool daemonize,
					 const reexec_fd_recvptr &reexec_ptr)
{
	LOG_FUNC_SCOPE(startup_logger);

	intern_server=LIBCXX_NAMESPACE::ptr<httpserver>
		::create(true, false, *this);

	intern_priv_server=LIBCXX_NAMESPACE::ptr<httpserver>
		::create(true, true, *this);

	extern_server=LIBCXX_NAMESPACE::ptr<httpserver>
		::create(false, false, *this);

	portmap_thread=LIBCXX_NAMESPACE::ptr<portmap_server>::create(*this);

	intern_listener=
		LIBCXX_NAMESPACE::fdlistener
		::create(meta.sockets[(int)startup_metainfo
				      ::sock_pub_listen], 10, 0,
			 "internserver");

	intern_priv_listener=
		LIBCXX_NAMESPACE::fdlistener
		::create(meta.sockets[(int)startup_metainfo
				      ::sock_priv_listen], 10, 0,
			 "internserver");

	if (!meta.sockets[(int)startup_metainfo::sock_http].empty())
		extern_listener=
			LIBCXX_NAMESPACE::fdlistener
			::create(meta.sockets[(int)startup_metainfo
					      ::sock_http], 10, 0,
				 "externserver",
				 "httpserver");

	portmap_thread->listen_sockets=
		meta.sockets[(int)startup_metainfo::sock_portmap];

	std::string me=LIBCXX_NAMESPACE::exename();

	if (daemonize && geteuid() == 0)
	{
		if (setreuid(0, 0))
			throw EXCEPTION("setreuid");

		LOG_INFO("Portmapper started"); // Open syslog before chrooting

		{
			LIBCXX_NAMESPACE::passwd nobody("nobody");

			if (!nobody->pw_name)
				throw EXCEPTION("Cannot get nobody's uid and gid");

			if (chdir(portmapdatadir.c_str()) < 0 ||
			    chroot(".") < 0)
				throw SYSEXCEPTION(portmapdatadir);

			portmapdatadir=".";

			setsid();

			if (setgroups(1, &nobody->pw_gid) < 0 ||
			    setregid(nobody->pw_gid, nobody->pw_gid) < 0 ||
			    setreuid(nobody->pw_uid, nobody->pw_uid) < 0)
				throw SYSEXCEPTION("nobody userid/groupid");
		}
		LOG_TRACE("Dropped root");
		meta.sockets[(int)startup_metainfo::sock_lockfile]
			.front()->cancel();
		// Wouldn't be able to do anyway, in the chroot jail.

	}

	{
		entry clientEntry[2];

		clientEntry[0].service=LIBCXX_NAMESPACE::httportmap::base::portmap_service;
		clientEntry[0].exclusive=true;
		clientEntry[0].port=meta.portmapclient_socket;
		clientEntry[0].pid=getpid();
		clientEntry[0].exe=me;
		clientEntry[0].user=0;

		clientEntry[1]=clientEntry[0];
		clientEntry[1].service=LIBCXX_NAMESPACE::httportmap::base::pid2exe_service;
		clientEntry[1].exclusive=false;
		clientEntry[1].port="*";

		services_obj_t::lock l(services);
		std::multimap<std::string, s_iter> dummy;

		regsvcs(clientEntry, clientEntry+2, dummy, l, "");
	}

	LOG_TRACE("Registered my services");
	if (!extern_listener.null())
		extern_listener->start(extern_server);

	intern_listener->start(intern_server);
	intern_priv_listener->start(intern_priv_server);
	portmap_thread_run=LIBCXX_NAMESPACE::run(portmap_thread, reexec_ptr);
	LOG_TRACE("Portmap thread started");
}

void httportmap_server::wait()
{
	LOG_FUNC_SCOPE(startup_logger);

	if (!intern_listener.null())
	{
		LOG_TRACE("Waiting for the internal listener to stop: "
			  << intern_listener.null());
		intern_listener->wait();
		LOG_TRACE("Internal listener stopped");
	}
}

httportmap_server::~httportmap_server()
{
	wait();

	if (!portmap_thread.null())
		portmap_thread->stop();

	if (!portmap_thread_run.null())
		portmap_thread_run->wait();

	if (!intern_priv_listener.null())
		intern_priv_listener->wait();

	if (!extern_listener.null())
		extern_listener->wait();
}


void httportmap_server::startup_metainfo::serialize(reexec_fd_send &iter)
{
	iter.iter(portmapclient_socket);

	std::list<std::pair<int, int> > nsockets;

	for (auto socketlist : sockets)
	{
		nsockets.push_back(std::make_pair(socketlist.first,
						  socketlist.second.size())
				   );
	}

	iter.iter(nsockets);

	for (auto socketlist : sockets)
	{
		for (auto socket : socketlist.second)
			iter.send_fd(socket);
	}
}

void httportmap_server::startup_metainfo::serialize(reexec_fd_recvObj &iter)
{
	iter.iter(portmapclient_socket);

	std::list<std::pair<int, int> > nsockets;

	iter.iter(nsockets);

	while (!nsockets.empty())
	{
		auto s=nsockets.front();

		while (s.second)
		{
			sockets[s.first].push_back(iter.recv_fd());
			--s.second;
		}
		nsockets.pop_front();
	}
}

#include "httportmap_server.opts.h"

class args {

public:
	std::string sockname;
	std::string files;
	int port;
	bool daemon;
	bool stop;

	args(const httportmap_server_opts &o) : sockname(o.socket->value),
						files(o.files->value),
						port(o.http->isSet() ?
						     o.port->value: -1),
						daemon(o.daemon->isSet()),
						stop(false)
	{
	}

	args() : port(80), daemon(false), stop(false) {}

	~args()
	{
	}

	template<typename iter_type>
	void serialize(iter_type &iter)
	{
		iter(sockname);
		iter(files);
		iter(port);
		iter(daemon);
		iter(stop);
	}

	std::string systemd_dir() const
	{
		return sockname + ".systemd";
	}

	std::string systemd_lockfile() const
	{
		return systemd_dir() + "/lock";
	}

	std::string systemd_sockfile() const
	{
		return systemd_dir() + "/socket";
	}
};


static void start(const args &a)
{
	LOG_FUNC_SCOPE(startup_logger);

	try {
		httportmap_server portmap(a.sockname,
					  a.files,
					  a.port,
					  a.daemon);

		LOG_DEBUG("Waiting for the server to stop");
		portmap.wait();

	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		LOG_FATAL(e);
		LOG_TRACE(e->backtrace);
		exit(1);
	}
}

static int stop(const std::string &socketName)
{
	LIBCXX_NAMESPACE::fdptr socket;

	try {
		socket=LIBCXX_NAMESPACE::netaddr
			::create(SOCK_STREAM,
				 "file:" + socketName
				 + ".priv/socket",
				 "")->connect();
	} catch (const LIBCXX_NAMESPACE::sysexception &e)
	{
		switch (e.getErrorCode()) {
		case ENOENT:
		case ECONNREFUSED:
			return 0;
		}
		throw;
	}

	LIBCXX_NAMESPACE::http::fdclientimpl client;

	client.install(socket, LIBCXX_NAMESPACE::fdptr(), 0);

	LIBCXX_NAMESPACE::http::requestimpl req;

	req.setURI("http://localhost/portmap/stop");

	LIBCXX_NAMESPACE::http::responseimpl resp;

	if (client.send(req, resp))
	{
		if (resp.getStatusCode() == 200)
		{
			if (req.responseHasMessageBody(resp))
			{
				for (LIBCXX_NAMESPACE::http::fdclientimpl
					     ::iterator
					     b(client.begin()),
					     e(client.end()); b != e;
				     ++b)
					;
			}

			// Wait for the process to terminate and
			// close the socket.

			try {
				char dummy;

				socket->nonblock(false);

				socket->read(&dummy, 1);
			} catch (...)
			{
			}
			return 0;
		}

		if (req.responseHasMessageBody(resp))
			std::copy(client.begin(), client.end(),
				  std::ostreambuf_iterator<char>(std::cerr));
		std::cerr << std::endl;
	}
	std::cerr << "Failed to stop the daemon:"
		  << std::endl;

	return 1;
}

// Fork. The child runs start(), which exits (start() forks again by invoking
// as_child(), then the parent exits. Wait for that parent to exit here,
// before returning.

static void start_as_child(const args &a)
{
	pid_t p=fork();

	if (p < 0)
		throw SYSEXCEPTION("fork");

	if (p == 0)
		start(a); // Does not return, calls exit()

	int status;

	if (waitpid(p, &status, 0) != p)
		throw SYSEXCEPTION("waitpid");

	if (!WIFEXITED(status))
		exit(1);

	if (WEXITSTATUS(status))
		exit(1);
}

// systemd-specific code to deal with its stupidity. Have a process listen
// for nothing other than a signal to reexec itself.
//
// After updating libcxx, the start() commands starts the new httportmap
// executable, which negotiated with the existing executable to do an
// orderly handover, at which point the old executable exits. The new
// httportmap daemon is a child of the new, started executable.
//
// systemd can't deal with this. We must have the existing process listen
// and reexec itself. We'll use a separate socket to pass the startup
// arguments, from the terminal-initiated command to the running daemon,
// which then

static void systemd_brain_damage(const std::string &me,
				 const LIBCXX_NAMESPACE::fd &lockfile,
				 const LIBCXX_NAMESPACE::fd &listenfd);

static void systemd_start(const args &a,
			  const std::string &exename)
{
	auto pipe=as_child();

	mkdir(a.systemd_dir().c_str(), 0700);
	std::string lockfilename=a.systemd_lockfile();
	std::string sockname=a.systemd_sockfile();

	LIBCXX_NAMESPACE::fdptr lockfile=
		LIBCXX_NAMESPACE::fd::base::lockf(lockfilename,
						F_TLOCK, 0600);

	if (lockfile.null())
		throw EXCEPTION("httportmap is already running");

	start_as_child(a);

	LIBCXX_NAMESPACE::fd listenfd=({
			std::list<LIBCXX_NAMESPACE::fd> fdlist;

			unlink(sockname.c_str());

			LIBCXX_NAMESPACE::netaddr::create(SOCK_STREAM,
							"file:" + sockname, "")
				->bind(fdlist, true);

			fdlist.front();
		});

	parent_can_exit(pipe, 0);

	listenfd->listen();
	systemd_brain_damage(exename, lockfile, listenfd);
}

static void systemd_brain_damage(const std::string &me,
				 const LIBCXX_NAMESPACE::fd &lockfile,
				 const LIBCXX_NAMESPACE::fd &listenfd)
{
	LIBCXX_NAMESPACE::fd cl=listenfd->accept();

	lockfile->closeonexec(false);
	listenfd->closeonexec(false);
	cl->closeonexec(false);

	auto c_locale=LIBCXX_NAMESPACE::locale::base::c();

	std::string lockfile_ss=
		LIBCXX_NAMESPACE::to_string(lockfile->get_fd(), c_locale);
	std::string listenfd_ss=
		LIBCXX_NAMESPACE::to_string(listenfd->get_fd(), c_locale);
	std::string cl_ss=
		LIBCXX_NAMESPACE::to_string(cl->get_fd(), c_locale);

	const char *n=me.c_str();

	execl(n, n, "systemd-reload", lockfile_ss.c_str(),
	      listenfd_ss.c_str(),
	      cl_ss.c_str(), (char *)0);
	throw SYSEXCEPTION("exec");
}

static void systemd_reload(const std::string &exename,
			   const std::list<std::string> &s)
{
	if (s.size() != 3)
		return; // Something incompatible.

	std::list<LIBCXX_NAMESPACE::fd> fds;

	for (auto &str:s)
	{
		std::istringstream i(str);

		int n= -1;

		i >> n;

		auto fd=LIBCXX_NAMESPACE::fd::base::dup(n);
		::close(n);
		fds.push_back(fd);
	}

	LIBCXX_NAMESPACE::fd cl=fds.back();

	fds.pop_back();

	LIBCXX_NAMESPACE::fdinputiter b(cl), e;

	LIBCXX_NAMESPACE::deserialize
		::iterator<LIBCXX_NAMESPACE::fdinputiter> i(b, e);

	args newargs;

	i(newargs);

	if (newargs.stop)
	{
		stop(newargs.sockname);
		unlink(newargs.systemd_sockfile().c_str());
		unlink(newargs.systemd_lockfile().c_str());
		return;
	}
	cl->close();
	start_as_child(newargs);
	systemd_brain_damage(exename, fds.front(), fds.back());
}

static void systemd_send(const args &a)
{
	try {
		auto sock=LIBCXX_NAMESPACE::netaddr::create(SOCK_STREAM,
							  "file:" +
							  a.systemd_sockfile(),
							  "")
			->connect();

		{
			LIBCXX_NAMESPACE::fdoutputiter o(sock);
			LIBCXX_NAMESPACE::serialize
				::iterator<LIBCXX_NAMESPACE::fdoutputiter>
				iter(o);

			iter(a);
			o.flush();
		}

		char dummy;

		sock->read(&dummy, 1);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
	}
}

static int main2(int argc, char **argv)
{
	// Load the UTF-8 and C locales, before we go into chroot jail
	auto utf8=LIBCXX_NAMESPACE::locale::base::utf8();
	auto c=LIBCXX_NAMESPACE::locale::base::c();

	httportmap_server_opts opts;

	LIBCXX_NAMESPACE::option::parser
		optionParser(opts.parse(argc, argv));

	if (optionParser->args.empty())
		return 0;

	std::string &cmd=optionParser->args.front();
	std::string me=LIBCXX_NAMESPACE::exename();

	if (cmd == "start")
	{
		start(args(opts));
		return 0;
	}

	if (cmd == "stop")
	{
		return stop(opts.socket->value);
	}

	if (cmd == "systemd-start")
	{
		args a=opts;

		a.daemon=true;
		systemd_start(a, me);
		return 0;
	}

	if (cmd == "systemd-restart")
	{
		args a=opts;

		a.daemon=true;
		systemd_send(a);
		return 0;
	}

	if (cmd == "systemd-stop")
	{
		args a=opts;

		a.stop=true;
		systemd_send(a);
		return 0;
	}

	if (cmd == "systemd-reload")
	{
		optionParser->args.pop_front();

		systemd_reload(me, optionParser->args);
		return 0;
	}

	if (cmd == "status")
	{
		try {
			LIBCXX_NAMESPACE::fd
				socket(LIBCXX_NAMESPACE::netaddr
				       ::create(SOCK_STREAM,
						"file:" DEFAULTSOCKET)
				       ->connect());
		} catch (...)
		{
			exit(1);
		}
		exit(0);
	}

	optionParser->usage(std::cout);
	exit(1);
}

static int docgi()
{
	LIBCXX_NAMESPACE::http::cgiimpl cgi(true);
	LIBCXX_NAMESPACE::http::requestimpl req(cgi);

	LIBCXX_NAMESPACE::fd
		socket(LIBCXX_NAMESPACE::netaddr
		       ::create(SOCK_STREAM, "file:" DEFAULTSOCKET)
		       ->connect());

	LIBCXX_NAMESPACE::http::fdclientimpl client;
	client.install(socket, LIBCXX_NAMESPACE::fdptr(), 0);

	LIBCXX_NAMESPACE::http::responseimpl resp;

	if (cgi.remote_addr.null() || cgi.server_addr.null() ||
	    cgi.remote_addr->address() != cgi.server_addr->address())
		req.replace("X-External-Request", "1");

	if (!(req.hasMessageBody()
	      ? client.send(req, cgi.begin(), cgi.end(), resp)
	      : client.send(req, resp)))
		throw EXCEPTION("Internal error: request refused");

	LIBCXX_NAMESPACE::http::cgiimpl::send_response_header(resp);

	if (req.responseHasMessageBody(resp))
		std::copy(client.begin(), client.end(),
			  std::ostreambuf_iterator<char>(std::cout));
	std::cout << std::flush;
	return 0;
}

int main(int argc, char **argv)
{
	LOG_FUNC_SCOPE(startup_logger);

	bool is_cgi=getenv("REQUEST_METHOD") != NULL;

	try {
		return is_cgi ? docgi():main2(argc, argv);
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		if (is_cgi)
		{
			std::cout << "Content-Type: text/plain"
				  << std::endl << std::endl
				  << e << std::endl << e->backtrace;
		}
		else
		{
			LOG_ERROR(e);
			LOG_TRACE(e->backtrace);
		}
		exit(1);
	}
}
