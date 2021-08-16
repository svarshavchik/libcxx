/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sysexception.H"
#include "x/managedsingletonapp.H"
#include "x/httportmap.H"
#include "x/fd.H"
#include "x/fileattr.H"
#include "x/locale.H"
#include "x/weaklist.H"
#include "x/pwd.H"
#include "x/sighandler.H"
#include "x/sigset.H"
#include "x/sighandler.H"
#include "x/property_value.H"
#include "x/messages.H"
#include "x/pidinfo.H"
#include "x/sigset.H"
#include "x/strtok.H"
#include "gettext_in.h"
#include <sstream>
#include <unistd.h>
#include <signal.h>
#include <sys/poll.h>

#include <list>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::singletonapp);

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

#pragma GCC visibility push(hidden)

class singletonapp::impl : virtual public obj {

	httportmap portmapclient;
	std::string n;

	static std::string svcname() LIBCXX_INTERNAL;

	std::string listensockname;

	void close();
public:
	runthreadbaseptr thrinstance;

	class mysighandler;

	ptr<obj> sighandlermcguffin;

	static property::value<std::string> signallistprop;

	static property::value<size_t> credtimeoutprop;

	impl(const ref<singletonapp::factorybaseObj> &app,
	     fdptr &connection,
	     uid_t uid,
	     mode_t mode) LIBCXX_INTERNAL;
	~impl() LIBCXX_INTERNAL;

	static void notrunning(uid_t uid)
		__attribute__((noreturn));

	static ref<obj> install_all_sighandlers(const sighandler_t &handler,
						sigset &signals)
		LIBCXX_INTERNAL;

};

#pragma GCC visibility pop

property::value<std::string>
singletonapp::impl::signallistprop(LIBCXX_NAMESPACE_STR
				   "::singletonapp::termsigs",
				   "SIGINT SIGTERM SIGQUIT SIGHUP");

property::value<size_t>
singletonapp::impl::credtimeoutprop(LIBCXX_NAMESPACE_STR
				    "::singletonapp::credtimeout", 30);

#pragma GCC visibility push(hidden)

class singletonapp::thr : public threadmsgdispatcherObj {


	weaklist<obj> *threadsptr;
	bool *termflagptr;

public:

	thr() LIBCXX_INTERNAL {}
	~thr() LIBCXX_INTERNAL {}

	void run(ptr<obj> &threadmsgdispatcher_mcguffin,
		 fd &listensock,
		 fdptr &initinstance,
		 ref<singletonapp::factorybaseObj> &app) LIBCXX_HIDDEN;

private:

	void launch(const ref<singletonapp::factorybaseObj> &app,
		    const fd &initinstance) LIBCXX_INTERNAL;

#include "singletonapp.msgs.H"

};

#pragma GCC visibility pop

class LIBCXX_HIDDEN singletonapp::impl::mysighandler : virtual public obj {

 public:
	ref<thr> runthr;

	mysighandler(const ref<thr> &runthrArg)
		: runthr(runthrArg)
	{
	}

	~mysighandler()
	{
	}

	void signal(int signum)
	{
		LOG_FUNC_SCOPE(singletonapp::logger);

		LOG_DEBUG("Signal " << signum << " received");
		runthr->stoplistening();
	}
};

// Derive a portmapper name based on the executable's dev+inode

std::string singletonapp::impl::svcname()
{
	std::ostringstream o;

	o << exename() << "@app."
	  << httportmap::base::portmap_service;

	return o.str();
}

singletonapp::factorybaseObj::factorybaseObj()
{
}

singletonapp::factorybaseObj::~factorybaseObj()
{
}

