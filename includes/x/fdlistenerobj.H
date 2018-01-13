/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fdlistenerobj_H
#define x_fdlistenerobj_H

#include <x/fd.H>
#include <x/fdlistenerimplobj.H>
#include <x/weakptr.H>
#include <x/tuple_param_pack.H>
#include <x/namespace.h>

#include <tuple>
#include <type_traits>
namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Listen for connections on a server socket, start a thread for each one.

class fdlistenerObj : virtual public obj {

	//! The implementation boject

	ref<fdlistenerImplObj> impl;

	//! Convert saved parameters in the tuple to a parameter pack.
	template<typename tuple_args> class invoke;

	//! Forward fdserverObj to some object.

	template<typename serverType, typename ...Args>
	class serverObj : public fdlistenerImplObj::fdserverObj {

		//! Our server
		serverType server;

		//! Any additional arguments to the server

		std::tuple<Args...> args;

	public:
		//! Constructor
		template<typename ...cons_Args>
		serverObj(//! Socket handler
			  const serverType &serverArg,

			  //! Forwarded parameters
			  cons_Args && ...argsArg)
			: server(serverArg),
			  args(std::forward<cons_Args>(argsArg)...)
		{
		}

		~serverObj()
		{
		}

		//! Invoke the handler's run() method
		void run(const fd &socket, //!< The new socket
			 const fd &termpipe) //!< The termination filedesc
			override
		{
			invoke<typename
			       tuple_2_param_pack<sizeof...(Args)>::type>
				::run(server, socket, termpipe, args);
		}
	};

public:

	//! Specify the file descriptor(s) to listen on.

	//! This type defines the second parameter to constructor, and specifies
	//! the sockets in one of several ways.

	//! This parameter may be an \c int, an explicit port
	//! number, a string that specifies a port by its name or
	//! number, a string that specifies a list of ports
	//! separated by commas or spaces (with the ports
	//! specified by name or number), or a
	//! \c std::list<int> \c &, an explicit list of
	//! port numbers.

	typedef fdlistenerImplObj::listenon listenon;

	//! Constructor

	fdlistenerObj(//! Which file descriptors to listen on

		      const listenon &listenonArg,

		      //! Maximum number of server threads
		      size_t maxthreads=10,

		      //! Minimum number of server threads to precreate
		      size_t minthreads=0,

		      //! Name of started threads, for logging purposes

		      const std::string &jobnameArg="server",

		      //! Property name.

		      //! If this is specified, maxthreads,
		      //! minthreads, and
		      //! maxwait is obtained from application
		      //! properties
		      //! propname::maxthreads,
		      //! propname::minthreads, and
		      //! propname::maxwait, and the above
		      //! parameters specify
		      //! the default values, if not set in
		      //! the property file.
		      const std::string &propname="");

	//! Destructor
	~fdlistenerObj();

	//! Start the listener

	//! start() takes only refs.
	//!
	template<typename objType, typename baseClass,
		 typename ...Args>
	void start(const ref<objType, baseClass> &server,
		   Args && ...args)
	{
		impl->start(ref<serverObj< ref<objType, baseClass>,
					   typename std::add_const<
						   typename
						   std::decay<Args>::type>
					   ::type...> >
			    ::create(server, std::forward<Args>(args)...));
	}

	//! Start the listener

	//! \overload
	//! If we get a ptr, make sure it's a ref.

	template<typename objType, typename baseClass,
		 typename ...Args>
	void start(const ptr<objType, baseClass> &server,
		   Args && ...args)
	{
		start(ref<objType, baseClass>(server),
		      std::forward<Args>(args)...);
	}

	//! Stop the listener

	//! The listener is signaled to stop. Invoke wait() to wait until
	//! the listener stops.
	void stop();

	//! Wait until the listener thread terminates
	void wait();
};

//! Helper specialization for invoking the server object.
template<size_t ...n>
class fdlistenerObj::invoke<tuple_param_pack<n...> > {

public:

	//! Invoke the saved server object's run() method

	//! run() gets the accepted socket connection, the termination pipe,
	//! and any additional parameters originally passed to start().

	template<typename server_type, typename tuple_type>
	static inline void run(const server_type &server,
			       const fd &socket,
			       const fd &termpipe,
			       tuple_type &tuple)
	{
		server->run(socket, termpipe, std::get<n>(tuple)...);
	}
};

#if 0
{
#endif
}
#endif
