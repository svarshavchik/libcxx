/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

/*! \file

This file defines the \c ptr template, a nullable pointer to a reference-counted
object.

*/

#ifndef x_ptr_H
#define x_ptr_H

#include <x/namespace.h>
#include <x/errors.H>
#include <x/ptrfwd.H>
#include <x/ptrimpl.H>
#include <utility>
#include <typeinfo>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A pointer to a reference-counted constant object. \anchor ptrAnchor

//! This is a version of \ref ptr "ptr"
//! that dereferences into a constant object.
//! See \ref ptr "ptr" for more information. With the sole %exception being that
//! dereferencing gives a constant object,
//! \ref const_ptr "const_ptr" is identical in all
//! other respects to \ref ptr "ptr". This is similar to the relationship
//! between an \c iterator and a \c const_iterator in the C++ Standard
//! Template library.
//!
//! And similar to STL iterators,
//! \c ptr<objClass> can be converted to \c const_ptr<objClass>, but not the
//! other
//! way around.

template<typename objClass,
	 typename baseClass> class const_ptr : public ptrImpl<objClass> {
public:

	//! Provide for this pointer's base class.

	typedef baseClass base;

	//! Define this pointer's object class.

	typedef objClass obj_type;
					      
	//! Initialize a constant reference.
	//! \see ptr

	inline explicit
	const_ptr(const objClass *pArg=nullptr) noexcept
		__attribute__((artificial))
		: ptrImpl<objClass>(const_cast<objClass *>(pArg))
	{
	}

	//! Copy constructor

	inline explicit
	const_ptr(const ptrImpl<objClass> &o) noexcept
		__attribute__((artificial))
		: ptrImpl<objClass>(o)
	{
	}

	//! Move constructor

	inline explicit
	const_ptr(ptrImpl<objClass> &&o) noexcept
		__attribute__((artificial))
		: ptrImpl<objClass>(std::move(o))
	{
	}

	//! Copy constructor

	template<typename baseClass2>
	inline explicit
	const_ptr(const const_ref<objClass, baseClass2> &o) noexcept
		: ptrImpl<objClass>(o)
	{
	}

	//! Move constructor

	template<typename baseClass2>
	inline explicit
	const_ptr(const_ref<objClass, baseClass2> &&o) noexcept
		: ptrImpl<objClass>(std::move(o))
	{
	}

	//! Copy constructor called by const_ref

	template<typename baseClass2>
	inline explicit
	const_ptr(const ref<objClass, baseClass2> &o) noexcept
		: ptrImpl<objClass>(o)
	{
	}

	//! Move constructor called by const_ref

	template<typename baseClass2>
	inline explicit
	const_ptr(ref<objClass, baseClass2> &&o) noexcept
		: ptrImpl<objClass>(std::move(o))
	{
	}

	//! The default destructor

	inline ~const_ptr() noexcept __attribute__((artificial))
	{
	}

	//! Implement the "*" operator for pointer references

	//! Dereferencing results in a constant object.
	//!
	inline const objClass &operator*() const
		__attribute__((artificial))
	{
		if (!this->refP)
			null_ptr_deref();

		return *this->refP;
	}

	//! Implement the "->" operator for pointer references

	//! Dereferencing results in a constant object.
	//!

	inline const objClass *operator->() const
		__attribute__((artificial))
	{
		if (!this->refP)
			null_ptr_deref();

		return this->refP;
	}

	//! The assignment operator

	inline const_ptr<objClass, baseClass>
	&operator=(const ptrImpl<objClass> &o)
		noexcept __attribute__((artificial))
	{
		ptrImpl<objClass>::operator=(o);

		return *this;
	}

	//! The move operator

	inline const_ptr<objClass, baseClass>
	&operator=(ptrImpl<objClass> &&o)
		noexcept __attribute__((artificial))
	{
		ptrImpl<objClass>::operator=(std::move(o));

		return *this;
	}

	//! A const_ptr of one type is friends with all const_ptrs.
	template<typename objClass2, typename baseClass2>
	friend class const_ptr;

	//! Construct from a reference to a subclass or a superclass
	
	template<typename objClass2>
	inline const_ptr(const ptrImpl<objClass2> &o)
		__attribute__((artificial));

	//! Convert from a pointer reference to a different type

	//! This operator= function allows getting a reference to this
	//! object type's superclass or subclass.
	template<typename objClass2> inline const_ptr<objClass, baseClass>
	&operator=(const ptrImpl<objClass2> &o)
		__attribute__((artificial));

	//! Convert from a native pointer to a subclass or a superclass

	template<typename objClass2> inline const_ptr<objClass, baseClass>
	&operator=(const objClass2 *o)
		__attribute__((artificial));

	//! Forward create() to the base type

	template<typename... Args_t>
	static inline auto create(Args_t &&...args)
	{
		return baseClass
			::template objfactory< ptr<objClass, baseClass> >
			::create(std::forward<Args_t>(args)...);
	}
};

