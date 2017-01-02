/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_singletonapp_H
#define x_singletonapp_H

#include <x/ptr.H>
#include <x/obj.H>
#include <x/threads/run.H>
#include <x/fdfwd.H>
#include <x/stoppable.H>
#include <x/httportmapfwd.H>
#include <x/sighandler.H>
#include <x/threadmsgdispatcher.H>
#include <x/logger.H>

#include <type_traits>
#include <unistd.h>
#include <sys/types.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A singleton application

//! Your main() instantiates some reference-counted object that implements
//! a newThread() method.
//!
//! newThread() returns a reference to a reference-counted object that
//! optionally
//! implements the \ref stoppable "stoppable interface", and which has a
//! a run() method that takes a single parameter which is a file descriptor.
//!
//! If this is the initial process, newThread() gets called, and the returned
//! object's run() method gets invoked.
//!
//! If this process is already running, the initial process's object's
//! newThread()
//! gets called again, and another execution thread invokes the returned
//! object's run() method.
//!
//! newThread() must return a new object for each call, and no other references
//! to the object must exist, outside of the executing thread. The object
//! returned by newThread() must go out of scope and get destroyed, when its
//! run() method returns.
//!
//! In all cases, create() returns an \ref instance "instance" reference,
//! a reference to an object that contains a socket file descriptor that's
//! connected to the socket file descriptor received by the run() method.
//!
//! \par Application startup and termination
//!
//! In main(), after create() returns, the process should send whatever
//! application-specific process startup information needs to be sent to the
//! thread running on the singleton side, then destroy the
//! \ref instance "instance" reference
//! and terminate.
//!
//! In the first process, the one that's running the singleton threads,
//! \ref instance "instance"'s destructor waits for all threads to terminate,
//! before the instance object gets destroyed.
//!
//! All additional processes do not have any threads running, and the
//! instance object gets destroyed without delay.
//!
//! \par Signals
//!
//! The first singleton creates a listening socket in <em>/tmp</em>.
//! The socket gets registered with the \ref httportmap "portmapper", and the
//! other processes connect to the registered socket, and that's how the magic
//! happens.
//!
//! Explicitly removing the <em>/tmp</em> socket, when the singleton
//! stops, is required to avoid polluting the <em>/tmp</em>
//! directory. For that reason, create() blocks the following signals:
//! <tt>SIGINT</tt>,
//! <tt>SIGHUP</tt>,
//! <tt>SIGTERM</tt>,
//! <tt>SIGQUIT</tt>, and installs a
//! \ref sighandler "signal handler".
//!
//! When any of those signals are received, the singleton process stops
//! accepting any more connections
//! from other application processes, and stops creating
//! new threads, however the singleton process cannot terminate until all
//! existing threads terminate. It is expected that upon receipt of this signal
//! all existing threads will stop promptly, at which point the entire
//! singleton process terminates:
//!
//! - if the thread object that's returned by newThread() also multiply
//! inherits from stoppableObj, each thread's stop() method gets invoked.
//!
//! - alternatively, signal handlers may be separately installed, and perform
//! the necessary duties that result in the termination of all threads.
//! installsighandler() is a convenience function that installs the given
//! handler for all signals that result in the singleton process stopping
//! accepting any new connections.
//!
//! \par Peer validation
//!
//! The socket that effects singleton instances is a public socket that anyone
//! can connect to, and it's even publicly registered. Some minimum security
//! may be implemented by invoking validate_peer(), which verifies the
//! connected peer's userid, and, for the most part that it's the same
//! application that invoked validate_peer().

class singletonapp {

	//! Internal logger object

	LOG_CLASS_SCOPE LIBCXX_PUBLIC;

public:

	class impl;

	class thr;

	class destroycb;

private:
	//! Singleton thread factory

	//! Each application process, including the initial one, results in
	//! newThread() getting invoked.

	class factorybaseObj : virtual public obj {

	public:
		//! Constructor
		factorybaseObj();

		//! Destructor
		~factorybaseObj();

		//! Construct a new thread for the singleton process.

		//! The subclass must implement newThread(). newThread() itself
		//! must be thread-safe. The subclass must return a new
		//! thread object, but not start it. The thread object gets
		//! started, and its starting argument is a reference to a
		//! \ref fd "file descriptor".
		virtual ref<obj> newThread(const fd &fdArg,
					   const ref<obj::destroyCallbackObj>
					   &cb)=0;
	};

	template<typename ref_type> class start_new_thread;
	template<typename ref_type, bool> class start_new_plain_or_dispatcher_thread;

	//! Invoke an application's newThread()

	template<typename ref_type> class factoryObj : public factorybaseObj {

		//! An application object that implements newThread()

		ref_type app;

	public:
		//! Constructor
		factoryObj(//! An application object that implements its own newThread method
			   const ref_type &appArg) : app(appArg)
		{
		}

		//! Destructor
		~factoryObj()
		{
		}

		//! Call the appliaction's newThread()

		ref<obj> newThread(//! Socket connection
				   const fd &fdArg,

				   //! Destructor callback to attach to the new thread object
				   const ref<obj::destroyCallbackObj> &cb)
		{
			auto obj=app->newThread();
			obj->ondestroy([cb]{cb->destroyed();});

			start_new_thread<decltype(obj)>::start(obj, fdArg);

			return obj;
		}
	};

public:
	//! An instance of a singleton process.

