/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sysexception.H"
#include "x/managedsingletonapp.H"
#include "x/httportmap.H"
#include "x/fd.H"
#include "x/fileattr.H"
#include "x/locale.H"
#include "x/ctype.H"
#include "x/eventqueuemsgdispatcher.H"
#include "x/weaklist.H"
#include "x/pwd.H"
#include "x/destroycallbackobj.H"
#include "x/sighandler.H"
#include "x/sigset.H"
#include "x/sighandler.H"
#include "x/property_value.H"
#include "x/messages.H"
#include "x/pidinfo.H"
#include "x/sigset.H"
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

	std::list<ptr<obj> > sighandlermcguffins;

	static property::value<std::string> signallistprop;

	static property::value<size_t> credtimeoutprop;

	impl(const ref<singletonapp::factorybaseObj> &app,
	     fdptr &connection,
	     uid_t uid,
	     mode_t mode) LIBCXX_INTERNAL;
	~impl() noexcept LIBCXX_INTERNAL;

	static void notrunning(uid_t uid)
		__attribute__((noreturn));

	static void installsighandler(const sighandler &handler,
				      std::list<ptr<obj> > &mcguffins,
				      sigset &signals)
 LIBCXX_INTERNAL;

};

#pragma GCC visibility pop

property::value<std::string>
singletonapp::impl::signallistprop(LIBCXX_NAMESPACE_WSTR
				   L"::singletonapp::termsigs",
				   "SIGINT SIGTERM SIGQUIT SIGHUP");

property::value<size_t>
singletonapp::impl::credtimeoutprop(LIBCXX_NAMESPACE_WSTR
				    L"::singletonapp::credtimeout", 30);

#pragma GCC visibility push(hidden)

class singletonapp::thr : public eventqueuemsgdispatcherObj {


	weaklist<obj> *threadsptr;
	bool *termflagptr;

public:

	thr() LIBCXX_INTERNAL {}
	~thr() noexcept LIBCXX_INTERNAL {}

	void run(fd &listensock,
		 fdptr &initinstance,
		 ref<singletonapp::factorybaseObj> &app) LIBCXX_HIDDEN;

private:

	void launch(const ref<singletonapp::factorybaseObj> &app,
		    const fd &initinstance) LIBCXX_INTERNAL;

#include "singletonapp.msgs.all.H"

};

#pragma GCC visibility pop

class LIBCXX_HIDDEN singletonapp::impl::mysighandler : public sighandlerObj {

 public:
	ref<thr> runthr;

	mysighandler(const ref<thr> &runthrArg)
		: runthr(runthrArg)
	{
	}

	~mysighandler() noexcept
	{
	}

	void signal(int signum)
	{
		runthr->stoplistening();
	}
};

// Derive a portmapper name based on the executable's dev+inode

std::string singletonapp::impl::svcname()
{
	std::ostringstream o;

	o << pidinfo().exe << "@app."
	  << httportmap::base::portmap_service;

	return o.str();
}

singletonapp::factorybaseObj::factorybaseObj()
{
}

singletonapp::factorybaseObj::~factorybaseObj() noexcept
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

	std::list<ptr<obj> > mcguffins;

	{
		sigset ss;

		installsighandler(implmysighandler, mcguffins, ss);

		ss.block();
	}

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
			std::string exename=pidinfo().exe;

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

	thrinstance=run(implthr, fd(connection), fdptr(fakeconn.first), app);

	sighandlermcguffins=mcguffins;
	connection=fakeconn.second;
}

void singletonapp::impl::notrunning(uid_t uid)
{
	throw EXCEPTION(gettextmsg(_txt("%1% is not running as %2%"),
				   pidinfo().exe,
				   passwd(uid)->pw_name));
}

singletonapp::impl::~impl() noexcept
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