template<typename objClass, typename baseClass>
template<typename objClass2>
inline const_ptr<objClass, baseClass>::const_ptr(const ptrImpl<objClass2> &o)

	: ptrImpl<objClass>(0)
{
	operator=(o);
}

template<typename objClass, typename baseClass>
template<typename objClass2> inline const_ptr<objClass, baseClass>
&const_ptr<objClass, baseClass>::operator=(const ptrImpl<objClass2> &o)

{
	objClass *newRefP=0;

	if (o.refP)
	{
		newRefP=dynamic_cast<objClass *>(o.refP);
		if (newRefP == 0)
			invalid_cast(typeid(*o.refP).name(),
				     typeid(objClass).name());
	}

	this->setRef(newRefP);
	return *this;
}

template<typename objClass, typename baseClass>
template<typename objClass2> inline const_ptr<objClass, baseClass>
&const_ptr<objClass, baseClass>::operator=(const objClass2 *o)

{
	objClass *newRefP=0;

	if (o)
	{
		newRefP=dynamic_cast<objClass *>(const_cast<objClass2 *>(o));
		if (newRefP == 0)
			invalid_cast(typeid(*o).name(),
				     typeid(objClass).name());
	}

	this->setRef(newRefP);
	return *this;
}

//! A pointer to a reference-counted object.

