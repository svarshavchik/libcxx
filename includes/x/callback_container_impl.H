/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_callback_container_impl_H
#define x_callback_container_impl_H

#include <x/ref.H>
#include <x/obj.H>
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/functionalrefptr.H>
#include <x/logger.H>
#include <x/callback_containerfwd.H>
#include <type_traits>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Helper to retrieve the return value of callbacks.

template<typename signature> class callback_return_type;

//! Helper to retrieve the return value of callbacks.

template<typename return_type,
	 typename ...Args> class callback_return_type<return_type(Args...)> {

public:
	typedef return_type type;
};

//! Helper to retrieve the return value of callbacks.

template<typename signature>
using callback_return_type_t=typename callback_return_type<signature>::type;

template<typename signature, typename container_type>
class callback_container_implObj;

template<typename signature, typename container_type>
class callback_container_implBase : public ptrref_base {

public:

	//! A mcguffin returned by \ref callback_container_implObj "create_mcguffin()".

	typedef ref<typename callback_container_implObj<signature,
							container_type>
		    ::mcguffinObj> mcguffin;

	//! A mcguffin returned by \ref callback_container_implObj "create_mcguffin()".

	typedef ptr<typename callback_container_implObj<signature,
							container_type>
		    ::mcguffinObj> mcguffinptr;
};


//! The actual callback container object.

//! This object implements all methods except for invoke(), which
//! is implemented in \ref callback_containerObj "callback_containerObj".

template<typename signature, typename container_type>
class callback_container_implObj : virtual public obj {

protected:
	//! The logging object.
	LOG_CLASS_SCOPE;
public:

	//! The container for the callbacks.
	typedef container_type callbacks_t;

	//! A convenience alias for a reference to this object.
	typedef ref<callback_container_implObj<signature, container_type>,
		    callback_container_implBase<signature, container_type>
		    > callback_container_t;

protected:

	//! Mutex-protected object

	//! \internal

	class container_info {

	public:
		//! The active callbacks.
		callbacks_t callbacks;

		//! The current callback being invoked.

		typename callbacks_t::iterator current_iterator;

		//! The callback was removing during its execution.
		bool current_callback_is_removed;

		//! The constructor.

		//! The current iterator is set to end(), and the removed
		//! flag to \c false.
		container_info() : current_iterator{callbacks.end()},
			      current_callback_is_removed(false)
		{
		}
	};

	//! Mutex-protected metadata.

	mpobj<container_info> container;

public:

	//! A mcguffin for one callback.

	class mcguffinObj : virtual public obj {

		//! My container.
		callback_container_t mycontainer;

		//! My iterator in the container.
		typename callbacks_t::iterator myiter;

	public:

		//! The constructor

		//! The constructor receives a reference to its own
		//! container, and the `end()` iterator value for the actual
		//! callback container.
		mcguffinObj(const callback_container_t &mycontainer,
			    typename callbacks_t::iterator myiter,

			    // Ignored, private object, so only
			    // create_mcguffin() can construct me.
			    mpobj<container_info> &ignored)
			: mycontainer(mycontainer), myiter(myiter)
		{
		}

		//! The destructor removes the callback from the container.

		~mcguffinObj()
		{
			typename mpobj<container_info>::lock lock{mycontainer->container};

			if (myiter == lock->callbacks.end())
				return; // Never installed.

			// This callback is getting invoked right now.
			// Just mark it for later removal.
			if (myiter == lock->current_iterator)
			{
				lock->current_callback_is_removed=true;
				return;
			}

			// Ok, safe to remove this callback.
			lock->callbacks.erase(myiter);
		}

		//! Activate the callback.

		//! The first parameter is always a \c functionref.
		//! The parameters get forwarded to the traits'
		//! \c install() method, saving the returned iterator value.

		template<typename ...Args>
		void install(const functionref<signature> &callback,
			     Args && ...args)
		{
			typename mpobj<container_info>::lock lock{mycontainer->container};

			if (myiter != lock->callbacks.end())
				throw EXCEPTION("Internal error.");

			myiter=lock->callbacks.install(callback,
						       std::forward<Args>(args)
						       ...);
		}

		//! Whether, or not, this callback was installed successfully.

		constexpr bool installed() const
		{
			typename mpobj<container_info>::lock lock{mycontainer->container};

			return (myiter != lock->callbacks.end());
		}
	};

	//! Create a mcguffin for a new callback.

	auto create_mcguffin()
	{
		auto me=callback_container_t(this);

		typename mpobj<container_info>::lock lock{container};

		return ref<mcguffinObj>::create(me, lock->callbacks.end(),
						container);
	}

	//! Create a mcguffin and install a new callback.

	//! The first parameter is a callable object, that gets turned into
	//! a function reference using functionref<signature>::create().
	//! That, and any
	//! remaining parameters get forwarded to install().

	template<typename functor_type,
		 typename ...Args>
	auto create_mcguffin_for(functor_type &&functor,
				 Args && ...args)
	{
		auto mcguffin=create_mcguffin();

		mcguffin->install(functionref<signature>
				  {std::forward<functor_type>(functor)},
				  std::forward<Args>(args)...);
		return mcguffin;
	}
protected:

