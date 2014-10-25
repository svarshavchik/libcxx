/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

/*! \file

This file defines the managed() function that implements a managed singleton.

*/

#ifndef x_managedsingletonapp_H
#define x_managedsingletonapp_H

#include <x/singletonapp.H>
#include <x/fd.H>
#include <x/fditer.H>
#include <x/weakptr.H>
#include <x/serialize.H>
#include <x/deserialize.H>
#include <x/destroycallbackflag.H>
#include <x/httportmap.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! managed() gives a ref to this object to the singleton's %instance() method.

//! This is the fourth argument received by the singleton object's %instance()
//! method. processed() should be invoked to indicate that the new instance
//! parameter has been processed. If it does not get invoked, and %instance()
//! returns, when the mcguffin, the fifth parameter, goes out of scope and
//! gets destroyed, the new instance process tries again, and attempts to
//! bring up the singleton.
//!
//! If processed() does not get invoked, it is expected that the primary
//! singleton thread is giving up the ghost. The primary thread should not
//! exist for much longer, in order so that the entire singleton process
//! can disappear, otherwise we lather, rinse, and repeat.

class singletonapp::processedObj : virtual public obj {
public:
	//! Processed flag
	bool flag;

	processedObj();
	~processedObj() noexcept;

	//! The secondary managed singleton instance has processed the request.
	void processed();
};

//! This is the factory object instantiated by managed()

//! This is the factory object that managed() instantiates and gives to
//! create().

template<typename appObjClass, typename appBaseClass, typename factory_type,
	 typename argsClass, typename argsBase,
	 typename ret_args>
class singletonapp::managedSingletonFactoryObj : virtual public obj {

	//! application object factory given to managed()
	factory_type factory;

	//! The portmapper object
	httportmap portmapper;

	//! The sameuser flag given to managed()
	bool sameuser;

	//! The primary application instance object

	//! The strong references to the application instance object are owned
	//! by each singleton thread. The initial invocation of newThread()
	//! will find nothing here. If all threads go out of scope and get
	//! destroyed, newThread() may still get invoked (if a connection
	//! from a new singleton process has just been established, concurrently
	//! with the last reference to the application object going out of
	//! scope, and the application object getting destroyed), so it gets
	//! instantiated again.
	weakptr<ptr<appObjClass, appBaseClass> > instance;

	//! Convenience typedef
	typedef ref<argsClass, argsBase> app_args;

	//! Convenience typedef
	typedef ptr<argsClass, argsBase> app_argsptr;

	//! Base class for singleton application threads.

	//! This is the base class. It validates the peer, deserializes the
	//! application parameters, invokes run() in the subclass, then
	//! serializates the application return value to the new singleton
	//! instance.

	class threadObj : public stoppableObj {
	public:
		//! The application object

		ref<appObjClass, appBaseClass> app;

		//! The portmapper
		httportmap portmapper;

		//! The sameuser flag passed to managed()

		bool sameuser;

		//! Constructor
		threadObj(ref<appObjClass, appBaseClass> &&appArg,
			  const httportmap &portmapperArg,
			  bool sameuserArg) : app(std::move(appArg)),
					      portmapper(portmapperArg),
					      sameuser(sameuserArg) {}

		//! Destructor
		~threadObj() noexcept
		{
		}

		//! Singleton therad implementation.
		void run(const fd &socket)
		{
			uid_t uid=validate_peer(portmapper, socket, sameuser);

			// Put the deserialized application arguments into a
			// ptr. It will always have a reference, but this
			// allows the application to receive parameters by
			// reference, and then discard them.

			app_argsptr args=({
					app_args args=app_args::create();

					fdinputiter in_iter(socket),
						in_iter_end;

					deserialize::iterator<fdinputiter>
						iter(in_iter, in_iter_end);

					iter(args);

					args;
				});

			// Run the application, then serialize the return value
			std::pair<ret_args, bool> ret=run(args, uid);

			{
				fdoutputiter out_iter(socket);

				serialize::iterator<fdoutputiter>
					iter(out_iter);

				iter(ret);
				out_iter.flush();
			}
		}

