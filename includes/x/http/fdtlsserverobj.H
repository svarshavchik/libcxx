/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_fdtlsserverobj_H
#define x_tls_fdtlsserverobj_H

#include <x/obj.H>
#include <x/http/fdtlsserverimpl.H>
#include <x/gnutls/sessioncachefwd.H>
#include <x/logger.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
	namespace gnutls {
		namespace http {
#if 0
		}
	}
}
#endif

//! Run a file descriptor-based HTTPS server after receiving a connection.

//! \see fdtlsserver

class fdtlsserverObj : virtual public obj {

protected:
	//! The logger object
	LOG_CLASS_SCOPE;

	//! The session cache.
	sessioncache cache;

public:
	//! Default constructor
	fdtlsserverObj();

	//! Install a custom cache
	fdtlsserverObj(const sessioncache &cacheArg);

	//! Destructor
	~fdtlsserverObj() noexcept;

	//! fdlistener gives us the file descriptor here, run with it.

	template<typename sessionFactoryObjClass,
		 typename sessionFactoryBaseClass,
		 typename serverFactoryObjClass,
		 typename serverFactoryBaseClass,
		 typename ...Args>
	void run(//! The new socket

		 const fd &socket,

		 //! The termination file descriptor

		 const fd &termpipe,

		 //! The TLS session factory must be supplied as an extra argument to fdlistener's start()

		 const ref<sessionFactoryObjClass, sessionFactoryBaseClass>
		 &session_factory,

		 //! A factory for a new server object that's a subclass of \ref fdtlsserverimpl "INSERT_LIBX_NAMESPACE::gnutls::http::fdtlsserverimpl".

		 const ref<serverFactoryObjClass, serverFactoryBaseClass>
		 &factory,


		 //! Any remaining arguments get forwarded to the instantiated server's run().

		 Args && ...args)
	{
		try {
			auto instance=factory->create();

			auto sess=session_factory->create(GNUTLS_SERVER,
							  socket);

			sess->session_cache(cache);
			instance->run(socket, termpipe, sess,
				      std::forward<Args>(args)...);
		} catch (const exception &e)
		{
			run_failed(socket, e);
		}
	}

	//! Log an error

	virtual void run_failed(const fd &socket,
				const exception &e);
};

#if 0
{
	{
		{
#endif
		}
	}
}
#endif
