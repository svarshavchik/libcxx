/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_stoppable_H
#define x_stoppable_H

#include <x/ref.H>
#include <x/ptr.H>
#include <x/weakptr.H>
#include <x/weaklist.H>
#include <x/obj.H>
#include <x/destroycallbackobj.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class stoppableObj;
class stoppableBase;

//! A reference to a stoppable object

typedef ref<stoppableObj, stoppableBase> stoppable;

//! A nullable pointer reference to a stoppable object

typedef ptr<stoppableObj, stoppableBase> stoppableptr;

//! An interface for something that's stoppable

//! An interface for a class that implements a method called stop().
//!
//! \ref stoppableBase::group() "stoppable::base::group()" installs
//! destructor callbacks for a sequence of objects. When
//! any one of those objects is destroyed, stop() gets invoked on the
//! next object in the sequence. The end result is that when any one in the
//! sequence of objects gets destroyed, all the others gets destroyed too.
//! [MUTUAL]

class stoppableObj : virtual public obj {

public:
	//! Constructor
	stoppableObj();

	//! Destructor
	~stoppableObj() noexcept;

	//! Request this thread to terminate itself

	virtual void stop()=0;
};


//! \c INSERT_LIBX_NAMESPACE::stoppable::base

//! Refer to this class as \c stoppable::base or \c stoppableptr::base

class stoppableBase {

public:

	class groupObj;

	//! A reference to a stoppable object group

	//! \code
	//! auto group=INSERT_LIBX_NAMESPACE::stoppable::base::group::create();
	//! \endcode
	//!
	//! Create a group of stoppable objects. Each object gets added to the
	//! group, individually.
	//!
	//! \code
	//! INSERT_LIBX_NAMESPACE::stoppable stoppable_obj= /* ... */;
	//!
	//! INSERT_LIBX_NAMESPACE::ref<INSERT_LIBX_NAMESPACE::obj> mcguffin=INSERT_LIBX_NAMESPACE::ref<INSERT_LIBX_NAMESPACE::obj>::create();
	//!
	//! auto group=INSERT_LIBX_NAMESPACE::stoppable::base::group::create();
	//!
	//! group->add(stoppable_obj);
	//! group->mcguffin(mcguffin);
	//! \endcode
	//!
	//! add() adds a stoppable object to the group.
	//! mcguffin() registers a mcguffin. When any registered mcguffin goes
	//! out of scope, all stoppable objects in the group have their
	//! stop() method invoked.
	//!
	//! A stoppable group maintains only weak references on the objects in
	//! the stoppable group. When they go out of scope and get destroyed,
	//! they get automatically removed from the stoppable group.
	//!
	//! When a registered mcguffin goes out of scope, all objects in the
	//! mcguffin's stoppable group get stop()ed, including objects added to
	//! the group after the mcguffin was registered.
	//!
	//! More than one mcguffin may be registered with the stoppable group.
	//! stop() gets invoked when any registered mcguffin goes out of scope
	//! and gets destroyed.

	typedef ref<groupObj> group;

	//! A nullable pointer reference to a stoppable object group
	typedef ptr<groupObj> groupptr;

	//! A reference to a constant stoppable object group
	typedef ref<groupObj> const_group;

	//! A nullable pointer reference to a constant stoppable object group
	typedef ptr<groupObj> const_groupptr;
};

//! A stoppable object group

//! See \ref stoppableBase::group "INSERT_LIBX_NAMESPACE::stoppable::base::group"
//! for more information.

class stoppableBase::groupObj : virtual public obj {

	//! members of the stoppable group.
	weaklist<stoppableObj> stoppables;

	class cbObj;

public:

	//! Constructor
	groupObj();

	//! Destructor
	~groupObj() noexcept;

	//! Add a stoppable object to the group

	//! When any stoppable group mcguffin goes out of scope, for now,
	//! this object's stop() method gets invoked.
	void add(const stoppable &member);

	//! Register a stoppable group mcguffin

	//! The passed object is made a mcguffin. Going forward, if this
	//! object goes out of scope and gets destroyed, all members of the
	//! stoppable group get stop()ed.

	void mcguffin(const ref<obj> &mcguffin);
};

#if 0
{
#endif
}
#endif
