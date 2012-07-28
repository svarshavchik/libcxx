/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fdlistenerobj.H"
#include "x/fdtimeoutobj.H"
#include "x/threads/run.H"
#include "x/threads/workerpool.H"
#include "x/ondestroy.H"
#include "x/timerfd.H"
#include "x/netaddr.H"
#include "x/sysexception.H"
#include "x/eventfd.H"

#include <poll.h>
#include <iomanip>
#include <iterator>
#include <condition_variable>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::fdlistenerImplObj);

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

fdlistenerImplObj::fdserverObj::fdserverObj()
{
}

fdlistenerImplObj::fdserverObj::~fdserverObj() noexcept
{
}

// A job that handles a single connection

class fdlistenerImplObj::listenerJobObj : public runthreadname,
					  virtual public obj {

	std::string name;

	// The server object that does whatever the connection needs to do
	ref<fdserverObj> server;

	// The file descriptor
	fd filedesc;

	// The termination file descriptor
	fd termfds;

	// Signal mask for the job
	sigset mask;

	std::string getName() const;

public:
	listenerJobObj(const std::string &nameArg,
		       const ref<fdserverObj> &serverArg,
		       const fd &filedescArg,
		       const fd &termfdsArg,
		       const sigset &maskArg) LIBCXX_HIDDEN;
	~listenerJobObj() noexcept LIBCXX_HIDDEN;

	void run() LIBCXX_HIDDEN;
};


fdlistenerImplObj::listenerJobObj
::listenerJobObj(const std::string &nameArg,
		 const ref<fdserverObj> &serverArg,
		 const fd &filedescArg,
		 const fd &termfdsArg,
		 const sigset &maskArg)
	: name(nameArg),
	  server(serverArg), filedesc(filedescArg),
	  termfds(termfdsArg)
{
}

fdlistenerImplObj::listenerJobObj::~listenerJobObj() noexcept
{
}

std::string fdlistenerImplObj::listenerJobObj::getName() const
{
	return name;
}

void fdlistenerImplObj::listenerJobObj::run()
{
	mask.setmask();
	server->run(filedesc, termfds);
}

std::string fdlistenerImplObj::listenerJobObj::getName() const;

// A mutex+condition variable that gets installed as a destroy callback
// for each connection thread. The condition variable gets tripped when the
// connection thread object gets destroyed.
//
// When the number of connections is at the maximum, the listener thread
// waits for the condition variable to get tripped.

class fdlistenerImplObj::serverDestroyCallbackObj
	: public destroyCallbackObj {

public:
	eventfd terminatefd;

	serverDestroyCallbackObj(const eventfd &terminatefdArg) LIBCXX_HIDDEN;
	~serverDestroyCallbackObj() noexcept LIBCXX_HIDDEN;

	void destroyed() noexcept LIBCXX_HIDDEN;
};


fdlistenerImplObj::serverDestroyCallbackObj
::serverDestroyCallbackObj(const eventfd &terminatefdArg)
	: terminatefd(terminatefdArg)
{
}

fdlistenerImplObj::serverDestroyCallbackObj::~serverDestroyCallbackObj() noexcept
{
}

void fdlistenerImplObj::serverDestroyCallbackObj::destroyed() noexcept
{
	terminatefd->event(1);
}

// The start argument to the listener thread. The start argument contains
// a pipe used to signal termination.

class fdlistenerImplObj::startArgObj : virtual public obj {

public:
	std::pair<fd, fd> pipe;
	ref<fdserverObj> server;

	startArgObj(const ref<fdserverObj> &serverArg) LIBCXX_HIDDEN;
	~startArgObj() noexcept LIBCXX_HIDDEN;
};

fdlistenerImplObj::startArgObj::startArgObj(const ref<fdserverObj> &serverArg)
	: pipe(fd::base::pipe()), server(serverArg)
{
}

fdlistenerImplObj::startArgObj::~startArgObj() noexcept
{
}


fdlistenerImplObj::listenon::listenon(int portnum)
{
	netaddr::create("", portnum, SOCK_STREAM)->bind(fdlist, true);
}

fdlistenerImplObj::listenon::listenon(const std::list<int> &ports)

{
	netaddr::create("", ports, SOCK_STREAM)->bind(fdlist, true);
}

fdlistenerImplObj::listenon::listenon(const std::string &ports)
{
	netaddr::create("", ports, SOCK_STREAM)->bind(fdlist, true);
}

fdlistenerImplObj::listenon::listenon(const std::list<fd> &fdlistArg)
 : fdlist(fdlistArg)
{
}

fdlistenerImplObj::listenon::~listenon() noexcept
{
}

fdlistenerImplObj::fdlistenerImplObj(const fdlistenerImplObj::listenon
				     &listenonArg,
				     size_t maxthreadsArg,
				     size_t minthreadsArg,
				     const std::string &jobnameArg,
				     const property::propvalue &propnameArg
				     )
	: listeners(listenonArg.fdlist),
	  maxthreads(maxthreadsArg),
	  minthreads(minthreadsArg),
	  jobname(jobnameArg),
	  propname(propnameArg),
	  mask(sigset::current())
{
	LOG_TRACE("Setting listening sockets to non-blocking mode");
	for (auto socket:listeners)
		socket->nonblock(true);

	LOG_TRACE("Starting to listen on the sockets");
	fd::base::listen(listeners);
	LOG_TRACE("Listening for new connections");
}

