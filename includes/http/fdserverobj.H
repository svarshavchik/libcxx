/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_fdserverobj_H
#define x_http_fdserverobj_H

#include <x/obj.H>
#include <x/http/fdserverimpl.H>
#include <x/logger.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

//! Run a file descriptor-based HTTP server after receiving a connection.

//! \see fdserver
//!

class fdserverObj : virtual public obj {

protected:
	//! The logging object.

	LOG_CLASS_SCOPE;

public:
	//! Constructor

	fdserverObj();

	//! Destructor
	~fdserverObj() noexcept;

	//! fdlistener gives us the file descriptor here, run with it.

	//! The additional parameter defines a create() method that instantiates
	//! a subclass of \ref fdserverimpl "INSERT_LIBX_NAMESPACE::http::fdserverimpl"

	template<typename factoryObjClass, typename factorybaseClass,
		  typename ...Args>
	void run(//! The new socket

		 const fd &socket,

		 //! The termination file descriptor

		 const fd &termpipe,

		 //! A factory for a new server object.
		 const ref<factoryObjClass, factorybaseClass> &factory,

		 //! Any additional arguments get forwarded to the instantiated server
		 Args && ...args)
	{
		try {

			auto instance=factory->create();
			instance->run(socket, termpipe,
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
#endif
	}
}
#endif
