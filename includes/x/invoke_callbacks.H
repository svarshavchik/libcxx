/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_invoke_callbacks_H
#define x_w_invoke_callbacks_H

#include <x/functionalrefptr.H>
#include <x/weaklist.H>
#include <vector>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif


//! Implement invoke_callbacks() and invoke_callbacks_log_exceptions().

template<typename ret_type> class callback_invocations {

public:

	//! Implement invoke_callbacks() for a weak list of callbacks that return a non-void value.

	template<typename stop_calling_functor,
		 typename default_value_functor,
		 typename ...Args,
		 typename ...invokeArgs>
	static ret_type invoke(//! List of callbacks
			       const weaklist<functionObj<ret_type(Args...)>>
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

		std::vector<ref<functionObj<ret_type(Args...)>>> callbacks;

		for (const auto &weakptr:*l)
		{
			auto cb=weakptr.getptr();

			if (!cb.null())
				callbacks.push_back(cb);
		}

		for (const auto &cb:callbacks)
		{
			auto value=cb(std::forward<invokeArgs>(args)...);

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
				   const weaklist<functionObj<ret_type(Args...)>
				   > &l,

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

		std::vector<ref<functionObj<ret_type(Args...)>>> callbacks;

		for (const auto &weakptr:*l)
		{
			auto cb=weakptr.getptr();

			if (!cb.null())
				callbacks.push_back(cb);
		}

		for (const auto &cb:callbacks)
		{
			try {
				auto value=cb(std::forward<invokeArgs>
					      (args)...);

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
	static void invoke(const weaklist<functionObj<void(Args...)>> &l,
			   invokeArgs && ...args)
	{
		std::vector<ref<functionObj<void(Args...)>>> callbacks;

		for (const auto &weakptr:*l)
		{
			auto cb=weakptr.getptr();
			if (!cb.null())
				callbacks.push_back(cb);
		}
		for (const auto &cb:callbacks)
			(*cb)(std::forward<invokeArgs>(args)...);
	}

	//! Invoke all callbacks in a weak list, and log any exceptions.

	template<typename ...Args, typename ...invokeArgs>
	static void invoke_log(const weaklist<functionObj<void(Args...)>> &l,
			       invokeArgs && ...args)
	{
		std::vector<ref<functionObj<void(Args...)>>> callbacks;

		for (const auto &weakptr:*l)
		{
			auto cb=weakptr.getptr();
			if (!cb.null())
				callbacks.push_back(cb);
		}

		for (const auto &cb:callbacks)
			try {
				(*cb)(std::forward<invokeArgs>(args)...);
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
inline ret_type invoke_callbacks(const weaklist<functionObj<ret_type(Args...)>>
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
(const weaklist<functionObj<ret_type(Args...)>> &l,
 invokeArgs && ...args)
{
	return callback_invocations<ret_type>
		::template invoke_log(l, std::forward<invokeArgs> (args)...);
}

#ifndef DOXYGEN
extern template void invoke_callbacks(const weaklist<functionObj<void()>> &);
extern template void invoke_callbacks_log_exceptions(const weaklist<functionObj<void()>> &);
extern template
void callback_invocations<void>::invoke(const weaklist<functionObj<void ()>> &);
extern template
void callback_invocations<void>::invoke_log(const weaklist<functionObj<void ()>> &);
#endif

#if 0
{
#endif
}

#endif
