/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_obj_H
#define x_obj_H

#include <x/namespace.h>
#include <x/refcnt.H>
#include <x/ptr.H>
#include <x/notcopyableobj.H>

#include <list>
#include <string>
#include <mutex>

//! Extension namespace

//! This namespace contains all templates and classes defined by
//! this library.

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class obj;
class weakinfo;
class destroyCallbackObj;
class ondestroyObj;
class exception;

template<class refType> class weakptr;

//! Superclass of reference-counted objects

//! The \ref obj "obj" class should be the superclass for all
//! reference-counted objects. \ref obj "obj" maintains an internal
//! reference count for this instance of the object. Your superclass should
//! be \b virtually derived from \ref obj "obj", like this:
//!
//! \code
//! class myClass : virtual public obj {
//!
//!    ....
//! };
//! \endcode
//!
//! Virtually deriving from \ref obj "obj" is required for
//! multiple inheritance to work correctly.
//!
//! \ref obj "obj"'s definition prohibits copying (the copy constructor
//! and the assignment operator are private, and not implemented). All
//! subclasses of \ref obj "obj" must be allocated on the heap, by
//! \c new.
//!
//! Subclass destructors must be explicitly declared as
//! "noexcept", signifying that the destructors do not throw any exceptions.
//! This is required.
//!
//! Instances of \ref obj "obj" and its superclasses are
//! not accessed directly, using a native pointer, but using
//! a "reference" handle. See the
//! \ref ptr "ptr" template class for more information.
//!
//! \see ptr

class obj : public refcnt, public notcopyableObj {

	//! The weak reference information associated with this object.

	//! weakinforef gets initialized upon demand, when the first weak
	//! reference to this object gets created.

	ptr<weakinfo> weakinforef;

protected:
	//! A miscellaneous object mutex

	//! This miscellanous mutex is used to protect access to weakinforef,
	//! and can be used by subclasses for very short term locking needs.

	mutable std::mutex objmutex;
public:

	//! Demangle a C++ type

	static void demangle(//! Mangled C++ type name

			     const char *mangled,

			     //! The demangled name is placed here
			     std::string &demangled) noexcept;

	//! Name of this object
	std::string objname() const;

	//! Destroy myself

	//! \internal
	//! This function gets invoked when the object's reference count
	//! goes down to zero.

	void destroy() noexcept;
public:
	template<typename baseType> friend class ptrImpl;
	template<typename baseClass> friend class ptrBase;

	friend class exception;
	friend class weakinfo;
	friend class ondestroyObj;
	template<typename refType> friend class weakptr;

	//! The default constructor

	obj() noexcept;

	//! The destructor

	//! \note The destructor may not necessarily be invoked by the same
	//! thread that constructed the object. If more than one thread carries
	//! references to the same object, whichever thread has the last
	//! reference go the object go out of scope will end up invoking
	//! the destructor

	virtual ~obj() noexcept;

	//! Install a destructor callback

	//! Add a callback to be invoked when this object gets destroyed.
	//! callback's ondestroy() method gets invoked when this object
	//! gets destroyed.
	//!
	//! \return the iterator of the inserted callback reference, in
	//! weakinforef's \c callback_list.
	std::list< ref<destroyCallbackObj> >::iterator
	addOnDestroy(//! Callback
		     const ref<destroyCallbackObj> &callback);

private:
	//! Construct a weak reference.

	//! \internal This function gets invoked by the weakptr template
	//! class.
	//! \return a weak reference to this object.

	ptr<weakinfo> weak();
};

#if 0
{
#endif
}


namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<typename objClass>
inline void ptrImpl<objClass>::setRef(objClass *newRefP) noexcept
{
	if (newRefP)
		newRefP->refcnt::refadd(1);

	objClass *oldRef=refP;

	refP=newRefP;

	if (oldRef && oldRef->refcnt::refadd(-1) == 0)
		static_cast<obj *>(oldRef)->destroy();
}

#if 0
{
#endif
}
#endif