singletonapp::impl::impl(const ref<singletonapp::factorybaseObj> &app,
			 fdptr &connection,
			 uid_t uid,
			 mode_t mode)
	: portmapclient(httportmap::create()), n(svcname())
{
	ref<thr> implthr=ref<thr>::create();

	ref<mysighandler> implmysighandler=ref<mysighandler>::create(implthr);

	auto mcguffin=
		({
			sigset ss;

			auto mcguffin=install_all_sighandlers
				([implmysighandler]
				 (int signum)
				 {
					 implmysighandler->signal(signum);
				 }, ss);

			ss.block();

			mcguffin;
		});

	while (1)
	{
		try {
			connection=portmapclient->connect(n, uid);

			char buf;

			if (connection->read(&buf, 1) == 1)
				return;

			if (uid != getuid())
				notrunning(uid);

			continue;
		} catch (...) {
		}

		if (uid != getuid())
			notrunning(uid);

		{
			std::string exename=LIBCXX_NAMESPACE::exename();

			size_t p=exename.rfind('/');

			if (p != std::string::npos)
				exename=exename.substr(p+1);

			std::string listensockdir=
				fd::base::mktempdir(mode) + "/"
				+ exename + ".";

			std::pair<fd, std::string> tmp
				=fd::base::tmpunixfilesock(listensockdir);
			connection=tmp.first;
			connection->listen();
			listensockname=tmp.second;
		}

		if (portmapclient->reg(n, listensockname, true))
			break;
		connection=fdptr();
		close();
	}

	std::pair<fd, fd> fakeconn=fd::base::socketpair();

	thrinstance=start_threadmsgdispatcher(implthr, fd(connection),
					      fdptr(fakeconn.first), app);

	sighandlermcguffin=mcguffin;
	connection=fakeconn.second;
}

void singletonapp::impl::notrunning(uid_t uid)
{
	throw EXCEPTION(gettextmsg(_txt("%1% is not running as %2%"),
				   exename(),
				   passwd(uid)->pw_name));
}

singletonapp::impl::~impl()
{
	try {
		close();
	} catch (...)
	{
	}
}

void singletonapp::impl::close()
{
	if (!thrinstance.null())
	{
		thrinstance->wait();
		portmapclient->dereg(n, listensockname);
		thrinstance=runthreadbaseptr();
	}

	if (listensockname.size() > 0)
	{
		unlink(listensockname.c_str());
		listensockname.clear();
	}
}

void singletonapp::thr::run(ptr<obj> &threadmsgdispatcher_mcguffin,
			    fd &listensock,
			    fdptr &initinstance,
			    ref<singletonapp::factorybaseObj> &app)
{
	msgqueue_auto msgqueue(this);
	threadmsgdispatcher_mcguffin=nullptr;

	LOG_FUNC_SCOPE(singletonapp::logger);

	weaklist<obj> threads(weaklist<obj>::create());
	threadsptr= &threads;
	bool termflag=false;
	termflagptr= &termflag;

	launch(app, initinstance);
	initinstance=fdptr();

	struct pollfd pfd[2];

	{
		fd efd=msgqueue->get_eventfd();

		pfd[0].fd=efd->get_fd();

		efd->nonblock(true);

		listensock->nonblock(true);
		pfd[1].fd=listensock->get_fd();
	}

	pfd[0].events=pfd[1].events=POLLIN;

	try {
		while (1)
		{
			if (!msgqueue->empty())
			{
				msgqueue.event();
				continue;
			}

			const fdptr newsock=listensock->accept();

			if (!newsock.null())
			{
				newsock->nonblock(false);
				newsock->write("", 1);
				launch(app, newsock);
				continue;
			}

			poll(pfd, termflag ? 1:2, -1);

			if (pfd[0].revents & POLLIN)
				msgqueue->get_eventfd()->event();
		}
	} catch (const stopexception &e)
	{
	} catch (const exception &e)
	{
		LOG_FATAL(e);
		LOG_TRACE(e->backtrace);
	}
}

void singletonapp::thr::launch(const ref<singletonapp::factorybaseObj> &app,
			       const fd &initinstance)
{
	LOG_FUNC_SCOPE(singletonapp::logger);

	try {
		ref<obj> lthr=app->new_thread(initinstance,
					      [me=ref{this}]
					      {
						      me->terminated();
					      });


		(*threadsptr)->push_back(lthr);

	} catch (const exception &e)
	{
		LOG_ERROR(e);
		LOG_TRACE(e->backtrace);
	}
}

void singletonapp::thr::dispatch_terminated()
{
	for (auto b: **threadsptr)
	{
		if (!b.getptr().null())
			return;
	}

	throw stopexception();
}

