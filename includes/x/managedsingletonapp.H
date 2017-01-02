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

//! The managed application singleton is not derived from threadmsgdispatcherObj

//! \internal
//!
//! A helper class for managed application singletons when the singletons
//! are not derived from threadmsgdispatcherObj.
//!
//! The real application singleton is stored in this class, and this class
//! is a proxy that gets returned by newThread() by the class that gets
//! passed to \ref singletonapp "singletonapp::create()".
//!
//! It defines the `run()` method that gets forwarded to the run_common()
//! virtual method that gets implemented in the subclass, and provides
//! a run_app() method that's
//!
//! For managed application singletons that are not derived from
//! threadmsgdispatcherObj, this class is derived only from stoppableObj,
//! and forwards its stop() to the real application singleton.

template<typename objClass, typename baseClass,
	 typename args_type>
class singletonapp::managedSingletonAppContainerObj<objClass, baseClass,
						    args_type, false>
	: public stoppableObj {

public:

	//! The application object

	const ref<objClass, baseClass> app;

	//! No additional invocation args are needed.
	class invoke_args {};

	//! Invoke the managed singleton app's run() method.
	inline auto run_app(invoke_args &ignore_args,
			    uid_t uid,
			    args_type &args)
	{
		return this->app->run(uid, args);
	}

	//! Constructor
	managedSingletonAppContainerObj(const ref<objClass, baseClass> &app)
		: app(app)
	{
	}

	//! Destructor

	~managedSingletonAppContainerObj() noexcept=default;

	//! Run common code.

	void run(const fd &socket)
	{
		invoke_args dummy;

		run_common(socket, dummy);
	}

	//! Singleton thread implementation.

	virtual void run_common(const fd &socket,
				invoke_args &args)=0;
};

//! The managed application singleton is derived from threadmsgdispatcherObj

//! \internal
//!
//! A helper class for managed application singletons when the singletons
//! are derived from threadmsgdispatcherObj.
//!
//! The real application singleton is stored in this class, and this class
//! is a proxy that gets returned by newThread() by the class that gets
//! passed to \ref singletonapp "singletonapp::create()".
//!
//! It defines the `run()` method that gets forwarded to the run_common()
//! virtual method that gets implemented in the subclass, and provides
//! a run_app() method that's
//!
//! Since the application singleton is a threadmsgdispatcherObj, this
//! proxy is also derived from threadmsgdispatcherObj, however it does not
//! get used. The threadmsgdispatcherObj's mcguffin gets forwarded to the
//! real application singleton's run() method.

template<typename objClass, typename baseClass,
	 typename args_type>
