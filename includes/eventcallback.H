/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_eventcallback_H
#define x_eventcallback_H

#include <mutex>
#include <x/weaklist.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Default handler object used by the eventcallback template.

template<typename arg_type> class eventcallbackbaseObj : virtual public obj {

public:

	//! Constructor
	eventcallbackbaseObj() {}

	//! Destructor
	~eventcallbackbaseObj() noexcept {}

	//! The callback that gets invoked

	virtual void event(const arg_type &arg)=0;
};

//! Specialization for callbacks that do not take an argument.

template<> class eventcallbackbaseObj<void> : virtual public obj {

public:

	//! Constructor
	eventcallbackbaseObj();

	//! Destructor
	~eventcallbackbaseObj() noexcept;

	//! The callback that gets invoked

	virtual void event()=0;
};

//! A reference to the base callback class

//! \see eventcallback

template<typename arg_type>
using eventcallbackbase=ref<eventcallbackbaseObj<arg_type>>;

//! Default handler invocation implementation

template<typename arg_type,
	 typename callback_type=eventcallbackbaseObj<arg_type> >
class eventcallbackinvoke {

public:

	//! Default event() implementation, invoke the handler's event() method.
	static void event( const ptr<callback_type> &handler,
			   const arg_type &arg)
	{
		handler->event(arg);
	}
};

//! Specialization for event handlers that take no arguments

template<typename callback_type>
class eventcallbackinvoke<void, callback_type> {

public:

	//! Default event() implementation, invoke the handler's event() method.

	static void event( const ptr<callback_type> &handler)

	{
		handler->event();
	}
};

//! A lightweight event callback mechanism.

//! This is a lightweight alternative to an
//! \ref eventfactory "event factory" that's a plain object, implements
//! a single event notification, is not a
//! reference-counted object, and does not use a registration handle.
//! This template defines a non-copyable object that registers handlers in
//! a weak list. When the registered object goes out of scope and gets
//! destroyed, it gets deregistered automatically. Until then, invoking
//! event() invokes all registered callback objects.
//!
//! The template argument is the argument that's passed to callback objects,
//! which must subclass from the reference-counted
//! \ref eventcallback::baseObj "baseObj" member class defined
//! by the template, which is typedef-ed to the template's second argument.
//!
//! \par Template arguments
//!
//! - the type of the argument passed to the callback function.
//!
//! - the type of the callback handler class, defaults to eventcallbackbaseObj
//! and gets typedef-ed to baseObj.
//!
//! - a callback invocation class, defaults to eventcallbackinvoke<arg_type>.
//! The callback invocation class must define a static method named event()
//! that takes a reference to the callback handler and the callback argument,
//! then invokes the callback handler. The default
//! eventcallbackinvoke<arg_type> implementation invokes the handler's
//! event() method.
//!
//! A specialization with the \c void argument type implements callbacks
//! that take no arguments. The specialization for eventcallbackinvoke<void>
//! defines an event() that takes only a reference to a callback object, and
//! invokes the callback object's event() method, without any arguments.

template<typename arg_type,
	 typename handler_type=eventcallbackbaseObj<arg_type>,
	 typename invoke_type=eventcallbackinvoke<arg_type> >
class eventcallback {

	//! This type

	typedef eventcallback<arg_type, handler_type, invoke_type> thisclass_t;

public:

	//! Not implemented

	eventcallback(const thisclass_t &)=delete;

	//! Not implemented

	thisclass_t &operator=(const thisclass_t &)=delete;

	//! Not implemented

	eventcallback(thisclass_t &&)=delete;

	//! Not implemented

	thisclass_t &operator=(thisclass_t &&)=delete;

	//! Base class for reference class instances registered by install()

	typedef handler_type baseObj;

private:
	//! The container for registered callbacks

	typedef weaklist<baseObj> callback_list_t;

	//! Registered callback
	callback_list_t callback_list;

	//! An internal mutex
	mutable std::mutex mutex;
public:

	//! Constructor
	eventcallback()
	: callback_list(callback_list_t::create())
	{
	}

	//! Destructor
	~eventcallback() noexcept
	{
	}

	//! Register a callback
	void install(const ref<baseObj> &callback)
	{
		callback_list->push_back(callback);
	}

	//! Helper class for using a lambda as a callback.