		//! Invoke the application.

		//! The subclass returns a pair, containing the return value,
		//! and a flag indicates that the parameters were processed
		//! (if not, the return value is meaningless).
		virtual std::pair<ret_args, bool> run(app_argsptr &args,
						      uid_t uid)=0;

		//! Forward stop() to the application object
		void stop()
		{
			app->stop();
		}
	};

	//! First singleton application thread

	//! The instantiated application object's run() method gets invoked.
	//!
	class firstThreadObj : public threadObj {

	public:
		//! Constructor
		firstThreadObj(ref<appObjClass, appBaseClass> &&appArg,
			       const httportmap &portmapper,
			       bool sameuser)
			: threadObj(std::move(appArg), portmapper, sameuser) {}

		//! Destructor
		~firstThreadObj() noexcept {}

		//! Invoke the singleton application thread's run()

		//! The returned processed flag is always true. It's presumed
		//! that the initial application thread always processes the
		//! application parameters.
		std::pair<ret_args, bool> run(app_argsptr &args,
					      uid_t uid)
		{
			return std::make_pair(this->app->run(uid, args), true);
		}
	};

	//! Additional singletona application threads

	//! Invoke the application object's instance() method.

	class additionalThreadObj : public threadObj {

	public:

		//! Constructor
		additionalThreadObj(ref<appObjClass, appBaseClass> &&appArg,
				    const httportmap &portmapper,
				    bool sameuser)
			: threadObj(std::move(appArg), portmapper, sameuser)
		{
		}

		//! Constructor
		~additionalThreadObj() noexcept
		{
		}

		//! Invoke the application object's instance() method.

		//! Afterwards, block until the mcguffin goes out of scope
		//! and gets detroyed, before returning the parameters.

		std::pair<ret_args, bool> run(app_argsptr &args,
					      uid_t uid)
		{
			processed flag=processed::create();
			ret_args ret=ret_args::create();

			destroyCallbackFlag cb=
				destroyCallbackFlag::create();

			{
				ref<obj> mcguffin=ref<obj>::create();

				mcguffin->ondestroy([cb]{cb->destroyed();});

				this->app->instance(uid, args, ret, flag,
						    mcguffin);
			}

			cb->wait();

			return std::make_pair(ret, flag->flag);
		}
	};

public:

	//! Construct the managed singleton factory

	managedSingletonFactoryObj(//! Application object factory
				   factory_type &&factoryArg,

				   //! The sameuser parameter to managed().
				   bool sameuserArg)
		: factory(std::move(factoryArg)),
		  portmapper(httportmap::base::regpid2exe()),
		  sameuser(sameuserArg)
	{
	}

	//! Destructor
	~managedSingletonFactoryObj() noexcept
	{
	}

	//! newThread() implementation for create().

	//! Constructs either the initial, or additional threads.
	ref<threadObj> newThread()
	{
		auto ptr=instance.getptr();

		if (ptr.null())
		{
			auto firstThread=
				ref<firstThreadObj>::create(factory(),
							    portmapper,
							    sameuser);

			instance=firstThread->app;

			return firstThread;
		}

		return ref<additionalThreadObj>::create(ptr,
							portmapper,
							sameuser);
	}
};

//! Helper class used by managed() to define various classes.

//! managed() determines the application parameter and return objects using
//! this helper class.

template<typename factory_type, typename args_type>
class singletonapp::managed_ret_type {

public:
	//! Fake parameter to the expected run() method in the application object.

	static ptr<typename args_type::obj_type,
		   typename args_type::base> &get_args_helper();

	//! Fake application object factory
	static factory_type &&get_factory_helper();

	//! Obtain the type returned by the application object's run() method.
	typedef decltype( get_factory_helper()()->run(0, get_args_helper()))
		ret_t;