	//! A reference to this object gets returned by create(). The initial
	//! process starts spawning thread's for each instance of the
	//! application. In all cases, \ref connection is a connected socket
	//! with the corresponding thread in the singleton process.
	class LIBCXX_HIDDEN instanceObj : virtual public obj {

	public:
		//! A connected socket to the singleton process's thread.

		//! This object must be before implinstance.
		//!

		fdptr connection;
	private:
		//! Internal implementation stuff
		ref<impl> implinstance;

	public:
		//! The constructor is private. The whole class is LIBCXX_HIDDEN.

		instanceObj(const ref<factorybaseObj> &factory, uid_t uid,
			    mode_t mode);

		//! Destructor

		//! In the process that runs all the singleton processes, wait
		//! for all of them to stop. In other processes, there are no
		//! threads to wait for.

		~instanceObj();

		friend class singletonapp;
	};

	//! A reference to the singleton instance object that's returned by create().

	typedef ref<instanceObj> instance;

	//! Create a singleton instance.

	//! If a singleton process is already running as the user specified by
	//! the uid parameter, a new thread gets constructed by the singleton
	//! process's
	//! \ref factorybaseObj "factory", and started. Otherwise, the given
	//! factory's newThread() method gets invoked to construct the first
	//! singleton thread, which then gets started.
	//!
	//! If a singleton process is not already running, and this process's
	//! real userid is not the uid parameter, an %exception gets thrown,
	//! since the singleton process cannot be started.
	//!
	//! To implement systemwide application singletons, that typically run
	//! as root, pass a 0 for a uid, and start this process
	//! as root, otherwise, leave it as the default.
	//!
	//! \return a reference to an \ref instance "instance" object with
	//! a socket connection to the singleton thread, wherever it is.

	template<typename objType, typename baseClass>
	static instance create(//! The application
			       const ref<objType, baseClass> &app,

			       //! The singleton must be running as this userid
			       uid_t uid=getuid(),

			       //! If this instance becomes the singleton, the permission mode for the socket

			       //! Should be set to 0755 for singletons that serve all uids.
			       mode_t mode=0700)
	{
		return create_internal(ref< factoryObj< ref<objType, baseClass>
							>
			>::create(app), uid, mode);
	}

private:

	//! Internal implementation of create().

	static instance
	create_internal(//! Singleton thread factory
			const ref<factorybaseObj> &factory,

			//! The singleton must be running as this userid
			uid_t uid,

			//! The permissions on the singleton's listening socket
			mode_t mode
			) LIBCXX_PUBLIC;
public:
	//! Attach a handler for all the blocked signals

	//! To effect filesystem socket cleanup, certain termination signals
	//! get preemptively blocked by create(). A handler may be installed
	//! for these signals, using this convenience function, so that it
	//! can take the necessary action to direct any singleton threads to
	//! terminate.
	static void installsighandler(//! Signal handler
				      const sighandler &handler,

				      //! Registration mcguffins returned by
				      //! \ref sighandlerBase::install "INSERT_LIBX_NAMESPACE::sighandler::base::install()".

				      std::list<ptr<obj> > &mcguffins)
;

	//! Validate singleton connection

	//! validate_peer() is typically invoked:
	//!
	//! - after \ref create "INSERT_LIBX_NAMESPACE::singletonapp::create"() returns,
	//! using the connection file descriptor in the
	//! \ref instance "instance".
	//!
	//! - in the started thread's <em>run</em>() method, using
	//! the thread's starting argument, the singleton side of the connected
	//! file descriptor.
	//!
	//! validate_peer() takes the following actions:
	//!
	//! - make the socket temporarily non-blocking
	//!
	//! - send the process's system credentials, its userid, group id, and
	//! process id, and wait up to 30 seconds for the peer to reciprocate
	//! in turn (its actually two timeouts, a maximum of 15 seconds each).
	//! The peer is also expected to invoke validate_peer() on the
	//! peer's side, to do the same.
	//!
	//! - verify that the other process is running the same executable
	//! binary that this process is running. This is not a 100% bulletproof
	//! check, the primary authentication is by userid. The check on the
	//! executable binary should be mostly considered to be a sanity check.
	//!
	//! - return the socket to a blocking state
	//!
	//! \return the peer's userid, which would typically be the same
	//! userid as this process's userid, except in systemwide singletons'
	//! case.
	//!
	//! An %exception gets thrown if the peer does not respond in 30 seconds
	//! or if it fails validation for any reason.
	//!
	//! \note
	//! validate_peer() announces the executable's pathname via
	//! the portmapper.