void singletonapp::thr::dispatch_stoplistening()
{
	LOG_FUNC_SCOPE(singletonapp::logger);

	*termflagptr=true;

	LOG_DEBUG("Termination signal received, no longer listening for new connections");

	for (auto b: **threadsptr)
	{
		ptr<obj> t(b.getptr());

		if (t.null())
			continue;

		stoppableObj *s=dynamic_cast<stoppableObj *>(&*t);

                if (s)
                        s->stop();
	}
}

singletonapp::instanceObj::instanceObj(const ref<singletonapp::factorybaseObj>
				       &factory, uid_t uid, mode_t mode)
	: implinstance(ref<impl>::create(factory, connection, uid, mode))
{
}

singletonapp::instanceObj::~instanceObj()
{
	connection=fdptr();

	runthreadbaseptr thrinstance=implinstance->thrinstance;

	if (!thrinstance.null())
		thrinstance->wait();
}

singletonapp::instance
singletonapp::create_internal(const ref<factorybaseObj> &factory,
			      uid_t uid,
			      mode_t mode)
{
	return instance::create(factory, uid, mode);
}

ref<obj> singletonapp::install_all_sighandlers(const sighandler_t &handler)
{
	sigset dummy;

	return impl::install_all_sighandlers(handler, dummy);
}

namespace {
#if 0
};
#endif

// A mcguffin for the multiple mcguffins created from install_sighandler

class sighandler_mcguffinObj : virtual public obj {

public:
	std::list<ref<obj>> mcguffins;
};
#if 0
{
#endif
}

ref<obj>
singletonapp::impl::install_all_sighandlers(const sighandler_t &handler,
					    sigset &signals)

{
	auto mcguffin=ref<sighandler_mcguffinObj>::create();

	std::list<std::string> signalnames;

	strtok_str(signallistprop.get(), " \t\r\n,;", signalnames);

	while (!signalnames.empty())
	{
		int signum=sigset::name2sig_orthrow(signalnames.front());

		signalnames.pop_front();

		signals += signum;

		mcguffin->mcguffins.push_back(install_sighandler(signum,
								 handler));
	}

	return mcguffin;
}

uid_t singletonapp::validate_peer(const fd &connection, bool sameuser)
{
	return validate_peer(httportmap::base::regpid2exe(),
			     connection, sameuser);
}

uid_t singletonapp::validate_peer(const httportmap &portmapper,
				  const fd &connection, bool sameuser)
{

	connection->nonblock(false);
	connection->recv_credentials(true);
	connection->send_credentials();
	connection->nonblock(true);

	fd::base::credentials_t uc;

	struct pollfd pfd;

	pfd.fd=connection->get_fd();
	pfd.events=POLLIN;

	if (::poll(&pfd, 1, impl::credtimeoutprop.get() * 1000) < 0 ||
	    (pfd.revents & POLLIN) == 0 ||
	    !connection->recv_credentials(uc))
	{
		errno=ETIMEDOUT;
		throw SYSEXCEPTION("singletonapp (recv_credentials check failed)");
	}

	if (sameuser && uc.uid != getuid())
	{
		errno=EPERM;
		throw SYSEXCEPTION("singletonapp (uid mismatch: "
				   << uc.uid << " vs " << getuid() << ")");
	}

	auto my_exename=exename();
	auto pid2exe=portmapper->pid2exe(uc.pid);

	if (my_exename != pid2exe)
	{
		errno=EPERM;
		throw SYSEXCEPTION("singletonapp (" << pid2exe
				   << " connected to "
				   << my_exename << ")");
	}

	if (connection->write("", 1) != 1)
	{
		errno=EIO;
		throw SYSEXCEPTION("singletonapp (write() failed)");
	}

	char dummy;

	if (::poll(&pfd, 1, impl::credtimeoutprop.get() * 1000) < 0 ||
	    (pfd.revents & POLLIN) == 0 ||
	    connection->read(&dummy, 1) != 1)
	{
		errno=ETIMEDOUT;
		throw SYSEXCEPTION("singletonapp (poll() failed)");
	}

	connection->nonblock(false);
	return uc.uid;
}

singletonapp::processedObj::processedObj() : flag(false)
{
}

singletonapp::processedObj::~processedObj()
{
}

void singletonapp::processedObj::processed()
{
	flag=true;
}

#if 0
{
#endif
}
