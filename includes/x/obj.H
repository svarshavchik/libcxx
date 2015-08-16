/*
** Copyright 2012-2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_obj_H
#define x_obj_H

#include <x/namespace.h>
#include <x/refcnt.H>
#include <x/ref.H>
#include <x/notcopyableobj.H>

#include <list>
#include <string>
#include <mutex>
#include <type_traits>

//! Extension namespace

//! This namespace contains all templates and classes defined by
//! this library.

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class obj;
class weakinfo;
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
//!
//! \code
//! typedef INSERT_LIBX_NAMESPACE::ref<aObj> a;
//!
//! typedef INSERT_LIBX_NAMESPACE::ref<bObj> b;
//!
//! bool am_i_really_b(const a &A)
//! {
//!      return A->isa<b>();
//! }
//! \endcode
//!
//! isa() is a convenience method for trying a dynamic cast to another
//! \ref ref "ref"'s or \ref ptr "ptr"'s object.

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

	//! Whether this object is a specific subclass.

	template<typename ref_or_ptr_type>
	bool isa() const
	{
		return dynamic_cast<const typename ref_or_ptr_type::obj_type *>
			(this)
			!= nullptr;
	}

	//! Destructor callbacks

	class destroyCallbackObj;

private:
	//! Helper class for invoking destructor callback functors/lambdas

	template<typename F> class functorCallbackObj;

	//! An exception occured in the destructor callback functor

	static void report_exception(const exception &);

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

	//! Helper method for constructing ondestroy() callbacks.

	template<typename functor_type> static
	ref<destroyCallbackObj> create_ondestroy(functor_type &&functor);

	//! Install a destructor callback

	//! Add a callback to be invoked when this object gets destroyed.
	//! The lambda functor parameter gets invoked when the last reference
	//! to this object goes out of scope, and it gets automatically
	//! destroyed.
	//!
	//! \return internal iterator of the inserted callback reference, in
	//! weakinforef's \c callback_list.

	template<typename functor_type>
	std::list< ref<destroyCallbackObj> >::iterator
	ondestroy(functor_type &&functor)
	{
		return do_add_ondestroy(create_ondestroy(std::forward
							 <functor_type>
							 (functor)));
	}
private:

	//! Install a destructor callback

	std::list< ref<destroyCallbackObj> >::iterator
	do_add_ondestroy(//! Callback
			  const ref<destroyCallbackObj> &callback);

	//! Construct a weak reference.

	//! \internal This function gets invoked by the weakptr template
	//! class.
	//! \return a weak reference to this object.

	ptr<weakinfo> weak();
};

//! Destructor callbacks

class obj::destroyCallbackObj : virtual public obj {

public:
	friend class obj;

	//! Default constructor

	destroyCallbackObj() noexcept;

	//! Default destructor
	~destroyCallbackObj() noexcept;

	//! The callback

	virtual void destroyed()=0;
};
	//! Helper class for invoking destructor callback functors/lambdas

template<typename F> class obj::functorCallbackObj
	: public destroyCallbackObj {

	//! The functor
	F functor;

public:
	//! Constructor
	template<typename FArg>
	functorCallbackObj(FArg && functorArg)
		: functor(std::forward<FArg>(functorArg))
	{
	}

	//! Destructor
	~functorCallbackObj() noexcept
	{
	}

	//! The destructor callback

	void destroyed() override
	{
		functor();
	}
};

template<typename functor_type>
ref<obj::destroyCallbackObj> obj::create_ondestroy(functor_type &&functor)
{
	// If the passed functor is itself a reference, it is
	// a near certainty that this is wrong.

	typedef typename std::enable_if<
		! std::is_lvalue_reference<functor_type>::value &&
		! std::is_rvalue_reference<functor_type>::value,
		functor_type>::type functor_cannot_be_a_reference
		__attribute__((unused));

	return ref<functorCallbackObj< functor_type > >
		::create(std::forward<functor_type>(functor));
}

#if 0
{
#endif
}


namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! ref(this) or ptr(this) cannot be used in a constructor or destructor

void ref_in_constructor(obj *p) __attribute__((noreturn));

template<typename objClass>
inline void ptrImpl<objClass>::setRef(objClass *newRefP) noexcept
{
	if (newRefP && newRefP->refcnt::refadd(1) == 0)
		ref_in_constructor(newRefP);

	objClass *oldRef;

	__atomic_exchange(&refP, &newRefP, &oldRef, __ATOMIC_SEQ_CST);

	if (oldRef && oldRef->refcnt::refadd(-1) == 0)
		oldRef->obj::destroy();
}

template<typename objClass>
inline void ptrImpl<objClass>::setRef_noconscheck(objClass *newRefP) noexcept
{
	if (newRefP)
		newRefP->refcnt::refadd(1);

	objClass *oldRef;

	__atomic_exchange(&refP, &newRefP, &oldRef, __ATOMIC_SEQ_CST);

	if (oldRef && oldRef->refcnt::refadd(-1) == 0)
		oldRef->obj::destroy();
}

template<typename objClass>
inline ptrImpl<objClass>::ptrImpl(objClass *pArg)
	noexcept
	: refP(pArg)
{
	if (refP && refP->refcnt::refadd(1) == 0)
		ref_in_constructor(refP);
}

template<typename objClass>
inline ptrImpl<objClass>::~ptrImpl() noexcept
{
	if (refP && refP->refcnt::refadd(-1) == 0)
		refP->obj::destroy();
}

template<typename objClass>
inline ptrImpl<objClass>::ptrImpl(const ptrImpl<objClass> &o )
	noexcept : refP(o.refP)
{
	if (refP)
		refP->refcnt::refadd(1);
}

#if 0
{
#endif
}
#endif
