/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_eventqueuedestroynotifyobj_H
#define x_eventqueuedestroynotifyobj_H

#include <x/eventqueue.H>
#include <x/weakptr.H>
#include <x/ref.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<typename argType> class eventqueuedestroynotifyObj;

template<typename argType> class eventqueuedestroynotifyBase;

//! Notify an event queue when another object gets destroyed

//! \code
//!
//! INSERT_LIBX_NAMESPACE::eventfd efd(INSERT_LIBX_NAMESPACE::eventfd::create());
//!
//! typedef INSERT_LIBX_NAMESPACE::eventqueue<qargClass> q_type;
//!
//! q_type q(q_type::create(efd));
//!
//! INSERT_LIBX_NAMESPACE::ref<myObj> myobj(INSERT_LIBX_NAMESPACE::ref<myObj>::create());
//!
//! // ...
//!
//! typedef INSERT_LIBX_NAMESPACE::eventqueuedestroynotify<qargClass> notify_t;
//!
//!
//! notify_t callback(notify_t::create(q, qargClass(), myobj));
//!
//! while (!callback->wasdestroyed())
//! {
//!     efd->event();
//! }
//!
//! qargClass value(q->pop());
//!
//! \endcode
//!
//! When \c myobj is destroyed, \c qargClass gets pushed into the given event
//! queue (and the event file descriptor gets notified)
//!
//! \note
//! This is a template alias for a \ref ref "ref" to an eventqueuedestroynotifyObj.
template<typename argType>
#ifdef DOXYGEN
class eventqueuedestroynotify {};
#else
using eventqueuedestroynotify=ref<eventqueuedestroynotifyObj<argType>,
				  eventqueuedestroynotifyBase<argType> >;
#endif

//! A nullable reference pointer to the event queue notification object.

//! \see eventqueuedestroynotify

template<typename argType>
#ifdef DOXYGEN
class eventqueuedestroynotifyptr {};
#else
using eventqueuedestroynotifyptr=ptr<eventqueuedestroynotifyObj<argType>,
				     eventqueuedestroynotifyBase<argType> >;
#endif

//! Refer to this class as eventqueuedestroynotify<argType>::base or eventqueuedestroynotify<argType>::base

template<typename argType> class eventqueuedestroynotifyBase {

public:

	//! A factory for a custom create()

	template<typename refptrType> class objfactory {

	public:
		//! Create a destructor reference

		static refptrType
		create(//! Notify this event file descriptor
		       const eventqueue<argType> &eventqueueArg,

		       //! Event queue object
		       const argType &argArg,

		       //! When this object goes out of scope
		       const ref<obj> &objArg)
		{
			auto r(ptrrefBase::objfactory<refptrType>
			       ::create(eventqueueArg, argArg));
			r->install(objArg);

			return r;
		}
	};
};

//! Notify an event queue when an object gets destroyed

//! See eventqueuedestroynotify for more information.
//!

template<typename argType>
class eventqueuedestroynotifyObj : virtual public obj {

	//! Weak reference to the event queue that will be notified

	weakptr< eventqueueptr<argType> > q;

	//! Argument that will be placed on the event queue

	argType arg;

	//! Flag - object destroyed

	bool destroyedflag;

public:

	template<typename friendType>
	friend class eventqueuedestroynotifyBase;

	//! Constructor

	eventqueuedestroynotifyObj(//! Event queue
				   const eventqueue<argType> &eventqueueArg,

				   //! Event queue object
				   const argType &argArg)
		: q(eventqueueArg), arg(argArg), destroyedflag(false)
	{
	}

	//! Destructor

	~eventqueuedestroynotifyObj() noexcept
	{
	}

	//! Check if the object was destroyed

	//! \c true if the object was destroyed.

	bool wasdestroyed()
	{
		std::lock_guard<std::mutex> lock(objmutex);

		return destroyedflag;
	}

private:

	//! Attach this callback to another object

	void install(const ref<obj> &obj)
	{
		auto me=ref<eventqueuedestroynotifyObj<argType>>(this);

		obj->ondestroy([me]{me->destroyed();});
	}

	//! Destructor notification

	void destroyed()
	{
		eventqueueptr<argType> e;

		{
			std::lock_guard<std::mutex> lock(objmutex);

			destroyedflag=true;

			e=q.getptr();
		}

		if (!e.null())
			e->event(arg);
	}
};

#if 0
{
#endif
}
#endif