//! This template defines a pointer to a reference-counted object.
//! Most objects implemented by this library are reference-counted objects.
//!
//! All reference-counted objects are virtually derived from \ref obj "obj".
//! Reference-counted objects are always allocated from the heap.
//! The template argument to \ref ptr "ptr<>" is the reference-counted object
//! class, which must be virtually derived from \ref obj "obj".
//! Most reference-based classes provide convenient static create() methods that
//! allocate a new reference-counted object on the heap.
//!
//! Reference-counted objects are not accessed directly, but via this
//! pointer object, which looks and acts like an ordinary pointer.
//! The \c * and \c -> operators work as expected. There's also a
//! \ref const_ptr "const_ptr" template class that dereferences to a constant
//! instance of an object. When the last
//! \ref ptr "ptr" or \ref const_ptr "const_ptr"
//! to a given object goes out of scope, the 
//! object gets automatically destroyed, via \c delete,
//! and any existing \ref weakptr "weak pointers" get invalidated.
//!
//! An \c INSERT_LIBX_NAMESPACE::ptr can be unbound and not refer to an
//! instance of its class, this is equivalent to a NULL pointer. Dereferencing
//! a NULL \c INSERT_LIBX_NAMESPACE::ptr throws an exception.
//!
//! Additionally, a \ref ref "INSERT_LIBX_NAMESPACE::ref" template defines
//! a non-NULL reference point, like a \ref ptr "ptr", but it always
//! refers to some instance of its class, and can never be NULL.
//!
//! Reference-counting is thread-safe. Note that this doesn't mean that the
//! objects themselves are automatically thread safe, just that the
//! recordkeeping, of how many pointer references to the object are outstanding,
//! is thread-safe.
//!
//! The naming convention is for reference-counted object classes having "Obj"
//! suffix, and define a \c typedef for the reference-counted pointer class:
//!
//! \code
//! class myClassObj : virtual public INSERT_LIBX_NAMESPACE::obj  {
//!
//! public:
//!     void myMethod();
//!
//!     myClassObj();
//!     ~myClassObj() noexcept;
//! };
//!
//! typedef INSERT_LIBX_NAMESPACE::ref<myClassObj> myClass;
//!
//! typedef INSERT_LIBX_NAMESPACE::ptr<myClassObj> myClassptr;
//!
//! // ...
//!
//! myClass classP(myClass::create());
//!
//! classP->myMethod();  // And so on...
//! \endcode
//!
//! The subclass of \ref obj "obj" must, as shown in the
//! above example, declare its destructor with \c "noexcept". Subclasses
//! of \ref obj "obj" are not allowed to throw any exceptions in their
//! destructors.
//!
//! \par The \c base class
//!
//! This pointer class contains a \c typedef member named "base" that contains
//! pointer-related static methods. The default base class is
//! \ref ptrrefBase "ptrrefBase", which supplies a single static
//! create() method that returns a \ref ptr "ptr" to a new-ed instance of the
//! object class, constructed using the default constructor.
//!
//! A second optional parameter to the \ref ptr "ptr" template is syntactical
//! sugar that
//! replaces \c ptrBase as the \c base class:
//!
//! \code
//! class myClassObj : virtual public INSERT_LIBX_NAMESPACE::obj {
//!
//! // ...
//!
//! };
//!
//! class myClassBase;
//!
//! typedef INSERT_LIBX_NAMESPACE::ptr<myClassObj, myClassBase> myClassptr;
//!
//! class myClassBase {
//! public:
//!
//!      static myClassptr create();
//!
//!      // ... Other static methods and definitions can go here
//! };
//! \endcode
//!
//! myClassptr::create() now calls this custom create() method, that
//! provides a custom construction of a new pointer. The base class can
//! define other methods, objects, and classes, that may be referenced
//! as "myClassptr::base::{name}".
//!
//! \par Unbound pointer references
//!
//! \code
//! myClassptr classP;
//! \endcode
//!
//! creates an unbound pointer
//! reference, which is equivalent to a \c NULL pointer.
//! Dereferencing it throws a null pointer %exception.
//!
//! \par Thread safety
//!
//! This reference pointer
//! implementation is thread safe. This does not mean that the underlying
//! objects, which the pointer references point to, are automatically
//! thread safe, just that all references to the same underlying object by
//! multiple threads are safely accounted for. Note that each pointer can
//! be used by a single thread only. Each individual thread can have its own
//! pointer to to the same referenced object. Different threads can use
//! the same actual pointer only if they arbitrate it amognst themselves, using
//! a mutex or some equivalent facility, so that no two threads use
//! the same pointer concurrently.
//!
//! The referenced object's destructor gets invoked by whichever thread ends
//! up having the last reference to the object go out of scope. This may not
//! be the same thread that constructed the object.
//!
//! \par Destructor callbacks
//!
//! \code
//! class mcguffinObj : virtual public INSERT_LIBX_NAMESPACE::obj  {
//!
//! // ...
//!
//! };
//!
//! INSERT_LIBX_NAMESPACE::<mcguffinObj> myobj=INSERT_LIBX_NAMESPACE::<mcguffinObj>::create();
//!
//! myobj->ondestroy([] {
//!              //! Your lambda goes here
//!         });
//! \endcode
//!
//! The \ref obj::ondestroy "ondestroy()" methods installs a functor or lambda
//! which gets invoked when the last pointer
//! reference to the object goes out of scope, and the object gets
//! destroyed.
//!
//! A destructor callback can be attached to any object derived from
//! \ref obj "INSERT_LIBX_NAMESPACE::obj". ondestroy() may be invoked more
//! than once, and each lambda gets invoked accordingly.
//!
//! Notes:
//!
//! - The lambda gets invoked by the thread in which the last
//! reference to the first object goes out of scope. This may not necessarily
//! be the same thread that created the first object, or the callback object.
//!
//! - The lambda may not reference the object being destroyed in
//! any fashion, not even through a weak reference. Doing so will may lead to
//! a thread deadlock, or memory corruption. For all practical purposes, the
//! original object no longer exists when the lambda gets invoked.
//!
//! - \c ondestroy() returns an iterator to an internal list. The internal
//! list is generally not visible. The return value gets used by
//! cancellable destructor callbacks.
//!
//! \par Simple destructor callbacks
//!
//! \code
//! #include <destroycallbackflag.H>
//!
//! INSERT_LIBX_NAMESPACE::destroyCallbackFlag cb(INSERT_LIBX_NAMESPACE::destroyCallbackFlag::create());
//!
//! INSERT_LIBX_NAMESPACE::ref<obj> someobj;
//!
//! // ...
//! someobj->ondestroy([cb]{cb->destroyed();});
//!
//! // ...
//!
//! cb->wait();
//!
//! \endcode
//!
//! \ref destroyCallbackFlag "INSERT_LIBX_NAMESPACE::destroyCallbackFlag"
//! implements a frequent design pattern where some object's destruction
//! does not itself takes any immediate action. Rather, at some point later,
//! a check is made whether the object is already destroyed. If not,
//! wait for its destruction.
//!
//! \c INSERT_LIBX_NAMESPACE::destroyCallbackFlag is itself a reference to
//! a reference-counted object.
//!
//! Use \c ondestroy() on another object to invoke
//! \c INSERT_LIBX_NAMESPACE::destroyCallbackFlag's destroyed() method.
//!
//! The \c wait() method blocks if \c destroyed() was not already invoked,
//! until it gets invoked (presumably, by some other execution thread).
//!
//! \par Cancellable destructor callbacks.
//!
//! Adding a destructor callback is irrevocable. The destructor
//! will now be invoked whenever the object gets destroyed.
//!
//! Using \ref ondestroy "INSERT_LIBX_NAMESPACE::ondestroy"
//! sets up a cancellable destructor callback, which allows
//! the destructor callback to be detached from the object it was
//! originally attached to.
//!
//! \code
//! #include <ondestroy.H>
//!
//! class mcguffinObj : virtual public INSERT_LIBX_NAMESPACE::obj {
//!
//! // ...
//!
//! };
//!
//! auto mcguffin=INSERT_LIBX_NAMESPACE::ref<mcguffinObj>::create();
//!
//! auto ondestroyHook=INSERT_LIBX_NAMESPACE::ondestroy::create([]
//!                        {
//!                            // ...
//!                        }, mcguffin, true);
//!
//! // ...
//!
//! ondestroyHook->cancel();
//!
//! \endcode
//!
//! Instead of invoking an object's ondestroy() function, instantiate a
//! standalone \c ondestroy object (which is itself a reference-counted
//! object). ondestroy()'s first parameter is a callback lambda, and
//! the second parameter is the object the destructor callback lambda gets
//! attached to. The lambda gets invoked, as usual,
//! when the object goes out of scope and gets destroyed.
//! If, for some reason it might be necessary to detach the destructor callback
//! from the object, use
//! \ref ondestroyObj::cancel() "cancel()"
//! at the appopriate time.
//!
//! \c INSERT_LIBX_NAMESPACE::ondestroy is itself
//! a pointer to a reference-counted object. If the third
//! parameter is \c true, \c cancel() gets invoked automatically when its last
//! reference goes out of scope. Otherwise, the third parameter's default
//! value of \c false leaves the destructor callback in place, and it gets
//! invoked when the object gets destroyed, as usual (if it's not destroyed
//! already).
//!
//! \note
//! cancel() can get invoked while the destructor callback is being executed
//! by another thread. cancel() blocks until the lambda returns.