void singletonapp::thr::run(fd &listensock,
			    fdptr &initinstance,
			    ref<singletonapp::factorybaseObj> &app)
{
	LOG_FUNC_SCOPE(singletonapp::logger);

	weaklist<obj> threads(weaklist<obj>::create());
	threadsptr= &threads;
	bool termflag=false;
	termflagptr= &termflag;

	launch(app, initinstance);
	initinstance=fdptr();

	struct pollfd pfd[2];

	{
		fd efd=msgqueue->getEventfd();

		pfd[0].fd=efd->getFd();

		efd->nonblock(true);

		listensock->nonblock(true);
		pfd[1].fd=listensock->getFd();
	}

	pfd[0].events=pfd[1].events=POLLIN;

	try {
		while (1)
		{
			if (!msgqueue->empty())
			{
				msgqueue->pop()->dispatch();
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
				msgqueue->getEventfd()->event();
		}
	} catch (const stopexception &e)
	{
	} catch (const exception &e)
	{
		LOG_FATAL(e);
		LOG_TRACE(e->backtrace);
	}
}

class singletonapp::destroycb : public destroyCallbackObj {

public:

	ref<thr> tptr;

	destroycb(const ref<thr> &ptrArg) : tptr(ptrArg) {}
	~destroycb() noexcept {}

	void destroyed() noexcept
	{
		tptr->terminated();
	}
};

void singletonapp::thr::launch(const ref<singletonapp::factorybaseObj> &app,
			       const fd &initinstance)
{
	LOG_FUNC_SCOPE(singletonapp::logger);

	try {
		ref<obj> lthr=app->newThread(initinstance,
					     ref<destroycb>
					     ::create(ref<thr>(this)));

		(*threadsptr)->push_back(lthr);

	} catch (const exception &e)
	{
		LOG_ERROR(e);
		LOG_TRACE(e->backtrace);
	}
}

void singletonapp::thr::dispatch(const terminated_msg &msg)
{
	for (auto b: **threadsptr)
	{
		if (!b.getptr().null())
			return;
	}

	throw stopexception();
}

void singletonapp::thr::dispatch(const stoplistening_msg &msg)
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

singletonapp::instanceObj::~instanceObj() noexcept
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

void singletonapp::installsighandler(const sighandler &handler,
				     std::list<ptr<obj> > &mcguffins)

{
	sigset dummy;

	impl::installsighandler(handler, mcguffins, dummy);
}

void singletonapp::impl::installsighandler(const sighandler &handler,
					   std::list<ptr<obj> > &mcguffins,
					   sigset &signals)

{
	std::list<std::string> signalnames;

	ctype(locale::base::global())
		.strtok_is(signallistprop.getValue(), signalnames);

	while (!signalnames.empty())
	{
		int signum=sigset::name2sig_orthrow(signalnames.front());

		signalnames.pop_front();

		signals += signum;

		mcguffins.push_back(sighandler::base::install(signum, handler));
	}
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

	pfd.fd=connection->getFd();
	pfd.events=POLLIN;

	if (::poll(&pfd, 1, impl::credtimeoutprop.getValue() * 1000) < 0 ||
	    (pfd.revents & POLLIN) == 0 ||
	    !connection->recv_credentials(uc))
	{
		errno=ETIMEDOUT;
		throw SYSEXCEPTION("singletonapp");
	}

	if ((sameuser && uc.uid != getuid())
	    || pidinfo().exe != portmapper->pid2exe(uc.pid))
	{
		errno=EPERM;
		throw SYSEXCEPTION("singletonapp");
	}

	if (connection->write("", 1) != 1)
	{
		errno=EIO;
		throw SYSEXCEPTION("singletonapp");
	}

	char dummy;

	if (::poll(&pfd, 1, impl::credtimeoutprop.getValue() * 1000) < 0 ||
	    (pfd.revents & POLLIN) == 0 ||
	    connection->read(&dummy, 1) != 1)
	{
		errno=ETIMEDOUT;
		throw SYSEXCEPTION("singletonapp");
	}

	connection->nonblock(false);
	return uc.uid;
}

singletonapp::processedObj::processedObj() : flag(false)
{
}

singletonapp::processedObj::~processedObj() noexcept
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
