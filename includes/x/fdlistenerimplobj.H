/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fdlistenerimplobj_H
#define x_fdlistenerimplobj_H

#include <x/fd.H>
#include <x/threads/run.H>
#include <x/weakptr.H>
#include <x/sigset.H>
#include <x/logger.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Listen for connections on a server socket, start a thread for each one.

class fdlistenerImplObj : virtual public obj {

	//! The logger object.

	LOG_CLASS_SCOPE;

	//! Listening sockets
	std::list<fd> listeners;

	//! Pipe used to signal all processes to stop
	weakptr<fdptr> stoppipe;

	//! The thread return type

	typedef runthreadptr<void> server_thread_t;

	//! The thread return
	server_thread_t server_thread;

	//! Thread configuration
	size_t maxthreads;

	//! Thread configuration
	size_t minthreads;

	//! Internal names of started threads
	std::string jobname;

	//! Property that specifies the default values for thread configuration parameters
	std::string propname;

	//! The thread's signal mask

	sigset mask;

	class startArgObj;

	class listenerJobObj;

	class serverDestroyCallbackObj;

public:

	//! fdlistenerimplObj's callback.

	class fdserverObj : virtual public obj {

	public:
		//! Constructor
		fdserverObj();

		//! Destructor
		~fdserverObj() noexcept;

		//! Implement socket server in the subclass.
		virtual void run(const fd &socket, //!< The new socket
				 const fd &termpipe //!< The termination filedesc
				 )=0;
	};


	//! Convenience class used to specify what server socket to listen on

	//! This class allows the server socket to be specified in a variety
	//! of ways: explicitly, as an \c int, or as a
	//! list of ports.

	class listenon {

		//! When all is said a done, we wind up with a list of sockets
		std::list<fd> fdlist;
	public:

		friend class fdlistenerImplObj;

		//! Listen on an explicit port
		listenon(int portnum);

		//! Listen on an explicit list of ports
		listenon(const std::list<int> &ports);

		//! Listen on an explicit port
		listenon(const std::string &portname);

		//! These sockets have already been created and bound
		listenon(const std::list<fd> &fdlistArg);

		~listenon() noexcept;
	};

	//! Constructor

	fdlistenerImplObj(//! Which file descriptors to listen on

			  const listenon &listenonArg,

			  //! Maximum number of server threads
			  size_t maxthreads=10,

			  //! Minimum number of server threads to precreate
			  size_t minthreads=0,

			  //! Name of started threads, for logging purposes

			  const std::string &jobnameArg="server",

			  //! Property name.

			  //! If this is specified maxthreads,
			  //! minthreads, and jobname obtained from application
			  //! properties
			  //! propname::max,
			  //! propname::min, and
			  //! propname::name, and the above
			  //! parameters specify
			  //! the default values, if not set in
			  //! the property file.
			  const std::string &propname="");

	//! Destructor
	~fdlistenerImplObj() noexcept;

	//! Start the listener
	void start(//! The server for each accepted socket
		   const ref<fdserverObj> &server);

	//! Signal the listener to stop

	//! The listener is signaled to stop. Invoke wait() to wait until
	//! the listener stops.

	void stop();

	//! The listener thread
	void run(const ref<startArgObj> &startArg);

	//! Wait for the listener to terminate
	void wait();

private:
	//! The listener thread
	void runimpl(const ref<startArgObj> &startArg) LIBCXX_INTERNAL;

};

#if 0
{
#endif
}
#endif