	//! Advance the current callback iterator

	//! \internal
	inline bool advance(typename mpobj<container_info>::lock &lock)
	{
		// Initially, the current iterator is end(). If so,
		// reset it to begin().

		if (lock->current_iterator == lock->callbacks.end())
		{
			lock->current_iterator=lock->callbacks.begin();
		}
		else
		{
			// Ok, in the middle of iterating.

			auto p=lock->current_iterator;

			++lock->current_iterator;

			// If the current callback was removed, it's safe
			// to get rid of it now.

			if (lock->current_callback_is_removed)
			{
				lock->callbacks.erase(p);
				lock->current_callback_is_removed=false;
			}
		}

		// Whether the end of the container was reached (or the container
		// was empty.
		return lock->current_iterator != lock->callbacks.end();
	}
};

//! Callback container object.

//! Template class that implements a thread-safe container for callbacks that
//! get invoked from a single execution thread.
//!
//! All of the following methods except for invoke() are actually implemented in
//! the callback_container_implObj superclass. This class implements
//! only invoke().
//!
//! Don't use this template directly. Use
//! \ref callback_list "INSERT_LIBX_NAMESPACE::callback_list",
//! \ref callback_map "INSERT_LIBX_NAMESPACE::callback_map", or
//! \ref callback_multimap "INSERT_LIBX_NAMESPACE::callback_multimap".
//!
//! The first template parameter is the callbacks' signature. The second
//! template parameter is a container of \c functionref<signature> objects.
//! It is typically inherited from a stock \c std container,
//! should be container-like, but implement three additional methods:
//! install(), dereference(), and a static invoke() method. Three default
//! templates are available:
//! \ref callback_list "INSERT_LIBX_NAMESPACE::callback_list",
//! \ref callback_map "INSERT_LIBX_NAMESPACE::callback_map", and
//! \ref callback_list "INSERT_LIBX_NAMESPACE::callback_multimap".
//!
//! \code
//! typedef INSERT_LIBX_NAMESPACE::callback_list<void (int)> my_callback_list;
//!
//! my_callback_list callbacks=my_callback_list::create();
//!
//! auto mcguffin=callbacks->create_mcguffin()
//!
//! mcguffin->install(INSERT_LIBX_NAMESPACE::functionref<void (int)>::Create
//!                   ([]
//!                    (int n)
//!                    {
//!                         // ...
//!                    }));
//!
//! auto mcguffin=callbacks->create_mcguffin_for(LIBCXX_NAMESPACE::functionref<void (int)>::Create
//!                                             ([]
//!                                             (int n)
//!                                             {
//!                                                  // ...
//!                                             }));
//! \endcode
//!
//! \ref callback_list "callback_list" implements a
//! callback_container_implObj for a simple list of callbacks that gets invoked one at
//! a time. This example create()s a container of callbacks that take a single
//! \c int parameter, and return \c void.
//! create_mcguffin() returns a
//! \ref callback_container_implBase "my_callback_container::base::mcguffin", a placeholder
//! for a new callback.
//!
//! Its install() method must be invoked exactly once. The parameter is a
//! a \ref functionref "function object reference". This installs the
//! callback. The callback remains installed until the mcguffin goes out of
//! scope and gets destroyed.
//!
//! create_mcguffin_for() is a shortcut for taking a function object
//! parameter, invoking create_mcguffin(), with create()
//! constructing the function object.
//!
//! \code
//! typedef INSERT_LIBX_NAMESPACE::callback_map<int, void (int)> my_callback_map;
//!
//! my_callback_map callbacks=my_callback_map::create();
//!
//! auto mcguffin=callbacks->create_mcguffin()
//!
//! mcguffin->install(INSERT_LIBX_NAMESPACE::functionref<void (int)>::create
//!                   ([]
//!                    (int n)
//!                    {
//!                         // ...
//!                    }),
//!                    4)
//!
//! if (!mcguffin->installed())
//!     // ...
//! \endcode
//!
//! \ref callback_map "LIBCXX_NAMESPACE::callback_map" implements a
//! callback_container_implObj for a \c std::map-based container of
//! callbacks. This example creates a container of maps of \c int.
//! \ref callback_map "callback_map"'s install() takes a 2nd parameter
//! that specifies the new callback's priority key, in the container.
//!
//! The new callbacks can't be installed in the map if its key already exists,
//! of course. installed() returns an indication if the callback was installed.
//!
//! There's also an
//! \ref callback_multimap "LIBCXX_NAMESPACE::callback_multimap", which
//! implements a
//! callback_container_implObj for a \c std::multimap-based container.
//!
//! \code
//! callbacks->invoke(2);
//! \endcode
//!
//! This invokes all function object references, callbacks,
//! that are installed, and whose
//! mcguffins have not been destroyed yet. Arguments to \c invoke() are
//! forwarded to each callback, all callbacks get executed one at a time.
//!
//! \note
//! Only one execution thread should call invoke(), at a time. If multiple
//! execution threads call invoke() concurrently:
//!
//! - Some callbacks may get invoked once, others may get invoked several
//! times.
//!
//! - Each callback can get invoked by any execution thread.
//!
//! - When callbacks return values, a callback whose return value
//! is specified as stopping callback execution may or may not stop the
//! callback execution in one or more threads. It's also possible that one or
//! more execution threads will start executing all the active callbacks again.
//!
//! For best results always use a single execution thread to invoke all
//! callbacks.
//!
//! \par
//! Callbacks that return values.
//!
//! create() requires two additional parameters if callbacks have a non-void
//! return value:
//!
//! \code
//! typedef INSERT_LIBX_NAMESPACE::callback_list<int ()> my_callback_list;
//!
//! my_callback_list callbacks=my_callback_list::create(
//!     []
//!     (int rc)
//!     {
//!          return rc < 0;
//!     },
//!     []
//!     {
//!          return -1;
//!     });
//! \endcode
//!
//! This example uses callbacks that return an \c int. After invoking
//! each active callback (see below), the first lambda gets invoked with the
//! lambda's return value. The first lambda must return a \c bool.
//! Returning \c false from the lambda stops the execution of any other
//! callbacks that have not been invoke()d yet, with invoke() returning the
//! the return value from the just-executed callback.
//! Returning \c true executes the next active callbacks.
//!
//! If all callbacks have been executed, with the first lambda always returning
//! \c true, the second lambda gets called, and invoke() returns its value:
//!
//! \code
//! int n=callbacks->invoke();
//! \endcode
//!
//! In this case, each callback gets invoked until a callback returns a
//! non-negative value. invoke() returns that value, or returns -1 if
//! all callbacks have been invoked.
//!
//!
//! \see callback_list_impl
//! \see callback_map_impl
//! \see callback_multimap_impl

