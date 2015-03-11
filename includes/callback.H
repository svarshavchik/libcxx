/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_callback_H
#define x_w_callback_H

#include <x/ref.H>
#include <x/callbackfwd.H>
#include <x/callbackobj.H>
#include <x/weaklist.H>
#include <x/namespace.h>

#include <vector>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A template that forwards invoke() to a functor

//! Used by \ref callback "callbacks".
//!
//! Subclasses either callbackObj, implements invoke().
//! Helper class used by \ref callback "callback"'s.

template<typename functor_type, typename base_class, typename ret_type,
	 typename ...Args>
class callbackImplObj : public base_class {

public:
	//! Functor to invoke
	functor_type functor;

	//! Constructor
	callbackImplObj(const functor_type &functorArg)
		: functor(functorArg)
	{
	}

	//! Move constructor
	callbackImplObj(functor_type &&functorArg)
		: functor(std::move(functorArg))
	{
	}

	//! Destructor
	~callbackImplObj() {}

	//! Forward arguments to functor

	ret_type invoke(Args ...args) override
	{
		return functor(std::forward<Args>(args)...);
	}
};

//! A template that forwards invoke() to a functor

//! A specialization for lambdas that return voids.

template<typename functor_type, typename base_class, typename ...Args>
class callbackImplObj<functor_type, base_class, void, Args...>
	: public base_class {

public:
	//! Functor to invoke
	functor_type functor;

	//! Constructor
	callbackImplObj(const functor_type &functorArg)
		: functor(functorArg)
	{
	}

	//! Move constructor
	callbackImplObj(functor_type &&functorArg)
		: functor(std::move(functorArg))
	{
	}

	//! Destructor
	~callbackImplObj() {}

	//! Forward arguments to functor

	void invoke(Args ...args) override
	{
		functor(std::forward<Args>(args)...);
	}
};

//! Implement invoke_callbacks() and invoke_callbacks_log_exceptions().

template<typename ret_type> class callback_invocations {

public:

	//! Implement invoke_callbacks() for a weak list of callbacks that return a non-void value.

	template<typename stop_calling_functor,
		 typename default_value_functor,
		 typename ...Args,
		 typename ...invokeArgs>
	static ret_type invoke(//! List of callbacks
			       const weaklist<callbackObj<ret_type, Args...>>
			       &l,

			       //! Lambda receives the return value
			       //! from each callback. If it returns
			       //! \c true, no more callbacks get
			       //! invoked, and the value gets
			       //! returned here.
			       stop_calling_functor &&stop_calling,

			       //! Lambda returns the value that
			       //! should be returned if stop_calling
			       //! always returned false.

			       default_value_functor &&default_value,

			       //! Any additional parameters forwarded
			       //! to each callback.
			       invokeArgs && ...args)
	{
		// Iterators hold a lock on the weak list. First, copy all
		// the callbacks to a vector, out of the weak list, then
		// invoke them, so the callbacks do not get invoked with a
		// held lock.

		std::vector<callback<ret_type, Args...>> callbacks;

		for (const auto &weakptr:*l)
		{
			auto cb=weakptr.getptr();

			if (!cb.null())
				callbacks.push_back(cb);
		}

		for (const auto &cb:callbacks)
		{
			auto value=cb->invoke(std::forward<args>...);

			if (stop_calling(value))
				return value;
		}

		return default_value();
	}

	//! Invoke callbacks in a callback list, log any exceptions they throw.

	template<typename stop_calling_functor,
		 typename default_value_functor,
		 typename ...Args,
		 typename ...invokeArgs>
	static ret_type invoke_log(//! List of callbacks
				   const weaklist<callbackObj<ret_type,
				   Args...>> &l,

				   //! Lambda receives the return value
				   //! from each callback. If it returns
				   //! \c true, no more callbacks get
				   //! invoked, and the value gets
				   //! returned here.
				   stop_calling_functor &&stop_calling,

				   //! Lambda returns the value that
				   //! should be returned if stop_calling
				   //! always returned false.

				   default_value_functor &&default_value,

				   //! Any additional parameters forwarded
				   //! to each callback.
				   invokeArgs && ...args)
	{
		// Iterators hold a lock on the weak list. First, copy all
		// the callbacks to a vector, out of the weak list, then
		// invoke them, so the callbacks do not get invoked with a
		// held lock.

		std::vector<callback<ret_type, Args...>> callbacks;

		for (const auto &weakptr:*l)
		{
			auto cb=weakptr.getptr();

			if (!cb.null())
				callbacks.push_back(cb);
		}

		for (const auto &cb:callbacks)
		{
			try {
				auto value=cb->invoke(std::forward<args>...);

				if (stop_calling(value))
					return value;
			} catch (const exception &e)
			{
				e->caught();
			}
		}

		return default_value();
	}
};