	template<typename lambda_type>
	class lambdaBaseHelperObj : public baseObj {

	public:
		//! The lambda
		lambda_type lambda;

		//! Constructor
		template<typename argType>
		lambdaBaseHelperObj(argType &&arg)
			: lambda(std::forward<argType>(arg))
		{
		}

		//! Destructor
		~lambdaBaseHelperObj() noexcept
		{
		}

		//! Forward event() to the lambda
		void event(const arg_type &arg) override
		{
			lambda(arg);
		}
	};

	//! Install a callback that invokes a lambda.

	//! This is available only when this template uses the
	//! default eventcallbackbaseObj value for the second template
	//! parameter.

	template<typename lambda_type>
	ref<baseObj> install_callback(lambda_type &&lambda)
	{
		auto cb=ref<lambdaBaseHelperObj<
				    typename std::decay<lambda_type>::type>>
			::create(std::forward<lambda_type>(lambda));

		install(cb);

		return cb;
	}

	//! Invoke all the registered callbacks
	void event(const arg_type &arg) const
	{
		// Iterating over a weak list acquires a read lock, so
		// grab the list of event handlers first, then invoke them,
		// and use our own mutex.

		std::lock_guard<std::mutex> lock(mutex);

		std::list<ref<baseObj> > cb_list;

		for (auto cbp: *callback_list)
		{
			ptr<baseObj> cb(cbp.getptr());

			if (!cb.null())
				cb_list.push_back(cb);
		}

		for (typename std::list<ref<baseObj> >::iterator
			     b(cb_list.begin()),
			     e(cb_list.end()); b != e; ++b)
			invoke_type::event(*b, arg);
	}
};

//! The void specialization implements callbacks without arguments.

template<typename handler_type,
	 typename invoke_type>
class eventcallback<void, handler_type, invoke_type> {

	//! This type

	typedef eventcallback<void, handler_type, invoke_type> thisclass_t;

public:

	//! Not implemented

	eventcallback(const thisclass_t &)=delete;

	//! Not implemented

	thisclass_t &operator=(const thisclass_t &)=delete;

	//! Not implemented

	eventcallback(thisclass_t &&)=delete;

	//! Not implemented

	thisclass_t &operator=(thisclass_t &&)=delete;


	//! Base class for reference class instances registered by install()

	typedef handler_type baseObj;

private:
	//! The container for registered callbacks

	typedef weaklist<baseObj> callback_list_t;

	//! Registered callback
	callback_list_t callback_list;

	//! An internal mutex
	mutable std::mutex mutex;

public:

	//! Constructor
	eventcallback()
	: callback_list(callback_list_t::create())
	{
	}

	//! Destructor
	~eventcallback() noexcept
	{
	}

	//! Register a callback
	void install(const ref<baseObj> &callback)
	{
		callback_list->push_back(callback);
	}
	//! Helper class for using a lambda as a callback.

	template<typename lambda_type>
	class lambdaBaseHelperObj : public baseObj {

	public:
		//! The lambda
		lambda_type lambda;

		//! Constructor
		template<typename argType>
		lambdaBaseHelperObj(argType &&arg)
			: lambda(std::forward<argType>(arg))
		{
		}

		//! Destructor
		~lambdaBaseHelperObj() noexcept
		{
		}

		//! Forward event() to the lambda
		void event() override
		{
			lambda();
		}
	};

	//! Install a callback that invokes a lambda

	template<typename lambda_type>
	ref<baseObj> install_callback(lambda_type &&lambda)
	{
		auto cb=ref<lambdaBaseHelperObj<
				    typename std::decay<lambda_type>::type>>
			::create(std::forward<lambda_type>(lambda));

		install(cb);

		return cb;
	}

	//! Invoke all the registered callbacks
	void event() const
	{
		// Iterating over a weak list acquires a read lock, so
		// grab the list of event handlers first, then invoke them,
		// and use our own mutex.

		std::lock_guard<std::mutex> lock(mutex);

		std::list<ptr<baseObj> > cb_list;

		for (auto cbl: *callback_list)
		{
			ptr<baseObj> cb(cbl.getptr());

			if (!cb.null())
				cb_list.push_back(cb);
		}

		for (auto e:cb_list)
			invoke_type::event(e);
	}
};

extern template class eventcallback<void>;

#if 0
{
#endif
}

#endif