template<typename signature, typename container_type>
class callback_containerObj : public callback_container_implObj<signature,
								container_type>
{
	typedef callback_return_type_t<signature> return_type_t;

	using callback_container_implObj<signature, container_type>::logger;

	std::function<bool (return_type_t)> keepgoing;

	std::function<return_type_t()> default_return_type;

public:

	template<typename functor_1_type,
		 typename functor_2_type>
	callback_containerObj(functor_1_type &&functor1,
			      functor_2_type &&functor2)
		: keepgoing(std::forward<functor_1_type>(functor1)),
		  default_return_type(std::forward<functor_2_type>(functor2))
	{
	}

	using typename callback_container_implObj<signature,
						  container_type>
	::container_info;

	//! Invoke callbacks.

	//! Arguments are forwarded to each active callback that gets invoked.
	//!
	//! Any exceptions thrown by each callback get caught.

	template<typename ...invokeArgs>
	return_type_t invoke(invokeArgs && ...invokeargs)
	{
		while (1)
		{
			// Call advance(), then retrieve the current iterator's
			// callback, while holding the mutex.

			auto value=
				({
					typename mpobj<container_info>::lock
						lock{this->container};

					if (!this->advance(lock))
						return default_return_type();

					lock->callbacks.dereference
						(lock->current_iterator);
				});

			try {
				auto ret=value(std::forward<invokeArgs>
					       (invokeargs)...);

				if (!keepgoing(ret))
				{
					typename mpobj<container_info>::lock
						lock{this->container};

					// We need to manually advance through
					// the remaining callbacks, in order to
					// wrap things up. Must call advance()
					// at least once.

					this->advance(lock);

					lock->current_iterator=
						lock->callbacks.end();
					return ret;
				}
			} catch (const exception &e)
			{
				e->caught();
			} catch (...)
			{
				LOG_ERROR("An unknown exception was thrown in a middle of a callback.");
			}
		}
	}
};

//! Specialization for a container of callbacks that return void.

//! Specialization of callback_containerObj when the callbacks return void.

template<typename ...Args, typename container_type>
class callback_containerObj< void(Args...), container_type>
	: public callback_container_implObj<void(Args...), container_type> {

	using callback_container_implObj<void(Args...), container_type>::logger;

public:
	using typename callback_container_implObj<void(Args...),
						  container_type>
	::container_info;

	//! Invoke callbacks.

	//! Arguments are forwarded to each active callback that gets invoked.
	//!
	//! Any exceptions thrown by each callback get caught.

	template<typename ...invokeArgs>
	void invoke(invokeArgs && ...invokeargs)
	{
		while (1)
		{
			// Call advance(), then retrieve the current iterator's
			// callback, while holding the mutex.

			auto value=
				({
					typename mpobj<container_info>::lock
						lock{this->container};

					if (!this->advance(lock))
						return;

					lock->callbacks.dereference
						(lock->current_iterator);
				});

			try {
				value(std::forward<invokeArgs>(invokeargs)...);
			} catch (const exception &e)
			{
				e->caught();
			} catch (...)
			{
				LOG_ERROR("An unknown exception was thrown in a middle of a callback.");
			}
		}
	}
};

#if 0
{
#endif
}

template<typename signature, typename container_type>
LOG_CLASS_INIT(LIBCXX_NAMESPACE::callback_container_implObj<signature,
	       container_type>);

#endif