//! Specialized invoke_callbacks() and invoke_callbacks_log_exceptions().

//! For callbacks that return void.

template<> class callback_invocations<void> {

public:


	//! Specialized definition of invoke_callbacks() for callbacks that return a void.

	//! Invoke all callbacks in a weak list.

	template<typename ...Args, typename ...invokeArgs>
	static void invoke(const weaklist<callbackObj<void, Args...>> &l,
			   invokeArgs && ...args)
	{
		std::vector<callback<void, Args...>> callbacks;

		for (const auto &weakptr:*l)
		{
			auto cb=weakptr.getptr();
			if (!cb.null())
				callbacks.push_back(cb);
		}

		for (const auto &cb:callbacks)
			cb->invoke(std::forward<invokeArgs>(args)...);
	}

	//! Invoke all callbacks in a weak list, and log any exceptions.

	template<typename ...Args, typename ...invokeArgs>
	static void invoke_log(const weaklist<callbackObj<void, Args...>> &l,
			       invokeArgs && ...args)
	{
		std::vector<callback<void, Args...>> callbacks;

		for (const auto &weakptr:*l)
		{
			auto cb=weakptr.getptr();
			if (!cb.null())
				callbacks.push_back(cb);
		}

		for (const auto &cb:callbacks)
			try {
				cb->invoke(std::forward<invokeArgs>(args)...);
			} catch (const exception &e)
			{
				e->caught();
			}
	}
};

//! Invoke all callbacks in a weak list.

template<typename ret_type,
	 typename ...Args,
	 typename ...invokeArgs>
inline ret_type invoke_callbacks(const weaklist<callbackObj<ret_type, Args...>>
				 &l,
				 invokeArgs && ...args)
{
	return callback_invocations<ret_type>::
		template invoke(l, std::forward<invokeArgs>(args)...);
}

//! Invoke all callbacks in a weak list, catching and logging any exception thrown from a callback.

template<typename ret_type,
	 typename ...Args,
	 typename ...invokeArgs>
inline ret_type invoke_callbacks_log_exceptions
(const weaklist<callbackObj<ret_type, Args...>> &l,
 invokeArgs && ...args)
{
	return callback_invocations<ret_type>
		::template invoke_log(l, std::forward<invokeArgs> (args)...);
}

//! Base class for \ref callback "callback" handles.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::callback<ret_type, args...>::base
//!
//! Implements invoke_callback_list(weaklist, args...) that invokes a list
//! of callbacks currently in some weak list, forwarding any remaining arguments
//! to each callback, in turn.

template<typename ret_type, typename ...Args>
class callbackBase {

public:

	//! Factory that defines a custom create() method

	template<typename ref_type>
	class objfactory {

	public:

		//! create() constructs a custom subclass of callbackObj

		//! create() for a callbackObj takes a functor for an
		//! argument, and pulls a switcheroo, constructing the
		//! implObj subclass.

		template<typename functor_type>
                static inline auto create(functor_type && functor)
		{
			return ref<callbackImplObj<typename
						   std::remove_reference
						   <functor_type>::type,
						   callbackObj<ret_type,
							       Args...>,
						   ret_type, Args...>>
				::create(std::forward<functor_type>(functor));
		}

		//! create() stub, construct a no-op callback

		static inline auto create()
		{
			return ptrrefBase::objfactory<ref<callbackObj<ret_type,
								      Args...>,
							  callbackBase<ret_type,
								       Args...>>
						      >::create();
		}
	};
};

#ifndef DOXYGEN
extern template class callbackBase<void>::objfactory<callback<void>>;
extern template void invoke_callbacks(const weaklist<callbackObj<void>> &);
extern template void invoke_callbacks_log_exceptions(const weaklist<callbackObj<void>> &);
extern template
void callback_invocations<void>::invoke(const weaklist<callbackObj<void>> &);
extern template
void callback_invocations<void>::invoke_log(const weaklist<callbackObj<void>> &);
#endif

#if 0
{
#endif
}

#endif
