/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sighandler.H"
#include "x/exception.H"
#include "x/singleton.H"
#include "x/signalfd.H"

#include <map>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::sighandlerObj);

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

sighandlerObj::sighandlerObj()
{
}

sighandlerObj::~sighandlerObj() noexcept
{
}

// Private implementation stuff

class sighandlerObj::priv {

 public:

	// Define a class that holds the list of installed signal handlers,
	// the signal file descriptor, and the file descriptor that's used
	// to terminate the thread.

	class handlersmeta {
	public:

		typedef std::multimap<int, sighandler> handlers_t;

		handlers_t handlers;

		signalfdptr sigfd;

		fdptr eoffd;
	};

	// A mutex-protected handlersmeta. The signal handler maintains a
	// reference to it, as well as each live signal handler mcguffin.

	class handlers : virtual public obj {

	public:
		handlers() LIBCXX_INTERNAL {}
		~handlers() noexcept LIBCXX_INTERNAL {}

		mpobj<handlersmeta> meta;

		typedef handlersmeta::handlers_t handlers_t;
	};

	// Thread that reads the signal file descriptor, and dispatches signal
	// events
	class threadimpl : virtual public obj {

	public:
		threadimpl() LIBCXX_HIDDEN {}
		~threadimpl() noexcept LIBCXX_HIDDEN {}

		void run(const ref<handlers> &start_arg)
			LIBCXX_HIDDEN;
	};

	// Define a class that holds a reference to the signal thread,
	// the list of installed handlers, and the file descriptor that
	// signals the signal thread to stop.

	class sigthreadmeta {

	public:
		runthreadptr<void> threadret;
		ptr<handlers> handlerlist;
		fdptr eoffd;
	};

	// A mutex-protected sigthreadmeta. The signal thread keeps a reference
	// to it, as well as it's being available as a global singleton.

	class sigthread : virtual public obj {

	public:
		mpobj<sigthreadmeta> sigthreadmetainfo;

		sigthread() LIBCXX_HIDDEN {}
		~sigthread() noexcept LIBCXX_HIDDEN {}
	};

	static singleton<sigthread> impl;

	// This is the mcguffin for an installed signal handler
	// It maintains a reference to a sigthread, as well as the iterator
	// for the signal handler in handlersmeta's list of installed handlers.
	// Its destructor removes the signal handler, then stops the thread,
	// if there are no other signal handlers.

	class mcguffin : virtual public obj {

	public:
		ptr<sigthread> p;
		handlersmeta::handlers_t::iterator iter;

		mcguffin() LIBCXX_HIDDEN {}
		~mcguffin() noexcept LIBCXX_HIDDEN
		{
			if (p.null())
				return; // Wasn't really installed

			runthreadptr<void> thread=stop();

			if (!thread.null())
				thread->wait();
		}

		runthreadptr<void> stop() noexcept LIBCXX_HIDDEN
		{
			mpobj<sigthreadmeta>::lock
				metalock(p->sigthreadmetainfo);

			ptr<handlers> handlerlist=metalock->handlerlist;

			mpobj<handlersmeta>::lock lock(handlerlist->meta);

			int signum=iter->first;

			handlers::handlers_t &h=lock->handlers;

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
};

singleton<sighandlerObj::priv::sigthread> sighandlerObj::priv::impl;

ref<obj> sighandlerBase::install(int signum,
				 const sighandler &handler)

{
	// mcguffin should be the last to go out of scope, in case an exception
	// gets thrown in this function.

	ptr<sighandlerObj::priv::mcguffin> mcguffin;

	ptr<sighandlerObj::priv::sigthread> sigthread
		=sighandlerObj::priv::impl.get();

	if (sigthread.null())
		throw EXCEPTION("Attempt to install a signal handler during process termination");

	mpobj<sighandlerObj::priv::sigthreadmeta>::lock
		metalock(sigthread->sigthreadmetainfo);

	sighandlerObj::priv::sigthreadmeta &meta=*metalock;

	// First time in, create the handlers

	if (meta.handlerlist.null())
		meta.handlerlist=ptr<sighandlerObj::priv::handlers>::create();

	mpobj<sighandlerObj::priv::handlersmeta>::lock
		handlersmetalock(meta.handlerlist->meta);

	sighandlerObj::priv::handlersmeta &m=*handlersmetalock;

	// First time in, create the signal file descriptor

	signalfdptr sigfd=m.sigfd;

	if (sigfd.null())
	{
		sigfd=signalfd::create();
		sigfd->nonblock(true);
	}

	if (m.handlers.find(signum) == m.handlers.end())
		sigfd->capture(signum);

	mcguffin=ptr<sighandlerObj::priv::mcguffin>::create();

	mcguffin->iter=m.handlers.insert(std::make_pair(signum, handler));
	mcguffin->p=sigthread;

	// First time in, start the thread

	if (meta.threadret.null())
	{
		std::pair<fd, fd> pipe=fd::base::pipe();

		pipe.first->nonblock(true);
		m.eoffd=pipe.first;

		ref<sighandlerObj::priv::threadimpl>
			thr=ref<sighandlerObj::priv::threadimpl>::create();

		meta.threadret=sigset::block_all().run(thr, meta.handlerlist);
		meta.eoffd=pipe.second;
	}
	m.sigfd=sigfd; // Save it, if it were created.

	return mcguffin;
}

void sighandlerObj::priv::threadimpl::run(const ref<handlers> &h)
{
	LOG_FUNC_SCOPE(sighandlerObj::logger);

	std::pair<signalfd, fd> startupfd=({
			mpobj<handlersmeta>::lock lock(h->meta);

			handlersmeta &m= *lock;

			std::make_pair(m.sigfd, m.eoffd);
		});

	struct pollfd pfd[2];

	pfd[0].fd=startupfd.first->getFd();
	pfd[1].fd=startupfd.second->getFd();

	pfd[0].events=pfd[1].events=POLLIN;

	while (1)
	{
		signalfd::base::getsignal_t sig=startupfd.first->getsignal();

		if (sig.ssi_signo)
		{
			// Retrieve the list of installed handlers, for this
			// signal

			std::list<sighandler> handlers;

			{
				mpobj<handlersmeta>::lock lock(h->meta);

				for (std::pair<handlersmeta::handlers_t
					       ::iterator,
					       handlersmeta::handlers_t
					       ::iterator>
					     iters=lock->handlers
					     .equal_range(sig.ssi_signo);
				     iters.first != iters.second;
				     ++iters.first)
				{
					handlers.push_back(iters.first->second);
				}
			}

			while (!handlers.empty())
			{
				try {
					handlers.front()->signal(sig.ssi_signo);
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