fdlistenerImplObj::~fdlistenerImplObj() noexcept
{
}

void fdlistenerImplObj::start(const ref<fdserverObj> &server)
{
	std::lock_guard<std::mutex> lock(objmutex);

	sigset::block_all block_sigs;
	// The listener thread has all signals blocked

	auto pipearg=ref<startArgObj>::create(server);

	server_thread=LIBCXX_NAMESPACE::run(ref<fdlistenerImplObj>(this),
					  pipearg);

	stoppipe=pipearg->pipe.second;
}

void fdlistenerImplObj::stop()
{
	std::lock_guard<std::mutex> lock(objmutex);

	fdptr w(stoppipe.getptr());

	stoppipe=fdptr();

	if (!w.null())
		w->close();
}

void fdlistenerImplObj::wait()
{
	server_thread_t thr=({
			std::lock_guard<std::mutex> lock(objmutex);

			auto cpy=server_thread;

			server_thread=server_thread_t();

			cpy;
		});

	if (!thr.null())
	{
		LOG_TRACE("Waiting for file descriptor listener to terminate");
		thr->wait();
		LOG_TRACE("File descriptor listener terminated");
	}
}

void fdlistenerImplObj::run(const ref<startArgObj> &startarg)
{
	LOG_TRACE("Listener thread running");

	try {
		runimpl(startarg);
	} catch (const exception &e)
	{
		LOG_ERROR(e);
		LOG_TRACE(e->backtrace);
	}
	LOG_TRACE("Listener thread terminating");
}

void fdlistenerImplObj::runimpl(const ref<startArgObj> &startarg)
{
	// Allocate a poll array for the given # of file descriptors, plus 2
	// for the stop signal pipe and the socket termination signaller.

	size_t n=listeners.size()+2;

	eventfd terminated_eventfd=eventfd::create();

	struct pollfd pfd[n];

	{
		size_t j=0;

		for (std::list<fd>::iterator
			     b=listeners.begin(),
			     e=listeners.end(); b != e; ++b, ++j)
		{
			pfd[j].fd=(*b)->getFd();
			pfd[j].events=POLLIN;
		}

		// Last one is the stop signal pipe

		pfd[j].fd=startarg->pipe.first->getFd();
		pfd[j].events=POLLIN;

		++j;
		pfd[j].fd=terminated_eventfd->getFd();
		pfd[j].events=POLLIN;

	}
	terminated_eventfd->nonblock(true);

	const size_t stop_fd=n-2;
	const size_t terminate_fd=n-1;

	workerpool<> jobthreads(workerpool<>::create(minthreads,
						     maxthreads,
						     jobname,
						     propname));

	ref<serverDestroyCallbackObj>
		destroysigref(ref<serverDestroyCallbackObj>
			      ::create(terminated_eventfd));

	// Every maxthreads connections, check whether to stop listening
	// until some existing connection terminates.

	size_t pending_chk_count=jobthreads->getMaxthreads();

	while(1)
	{
		if (pending_chk_count == 0)
		{
			pending_chk_count=jobthreads->getMaxthreads();

			if (pending_chk_count <= 0)
				pending_chk_count=1;

			size_t cnt=jobthreads->getPendingCount();

			if (cnt >= pending_chk_count)
			{
				pending_chk_count=0;

				if (poll(pfd+stop_fd, 2, -1) < 0)
					if (errno != EINTR)
						throw SYSEXCEPTION("poll");

				if (pfd[stop_fd].revents &
				    (POLLIN|POLLHUP))
					break; // Stop pipe signalled

				if (pfd[terminate_fd].revents &
				    (POLLIN|POLLHUP))
					terminated_eventfd->event();

				continue;
			}

			pending_chk_count -= cnt;
		}

		if (poll(pfd, n, -1) < 0)
		{
			if (errno != EINTR)
				throw SYSEXCEPTION("poll");
			continue;
		}

		if (pfd[terminate_fd].revents & (POLLIN|POLLHUP))
			terminated_eventfd->event();

		size_t j=0;

		for (auto socket:listeners)
		{
			if (!(pfd[j++].revents & POLLIN))
				continue;

			sockaddrptr peeraddr;
			bool accept_called=true;

			try {
				fdptr newsock=socket->accept(peeraddr);

				if (newsock.null())
					continue;

				accept_called=false;

				auto newthr(ref<listenerJobObj>::create
					    (jobname,
					     startarg->server,
					     newsock,
					     startarg->pipe.first,
					     mask));

				newthr->addOnDestroy(destroysigref);
				--pending_chk_count;

				jobthreads->run(newthr);
			} catch (const exception &e)
			{
				if (!accept_called)
					throw;
			}

		}

		if (pfd[stop_fd].revents &
		    (POLLIN|POLLHUP)) // Stop pipe signalled
			break;
	}
}

#if 0
{
#endif
}
