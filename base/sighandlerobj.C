/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sighandler.H"
#include "x/exception.H"
#include "x/singleton.H"
#include "x/signalfd.H"
#include "x/logger.H"

#include <map>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

// Private implementation stuff

namespace {
#if 0
}
#endif

// Define a class that holds the list of installed signal handlers,
// the signal file descriptor, and the file descriptor that's used
// to terminate the thread.

class handlersmeta {
public:

	typedef std::multimap<int, sighandler_t> handlers_t;

	handlers_t handlers;

	signalfdptr sigfd;

	fdptr eoffd;
};

// A mutex-protected handlersmeta. The signal handler maintains a
// reference to it, as well as each live signal handler mcguffin.

class handlersObj : virtual public obj {

public:
	handlersObj()=default;
	~handlersObj()=default;

	mpobj<handlersmeta> meta;

	typedef handlersmeta::handlers_t handlers_t;
};

// Thread that reads the signal file descriptor, and dispatches signal
// events
class threadimplObj : virtual public obj {

public:
	threadimplObj()=default;
	~threadimplObj()=default;

	void run(const ref<handlersObj> &start_arg);
};

// Define a class that holds a reference to the signal thread,
// the list of installed handlers, and the file descriptor that
// signals the signal thread to stop.

class sigthreadmeta {

public:
	runthreadptr<void> threadret;
	ptr<handlersObj> handlerlist;
	fdptr eoffd;
};

// A mutex-protected sigthreadmeta. The signal thread keeps a reference
// to it, as well as it's being available as a global singleton.

class sigthreadObj : virtual public obj {

public:
	mpobj<sigthreadmeta> sigthreadmetainfo;

	sigthreadObj()=default;
	~sigthreadObj()=default;
};

static singleton<sigthreadObj> impl;

// This is the mcguffin for an installed signal handler
// It maintains a reference to a sigthread, as well as the iterator
// for the signal handler in handlersmeta's list of installed handlers.
// Its destructor removes the signal handler, then stops the thread,
// if there are no other signal handlers.

class mcguffinObj : virtual public obj {

public:
	ptr<sigthreadObj> p;
	handlersmeta::handlers_t::iterator iter;

	mcguffinObj()=default;

	~mcguffinObj()
	{
		if (p.null())
			return; // Wasn't really installed

		runthreadptr<void> thread=stop();

		if (!thread.null())
			thread->wait();
	}

	runthreadptr<void> stop() noexcept
	{
		mpobj<sigthreadmeta>::lock
			metalock(p->sigthreadmetainfo);

		ptr<handlersObj> handlerlist=metalock->handlerlist;

		mpobj<handlersmeta>::lock lock(handlerlist->meta);

		int signum=iter->first;

		auto &h=lock->handlers;

		h.erase(iter);

		if (h.find(signum) == h.end())
			// No more handlers for this signal
			lock->sigfd->uncapture(signum);

		if (h.empty())
		{
			// No more handlers, stop the thread

			runthreadptr<void> thread=
				metalock->threadret;

			metalock->threadret=runthreadptr<void>();
			metalock->eoffd=fdptr();

			return thread;
		}

		return runthreadptr<void>();
	}
};

#if 0
{
#endif
}

ref<obj> install_sighandler(int signum,
			    const sighandler_t &handler)

{
	// mcguffin should be the last to go out of scope, in case an exception
	// gets thrown in this function.

	ptr<mcguffinObj> mcguffin;

	ptr<sigthreadObj> sigthread=impl.get();

	if (sigthread.null())
		throw EXCEPTION("Attempt to install a signal handler during process termination");

	mpobj<sigthreadmeta>::lock
		metalock(sigthread->sigthreadmetainfo);

	sigthreadmeta &meta=*metalock;

	// First time in, create the handlers

	if (meta.handlerlist.null())
		meta.handlerlist=ptr<handlersObj>::create();

	mpobj<handlersmeta>::lock
		handlersmetalock(meta.handlerlist->meta);

	auto &m=*handlersmetalock;

	// First time in, create the signal file descriptor

	signalfdptr sigfd=m.sigfd;

	if (!sigfd)
	{
		sigfd=signalfd::create();
		sigfd->nonblock(true);
	}

	if (m.handlers.find(signum) == m.handlers.end())
		sigfd->capture(signum);

	mcguffin=ptr<mcguffinObj>::create();

	mcguffin->iter=m.handlers.insert(std::make_pair(signum, handler));
	mcguffin->p=sigthread;

	// First time in, start the thread

	if (!meta.threadret)
	{
		auto [first, second]=fd::base::pipe();

		first->nonblock(true);
		m.eoffd=first;

		auto thr=ref<threadimplObj>::create();

		meta.threadret=sigset::block_all().run(thr, meta.handlerlist);
		meta.eoffd=second;
	}
	m.sigfd=sigfd; // Save it, if it were created.

	return mcguffin;
}

LOG_FUNC_SCOPE_DECL(sighandler, sighandler_logger);

void threadimplObj::run(const ref<handlersObj> &h)
{
	LOG_FUNC_SCOPE(sighandler_logger);

	auto [sigfd, eoffd]=({
			mpobj<handlersmeta>::lock lock(h->meta);

			handlersmeta &m= *lock;

			std::make_pair(m.sigfd, m.eoffd);
		});

	struct pollfd pfd[2];

	pfd[0].fd=sigfd->get_fd();
	pfd[1].fd=eoffd->get_fd();

	pfd[0].events=pfd[1].events=POLLIN;

	while (1)
	{
		signalfd::base::getsignal_t sig=sigfd->getsignal();

		if (sig.ssi_signo)
		{
			// Retrieve the list of installed handlers, for this
			// signal

			std::list<sighandler_t> handlers;

			{
				mpobj<handlersmeta>::lock lock(h->meta);

				for (auto [first, second]=lock->handlers
					     .equal_range(sig.ssi_signo);
				     first != second;
				     ++first)
				{
					handlers.push_back(first->second);
				}
			}

			while (!handlers.empty())
			{
				try {
					handlers.front()(sig.ssi_signo);
				} catch (const exception &e)
				{
					LOG_ERROR(e);
					LOG_TRACE(e->backtrace);
				}
				handlers.pop_front();
			}
			continue;
		}

		if (::poll(pfd, 2, -1) < 0)
			continue;

		if (pfd[1].revents & (POLLIN|POLLHUP))
			break;
	}
}

#if 0
{
#endif
}