class singletonapp::managedSingletonAppContainerObj<
	objClass, baseClass, args_type, true>
	: public threadmsgdispatcherObj {

public:

	//! The application object

	const ref<objClass, baseClass> app;

	//! Pass start_threadmsgdispatcher()'s mcguffin as the invocation args.

	typedef x::ptr<x::obj> invoke_args;

	//! Invoke the managed singleton app's run() method.

	auto run_app(ptr<obj> &threadmsgdispatcher_mcguffin,
		     uid_t uid,
		     args_type &args)
	{
		return this->app->run(threadmsgdispatcher_mcguffin, uid, args);
	}

	//! Constructor
	managedSingletonAppContainerObj(const ref<objClass, baseClass> &app)
		: app(app)
	{
	}

	//! Destructor
	~managedSingletonAppContainerObj() noexcept=default;

	//! Run common code.

	//! We pass through the threadmsgdispatcher_mcguffin, all the way
	//! to run_app().

	void run(x::ptr<x::obj> &threadmsgdispatcher_mcguffin, const fd &socket)
	{
		run_common(socket, threadmsgdispatcher_mcguffin);
	}

	//! Singleton thread implementation.

	virtual void run_common(const fd &socket,
				invoke_args &args)=0;
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

	//! Convenience typedef
	typedef managedSingletonAppContainerObj<
		appObjClass, appBaseClass, app_argsptr> app_container_t;


	//! Base class for singleton application threads.

	//! This is the base class. The base class implements run_common()
	//! that's inherited from the managedSingletonAppContainerObj.
	//!
	//! It validates the peer, deserializes the application parameters,
	//! calls invoke() in its subclass, then
	//! serializes the application's return value to the new singleton
	//! instance.

	class threadObj : public app_container_t {
	public:

		//! Convenience typedef
		typedef typename app_container_t::invoke_args invoke_args;

		//! The portmapper
		httportmap portmapper;

		//! The sameuser flag passed to managed()

		bool sameuser;

		//! Constructor
		threadObj(ref<appObjClass, appBaseClass> &&appArg,
			  const httportmap &portmapperArg,
			  bool sameuserArg)
			: app_container_t(std::move(appArg)),
			  portmapper(portmapperArg),
			  sameuser(sameuserArg) {}

		//! Destructor
		~threadObj() noexcept
		{
		}

		//! Singleton thread implementation.
		void run_common(const fd &socket,
				invoke_args &run_app_args)
			override
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
			std::pair<ret_args, bool>
				ret=invoke(args, uid, run_app_args);

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
		virtual std::pair<ret_args, bool> invoke(app_argsptr &args,
							 uid_t uid,
							 invoke_args &)=0;

		//! Forward stop() to the application object
		void stop() override
		{
			this->app->stop();
		}
	};

	//! The first singleton application thread

	//! Invokes run_app() in managedSingletonAppContainerObj, to execute
	//! the real application singleton class's run() method.

	class firstThreadObj : public threadObj {

	public:

		//! Convenience typedef

		typedef typename threadObj::invoke_args invoke_args;

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
		std::pair<ret_args, bool> invoke(app_argsptr &args,
						 uid_t uid,
						 invoke_args &inv_args)
			override
		{
			return std::make_pair(this->run_app(inv_args,
							    uid,
							    args),
					      true);
		}
	};

	//! Another singleton application thread.

	//! The application singleton that's used to construct this is the
	//! existing application singleton. Implements invoke() that
	//! executes the existing application singleton's instance() method.

	class additionalThreadObj : public threadObj {

	public:
		//! Convenience typedef

		typedef typename threadObj::invoke_args invoke_args;

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

		std::pair<ret_args, bool> invoke(app_argsptr &args,
						 uid_t uid,
						 invoke_args &inv_args)
			override
		{
			processed flag=processed::create();
			ret_args ret=ret_args::create();

			destroy_callback cb=
				destroy_callback::create();

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

	//! Constructs either the initial, or an additional thread.

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
//!
//! Select the appropriate specialization depending upon whether the
//! object returned by the factory is derived from threadmsgdispatcherObj.

template<typename factory_type, typename args_type>
class singletonapp::managed_ret_type
	: public managed_ret_type_impl<
	factory_type, args_type,
	std::is_base_of<threadmsgdispatcherObj,
			typename decltype(std::declval<factory_type &&>()())
			::obj_type>::value> {};

//! Helper class specialization for non-threadmsgdispatcherObj singleton apps.

template<typename factory_type, typename args_type>
class singletonapp::managed_ret_type_impl<factory_type, args_type, false> {

public:
	//! Convenience typedef
	typedef ptr<typename args_type::obj_type,
		    typename args_type::base> ptr_t;

	//! Determine the object returned by the application object factory.
	typedef decltype( std::declval<factory_type &&>()() ) impl_t;

	//! Obtain the type returned by the application object's run() method.
	typedef decltype( std::declval<factory_type &&>()()
			  ->run(0, std::declval<ptr_t &>())) ret_t;
};

//! Helper class specialization for threadmsgdispatcherObj singleton apps.

template<typename factory_type, typename args_type>
class singletonapp::managed_ret_type_impl<factory_type, args_type, true> {

public:
	//! Convenience typedef
	typedef ptr<typename args_type::obj_type,
		    typename args_type::base> ptr_t;

	//! Determine the object returned by the application object factory.
	typedef decltype( std::declval<factory_type &&>()() ) impl_t;

	//! Obtain the type returned by the application object's run() method.
	typedef decltype( std::declval<factory_type &&>()()
			  ->run(std::declval<ptr<obj> &>(),
				0, std::declval<ptr_t &>())) ret_t;
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
std::pair<typename singletonapp::managed_ret_type<
		  factory_type, args_type>
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