	//! Determine the object returned by the application object factory.
	typedef decltype( get_factory_helper()() ) impl_t;

};

#ifndef DOXYGEN_FIXME

//! A managed application singleton

//! This is a wrapper for create() that provides a higher-level API, that
//! introduces the concept of singleton application parameters and return
//! values. Both are reference-counted objects, with a serialization function.
//!
//! \return a std::pair, containing the return object, and the userid of the
//! singleton process that processed the parameters.

template<typename factory_type, typename args_type>
inline
std::pair<typename singletonapp::managed_ret_type<factory_type, args_type>
	  ::ret_t, uid_t>
singletonapp::managed(//! A functor that constructs an aplication object

		      //! The functor gets invoked if this invocation becomes
		      //! the singleton instance.
		      factory_type &&factory,

		      //! The argument to the singleton application.

		      //! This must be a \ref ref "INSERT_LIBX_NAMESPACE::ref".
		      //!
		      const args_type &args,

		      //! The userid parameter to create().
		      uid_t singletonappuid,

		      //! The socket mode parameter to create().
		      mode_t singletonsockmode,

		      //! Whether singleton processes are per userid.
		      bool mustbesameuser)
{

	// Determine the return type, and the application object.
	typedef typename managed_ret_type<factory_type, args_type>::ret_t
		ret_type;
	typedef typename managed_ret_type<factory_type, args_type>::impl_t
		impl_t;

	// Invoke the actual managed() method.
	return managed_helper<factory_type, impl_t, args_type, ret_type>
		::managed(std::move(factory), args,
			  singletonappuid,
			  singletonsockmode, mustbesameuser);
}
#endif

//! managed() implementation.

//! This specialization of the managed_helper class enforces the requirement
//! that:
//!
//! - the singleton invocation parameter object,
//!
//! - the singleton return value object, and
//!
//! - the singleton application object
//!
//! are all \ref ref "INSERT_LIBX_NAMESPACE::ref"s.
//!
template<typename factory_type,
	 typename implObj, typename implBase,
	 typename argsObj, typename argsBase,
	 typename retObj, typename retBase>
class singletonapp::managed_helper<factory_type,
				   ref<implObj, implBase>,
				   ref<argsObj, argsBase>,
				   ref<retObj, retBase> > {

public:

	//! managed() implementation

	//! All the types are coerced now, time to get down to business.
	//!
	//! Invoke create() to create the application singleton. Serialize
	//! the parameters into the singleton socket connection, then wait for
	//! and deserialize the return value. If the flag indicates that the
	//! invocation was not processed, lather, rinse, repeat.
	static std::pair<ref<retObj, retBase>, uid_t>
	managed(factory_type &&factoryArg,
		const ref<argsObj, argsBase> &args,
		uid_t uid,
		mode_t mode,
		bool mustbesameuser)
	{
		// Make the factory for create()

		auto factory=ref<managedSingletonFactoryObj
				 <implObj, implBase,
				  factory_type,
				  argsObj, argsBase,
				  ref<retObj, retBase> > >
			::create(std::move(factoryArg), mustbesameuser);

		while (1)
		{
			instance instance=create(factory, uid, mode);

			fd connection=instance->connection;

			uid_t suid=validate_peer(connection, mustbesameuser);

			{
				fdoutputiter out_iter(connection);

				serialize::iterator<fdoutputiter>
					iter(out_iter);

				iter(args);
				out_iter.flush();
			}

			auto ret=std::make_pair(ref<retObj, retBase>::create(),
						false);

			fdinputiter in_iter(connection), in_iter_end;

			deserialize::iterator<fdinputiter>
				iter(in_iter, in_iter_end);

			iter(ret);

			if (!ret.second)
				continue;

			return std::make_pair(ret.first, suid);
		}
	}
};

#if 0
{
#endif
}
#endif