	static uid_t validate_peer(//! Connection with the peer
				   const fd &connection,

				   //! Require that the peer must have the same
				   //! real userid, when create() receives
				   //! the starting process's real userid.
				   //!
				   //! For systemwide singletons, with a fixed
				   //! userid passed to create(), set this to
				   //! \c false, and check the returned uid
				   //! yourself.
				   bool sameuser=true);

	//! Validate a singleton connection, invoked by the existing instance

	//! Long-running daemons should invoke
	//! \c INSERT_LIBX_NAMESPACE::httportmap::base::regpid2exe(), at
	//! startup, then pass it to this overloaded version. On
	//! systems with prelink(1), long running daemons may find their
	//! /proc/self/exe disappear. Must register with the portmapper prior
	//! to this.

	static uid_t validate_peer(const httportmap &portmapper,
				   const fd &connection,
				   bool sameuser=true);
	// --------------------------

	class processedObj;

	//! Convenience typedef
	typedef ref<processedObj> processed;

	//! Convenience typedef
	typedef ptr<processedObj> processedptr;

private:
	template<typename appObjClass, typename appBaseClass,
		 typename factory_type,
		 typename argsClass, typename argsBase,
		 typename ret_args>
	class managedSingletonFactoryObj;

	template<typename objClass, typename baseClass,
		 typename args_type,
		 bool=std::is_base_of<threadmsgdispatcherObj, objClass>
		 ::value> class managedSingletonAppContainerObj;

	template<typename factory_type, typename args_type>
	class managed_ret_type;

	template<typename factory_type, typename args_type, bool>
	class managed_ret_type_impl;

	template<typename factory_type, typename impl_type,
		 typename args_type, typename ret_type> class managed_helper;
public:

	//! A managed application singleton

	//! This is a wrapper for create() that provides a higher-level API, that
	//! introduces the concept of singleton application parameters and return
	//! values. Both are reference-counted objects, with a serialization function.
	//!
	//! \return a std::pair, containing the return object, and the userid of the
	//! singleton process that processed the parameters.

	template<typename factory_type, typename args_type>
	static std::pair<typename managed_ret_type<factory_type, args_type>
			 ::ret_t, uid_t>
	managed(factory_type &&factory, const args_type &args,
		uid_t singletonappuid=getuid(),
		mode_t singletonsockmode=0700,
		bool mustbesameuser=true);
};

//! newThread() must return a ref. Enforce using a specialization.

//! start_new_thread() is undefined otherwise.

template<typename obj_type, typename baseClass>
class singletonapp::start_new_thread<ref<obj_type, baseClass> >
	: public singletonapp
::start_new_plain_or_dispatcher_thread<ref<obj_type, baseClass>,
				       std::is_base_of<threadmsgdispatcherObj,
						       obj_type>::value> {};


//! The singleton is not based on threadmsgdispatcher, gets started via run()

template<typename ref_type>
class singletonapp::start_new_plain_or_dispatcher_thread<ref_type, false> {

public:

	//! Capture and log singleton threads' uncaught exceptions

	//! Because nobody else would.
	//!
	class newThreadObj : virtual public obj {

	public:
		//! The actual implementation

		ref_type impl;

		//! Constructor
		newThreadObj(const ref_type &implArg)
			: impl(implArg)
		{
		}

		//! Destructor
		~newThreadObj() {
		}

		//! Invoke new singleton thread, capture exceptions

		void run(const fd &runArg)
		{
			try {
				impl->run(runArg);
			} catch (const exception &e)
			{
				LOG_ERROR(e);
				LOG_TRACE(e->backtrace);
			}
		}
	};

	//! Invoke the newly-created thread's run().

	static void start(const ref_type &obj,
			  const fd &runArg)
	{
		run(ref<newThreadObj>::create(obj), runArg);
	}
};

//! The singleton is based on threadmsgdispatcher, gets started via start_threadmsgdispatcher()

template<typename ref_type>
class singletonapp::start_new_plain_or_dispatcher_thread<ref_type, true> {

public:

	//! Capture and log singleton threads' uncaught exceptions

	//! Because nobody else would.
	//!
	class newThreadObj : virtual public obj {

	public:
		//! The actual implementation

		ref_type impl;

		//! Constructor
		newThreadObj(const ref_type &implArg)
			: impl(implArg)
		{
		}

		//! Destructor
		~newThreadObj() {
		}

		//! Invoke new singleton thread, capture exceptions

		void run(ptr<obj> &threadmsgdispatcher_mcguffin,
			 const fd &runArg)
		{
			try {
				impl->run(threadmsgdispatcher_mcguffin, runArg);
			} catch (const exception &e)
			{
				LOG_ERROR(e);
				LOG_TRACE(e->backtrace);
			}
		}
	};

	//! Invoke the newly-created thread's run().

	static void start(const ref_type &obj,
			  const fd &runArg)
	{
		start_threadmsgdispatcher(ref<newThreadObj>::create(obj),
					  runArg);
	}
};


#if 0
{
#endif
}
#endif