template<typename objClass,
	 typename baseClass> class ptr : public const_ptr<objClass, baseClass> {
public:
	//! The default constructor

	//! The constructor takes an optional parameter, \c pArg, which must
	//! be a pointer to a heap-allocated instance of \c objClass .
	//! pArg \b must point to an object created by \c new, whose
	//! reference count gets incremented.
	//!
	//! Newly-created objects' initial reference count is zero, so here's
	//! the usual way to create new objects:
	//! \code
	//! typedef INSERT_LIBX_NAMESPACE::ptr<myClassObj> myClass;
	//!
	//! // ...
	//!
	//! myClass newObject(new myClassObj);
	//! \endcode
	//!
	//! \c newObject becomes a reference to a newly-created \c myClassObj
	//! object. A plain
	//! \code
	//! myClass newObject;
	//! \endcode
	//! becomes an unbound reference. Attempting to access \c
	//! newObject->method()
	//! throws a null pointer exception.

	inline explicit ptr(objClass *pArg=nullptr) noexcept
		__attribute__((artificial))
		: const_ptr<objClass, baseClass>(pArg)
	{
	}

	//! Copy constructor

	inline explicit
	ptr(const ptr<objClass, baseClass> &o) noexcept
		__attribute__((artificial))
		: const_ptr<objClass, baseClass>(o)
	{
	}

	//! Move constructor

	inline explicit
	ptr(ptr<objClass, baseClass> &&o) noexcept
		__attribute__((artificial))
		: const_ptr<objClass, baseClass>(std::move(o))
	{
	}

	//! Copy constructor

	template<typename baseClass2>
	inline explicit
	ptr(const ref<objClass, baseClass2> &o) noexcept
		: const_ptr<objClass, baseClass>(o)
	{
	}

	//! Move constructor

	template<typename baseClass2>
	inline explicit
	ptr(ref<objClass, baseClass2> && o) noexcept
		: const_ptr<objClass, baseClass>(std::move(o))
	{
	}

	// A const_ref is allowed to call the following constructors

	template<typename objClass2, typename baseClass2>
	friend class const_ref;

private:
	//! Copy constructor called by const_ref

	template<typename baseClass2>
	inline explicit
	ptr(const const_ref<objClass, baseClass2> &o) noexcept
		: const_ptr<objClass, baseClass>(o)
	{
	}

	//! Move constructor called by const_ref

	template<typename baseClass2>
	inline explicit
	ptr(const_ref<objClass, baseClass2> && o) noexcept
		: const_ptr<objClass, baseClass>(std::move(o))
	{
	}

public:
	//! The default destructor

	//! If this is the last reference bound to an object, the object
	//! gets automatically destroyed via \c delete.

	inline ~ptr() noexcept __attribute__((artificial))
	{
	}

	//! Implement the "*" operator for object references

	//! Using "*" on a reference works as expected, as if the reference
	//! was a native pointer.
	//! An exception gets thrown if this reference is not bound to an
	//! instance of \c objClass.
	inline objClass &operator*() const
		 __attribute__((artificial))
	{
		if (!this->refP)
			null_ptr_deref();

		return const_cast<objClass &>(*this->refP);
	}

	//! Implement the "->" operator for object references

	//! Using "->" on a reference works as expected, as if the reference
	//! was a native pointer.
	//! An exception gets thrown if this reference is not bound to an
	//! instance of \c objClass.
	inline objClass *operator->() const
		__attribute__((artificial))
	{
		if (!this->refP)
			null_ptr_deref();

		return const_cast<objClass *>(this->refP);
	}

	//! The assignment operator

	//! If this reference is bound to an object, its reference count
	//! gets decremented, and if this is its last reference, the object
	//! gets destroyed via \c delete. If \c o is bound to an instance of
	//! \c objClass, its reference count gets incremented.

	inline ptr<objClass, baseClass>
	&operator=(//! An existing reference.

		   //! If this reference is bound to an instance of \c objClass,
		   //! its reference count gets incremented.
	    
		   const ptr<objClass, baseClass> &o)
		noexcept __attribute__((artificial))
	{
		const_ptr<objClass, baseClass>::operator=(o);

		return *this;
	}

	//! The move operator

	inline ptr<objClass, baseClass>
	&operator=(//! An existing reference.

		   //! If this reference is bound to an instance of \c objClass,
		   //! its reference count gets incremented.
	    
		   ptr<objClass, baseClass> &&o)
		noexcept __attribute__((artificial))
	{
		const_ptr<objClass, baseClass>::operator=(std::move(o));

		return *this;
	}


	//! A ref of one type is friends with refs of all other types.
	template<typename objClass2, typename baseClass2> friend class ptr;

	//! Construct from a reference to a subclass or a superclass.
	
	//! \overload
	//! This constructor allows getting a reference to this
	//! object type's superclass or subclass.

	template<typename objClass2, typename baseClass2> inline
	ptr(//! A reference to another subclass or a superclass of this object.

	    //! An exception gets thrown if o cannot be converted to the right
	    //! object type.
	    const ptr<objClass2, baseClass2> &o) __attribute__((artificial));

	//! Move constructor from a subclass or a superclass.
	
	template<typename objClass2, typename baseClass2> inline
	ptr(//! Some other subclass or a superclass of this object.

	    //! An %exception gets thrown if o cannot be converted to the right
	    //! object type.
	    ptr<objClass2, baseClass2> &&o)
		__attribute__((artificial));

	//! Convert from a reference to a different type.

	//! This operator= function allows getting a reference to this
	//! object type's superclass or subclass.
	template<typename objClass2, typename baseClass2>
	inline ptr<objClass, baseClass>
	&operator=(//! Reference to this object's superclass or subclass instance.

		   //! An exception gets thrown if the reference cannot be
		   //! converted to this instance's object type.
		   const ptr<objClass2, baseClass2> &o)
		__attribute__((artificial));

	//! Move from a reference to a different type.

	template<typename objClass2, typename baseClass2>
	inline ptr<objClass, baseClass>
	&operator=(//! A pointer to a superclass or a subclass.

		   //! An %exception gets thrown if the reference cannot be
		   //! converted to this instance's object type.
		   ptr<objClass2, baseClass2> &&o)
		__attribute__((artificial));

	//! Convert from a reference to a different type.

	//! This operator= function allows getting a reference to this
	//! object type's superclass or subclass.
	template<typename objClass2> inline ptr<objClass, baseClass>
	&operator=(objClass2 *o)
		__attribute__((artificial));
};

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2> inline
ptr<objClass, baseClass>::ptr(const ptr<objClass2, baseClass2> &o)

	: const_ptr<objClass, baseClass>(nullptr)
{
	operator=(o);
}

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2> inline
ptr<objClass, baseClass>::ptr(ptr<objClass2, baseClass2> &&o)

	: const_ptr<objClass, baseClass>(nullptr)
{
	operator=(std::forward<ptr<objClass2, baseClass2> >(o));
}

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2>
inline ptr<objClass, baseClass>
&ptr<objClass, baseClass>::operator=(const ptr<objClass2, baseClass2> &o)

{
	const_ptr<objClass, baseClass>::operator=(o);
	return *this;
}

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2>
inline ptr<objClass, baseClass>
&ptr<objClass, baseClass>::operator=(ptr<objClass2, baseClass2> &&o)
{
	const_ptr<objClass, baseClass>::operator=(std::move(o));
	return *this;
}

template<typename objClass, typename baseClass>
template<typename objClass2>
inline ptr<objClass, baseClass>
&ptr<objClass, baseClass>::operator=(objClass2 *o)

{
	const_ptr<objClass, baseClass>::operator=(o);
	return *this;
}

#if 0
{
#endif
}
#endif
