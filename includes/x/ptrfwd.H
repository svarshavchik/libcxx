/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ptrfwd_H
#define x_ptrfwd_H

#include <utility>
#include <functional>
#include <x/concepts.H>
#include <x/ptrimpl.H>

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

template<typename objClass, typename baseClass>
class const_ptr : public ptrImpl<objClass>,
		  public std::compare_three_way {
public:

	using ptrImpl<objClass>::operator bool;
	using ptrImpl<objClass>::operator !;
	//! Provide for this pointer's base class.

	typedef baseClass base;

	//! Define this pointer's object class.

	typedef objClass obj_type;

	const_ptr() noexcept LIBCXX_INLINE;

	explicit
	const_ptr(const objClass *pArg) noexcept LIBCXX_INLINE;

	explicit
	const_ptr(const ptrImpl<objClass> &o) noexcept LIBCXX_INLINE;

	template<typename baseClass2>
	explicit const_ptr(const_ptr<objClass, baseClass2> &&o)
		noexcept;

	template<typename baseClass2>
	explicit const_ptr(ptr<objClass, baseClass2> &&o)
		noexcept;

	template<typename baseClass2>
	explicit
	const_ptr(const const_ref<objClass, baseClass2> &o) noexcept;

	template<typename baseClass2>
	explicit
	const_ptr(const_ref<objClass, baseClass2> &&o) noexcept;

	template<typename baseClass2>
	explicit
	const_ptr(const ref<objClass, baseClass2> &o) noexcept;

	template<typename baseClass2>
	explicit
	const_ptr(ref<objClass, baseClass2> &&o) noexcept;

	~const_ptr() LIBCXX_INLINE;
	const objClass &operator*() const LIBCXX_INLINE;

	const objClass *operator->() const LIBCXX_INLINE;

	const_ptr<objClass, baseClass>
	&operator=(const ptrImpl<objClass> &o)
		noexcept;

	const_ptr<objClass, baseClass>
	&operator=(ptrImpl<objClass> &&o)
		noexcept LIBCXX_INLINE;

	template<typename objClass2, typename baseClass2>
	friend class const_ptr;

	template<typename objClass2>
	const_ptr(const ptrImpl<objClass2> &o);

	template<typename objClass2> const_ptr<objClass, baseClass>
	&operator=(const ptrImpl<objClass2> &o);

	template<typename objClass2> const_ptr<objClass, baseClass>
	&operator=(const objClass2 *o);

	template<typename... Args_t>
	static auto create(Args_t &&...args) LIBCXX_INLINE;

	template<typename T>
	auto operator<=>(const T &o)
		const noexcept requires is_derived_from_ref_or_ptr<T>
		LIBCXX_INLINE;

	template<typename T>
	bool operator==(const T &o)
		const noexcept requires is_derived_from_ref_or_ptr<T>
		LIBCXX_INLINE;

	template<typename T>
	bool operator!=(const T &o)
		const noexcept requires is_derived_from_ref_or_ptr<T>
		LIBCXX_INLINE;

	template<typename T>
	bool operator<(const T &o)
		const noexcept requires is_derived_from_ref_or_ptr<T>
		LIBCXX_INLINE;

	template<typename T>
	bool operator<=(const T &o)
		const noexcept requires is_derived_from_ref_or_ptr<T>
		LIBCXX_INLINE;

	template<typename T>
	bool operator>(const T &o)
		const noexcept requires is_derived_from_ref_or_ptr<T>
		LIBCXX_INLINE;

	template<typename T>
	bool operator>=(const T &o)
		const noexcept requires is_derived_from_ref_or_ptr<T>
		LIBCXX_INLINE;
};

//! A pointer to a reference-counted object.

//! This template defines a pointer to a reference-counted object.
//! Most objects implemented by this library are reference-counted objects.
//!
//! All reference-counted objects are virtually derived from \ref obj "obj".
//! All reference-counted objects must be constructed with create():
//!
//! \code
//! class widgetObj : virtual public INSERT_LIBX_NAMESPACE::obj {
//!  // ...
//! };
//!
//! typedef INSERT_LIBX_NAMESPACE::ref<widgetObj> widget;
//!
//! typedef INSERT_LIBX_NAMESPACE::ref<widgetObj> widgetptr;
//!
//! widget w=widget::create();
//!
//! widgerptr wp=widgetptr::create();
//! \endcode
//!
//! The template argument to \ref ptr "ptr" is the reference-counted object
//! class, which must be virtually derived from \ref obj "obj".
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
//! refers to some instance of its class and can never be NULL.
//!
//! Each ptr/const_ptr/ref/const_ref can be moved/copied, and each instance
//! of it included in the reference count. When the last
//! ptr/const_ptr/ref/const_ref goes out of scope, the referenced object
//! gets \c deleted-d.
//!
//! \par Naming convention
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
//!     ~myClassObj();
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
//! \ref ptrref_base "ptrref_base", which supplies a single static
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
//! \par Making weak pointers
//!
//! \code
//! INSERT_LIBX_NAMESPACE::weakptr<ptr<<obj>> weakling=classP.weaken();
//! \endcode
//!
//! weaken() is provided for convenience, which constructs
//! a \ref weakptr "weak pointer" from this strong pointer.
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
//! #include <destroy_callback.H>
//!
//! INSERT_LIBX_NAMESPACE::destroy_callback cb(INSERT_LIBX_NAMESPACE::destroy_callback::create());
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
//! \ref destroy_callback "INSERT_LIBX_NAMESPACE::destroyCallbackFlag"
//! implements a frequent design pattern where some object's destruction
//! does not itself takes any immediate action. Rather, at some point later,
//! a check is made whether the object is already destroyed. If not,
//! wait for its destruction.
//!
//! \c INSERT_LIBX_NAMESPACE::destroy_callback is itself a reference to
//! a reference-counted object.
//!
//! Use \c ondestroy() on another object to invoke
//! \c INSERT_LIBX_NAMESPACE::destroy_callback's destroyed() method.
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
	using const_ptr<objClass, baseClass>::operator bool;
	using const_ptr<objClass, baseClass>::operator !;

	ptr() noexcept LIBCXX_INLINE;

	explicit ptr(objClass *pArg) noexcept
		LIBCXX_INLINE;

	explicit
	ptr(const ptr<objClass, baseClass> &o) noexcept
		LIBCXX_INLINE;

	explicit
	ptr(ptr<objClass, baseClass> &&o) noexcept
		LIBCXX_INLINE;

	template<typename baseClass2>
	explicit
	ptr(const ref<objClass, baseClass2> &o) noexcept;

	template<typename baseClass2>
	explicit
	ptr(ref<objClass, baseClass2> && o) noexcept;

	template<typename objClass2, typename baseClass2>
	friend class const_ref;

private:

	template<typename baseClass2>
	explicit
	ptr(const const_ref<objClass, baseClass2> &o) noexcept;

	template<typename baseClass2>
	explicit
	ptr(const_ref<objClass, baseClass2> && o) noexcept;

public:

	~ptr() LIBCXX_INLINE;

	objClass &operator*() const
		LIBCXX_INLINE;
	objClass *operator->() const
		LIBCXX_INLINE;

	ptr<objClass, baseClass>
	&operator=(const ptr<objClass, baseClass> &o)
		noexcept LIBCXX_INLINE;

	ptr<objClass, baseClass>
	&operator=(ptr<objClass, baseClass> &&o)
		noexcept LIBCXX_INLINE;

	using const_ptr<objClass, baseClass>::operator<=>;
	using const_ptr<objClass, baseClass>::operator==;
	using const_ptr<objClass, baseClass>::operator!=;
	using const_ptr<objClass, baseClass>::operator<;
	using const_ptr<objClass, baseClass>::operator<=;
	using const_ptr<objClass, baseClass>::operator>;
	using const_ptr<objClass, baseClass>::operator>=;

	template<typename objClass2, typename baseClass2> friend class ptr;

	template<typename objClass2, typename baseClass2> inline
	ptr(const ptr<objClass2, baseClass2> &o) LIBCXX_INLINE;

	template<typename objClass2, typename baseClass2> inline
	ptr(ptr<objClass2, baseClass2> &&o)
		LIBCXX_INLINE;

	template<typename objClass2, typename baseClass2>
	ptr<objClass, baseClass>
	&operator=(const ptr<objClass2, baseClass2> &o);

	template<typename objClass2, typename baseClass2>
	ptr<objClass, baseClass>
	&operator=(ptr<objClass2, baseClass2> &&o);

	template<typename objClass2> ptr<objClass, baseClass>
	&operator=(objClass2 *o);

	ptr<objClass, baseClass>
	&operator=(std::nullptr_t) LIBCXX_INLINE;

	//! Forward create() to the base type

	template<typename... Args_t>
	static auto create(Args_t &&...args) LIBCXX_INLINE;

	auto weaken() const;
};

//! A reference to a constant object.

//! This is a version of \ref ref "ref"
//! that refers to a constant object.
//! See \ref ref "ref" for more information. With the sole %exception being that
//! dereferencing gives a constant object,
//! \ref const_ref "const_ref" is identical in all
//! other respects to \ref ref "ref". This is similar to the relationship
//! between an \c iterator and a \c const_iterator in the C++ library.
//!
//! And similar to iterators,
//! \c ref<objClass> can be converted to \c const_ref<objClass>, but not the
//! other
//! way around.

template<typename objClass, typename baseClass> class const_ref {

protected:
	//! The underlying pointer object.

	ptr<objClass, baseClass> p;

private:

	//! Return the underlying pointer

	objClass *getrefP() const noexcept LIBCXX_INLINE
	{
		return p.refP;
	}

public:
	template<typename objClass2>
	friend class ptrImpl;

	template<typename objClass2, typename baseClass2>
	friend class const_ptr;

	//! Define this pointer's base class.

	typedef baseClass base;

	//! Define this pointer's object class.

	typedef objClass obj_type;

	explicit
	const_ref(const objClass *pArg)
		LIBCXX_INLINE;

	~const_ref() LIBCXX_INLINE;

	const objClass &operator*() const
		LIBCXX_INLINE;

	const objClass *operator->() const
		LIBCXX_INLINE;

	const_ref<objClass, baseClass>
	&operator=(const ptrImpl<objClass> &o) LIBCXX_INLINE;

	const_ref<objClass, baseClass>
	&operator=(ptrImpl<objClass> &&o) LIBCXX_INLINE;

	const_ref<objClass, baseClass>
	&operator=(const const_ref<objClass, baseClass> &o)
		noexcept LIBCXX_INLINE;

	const_ref<objClass, baseClass>
	&operator=(const_ref<objClass, baseClass> &&o)
		noexcept LIBCXX_INLINE;

	//! A ref of one type is friends with refs of all other types.
	template<typename objClass2, typename baseClass2>
	friend class const_ref;

	explicit
	const_ref(const const_ref<objClass, baseClass> &o) noexcept
		LIBCXX_INLINE;

	template<typename objClass2>
	const_ref(const ptrImpl<objClass2> &o)
		LIBCXX_INLINE;

	template<typename objClass2>
	const_ref(ptrImpl<objClass2> &&o)
		LIBCXX_INLINE;

	template<typename objClass2, typename baseClass2>
	const_ref(const const_ref<objClass2, baseClass2> &o)
		LIBCXX_INLINE;

	template<typename objClass2> const_ref<objClass, baseClass>
	&operator=(const ptrImpl<objClass2> &o)
		LIBCXX_INLINE;

	operator const const_ptr<objClass, baseClass> &() const noexcept
	LIBCXX_INLINE;

	template<typename objClass2, typename baseClass2>
	operator const const_ptr<objClass2, baseClass2>() const
		LIBCXX_INLINE;

	template<typename T>
	auto operator<=>(const T &o)
		const noexcept requires is_derived_from_ref_or_ptr<T>
		LIBCXX_INLINE;

	template<typename T>
	bool operator==(const T &o)
		const noexcept requires is_derived_from_ref_or_ptr<T>
		LIBCXX_INLINE;

	template<typename T>
	bool operator!=(const T &o)
		const noexcept requires is_derived_from_ref_or_ptr<T>
		LIBCXX_INLINE;

	template<typename T>
	bool operator<(const T &o)
		const noexcept requires is_derived_from_ref_or_ptr<T>
		LIBCXX_INLINE;

	template<typename T>
	bool operator<=(const T &o)
		const noexcept requires is_derived_from_ref_or_ptr<T>
		LIBCXX_INLINE;

	template<typename T>
	bool operator>(const T &o)
		const noexcept requires is_derived_from_ref_or_ptr<T>
		LIBCXX_INLINE;

	template<typename T>
	bool operator>=(const T &o)
		const noexcept requires is_derived_from_ref_or_ptr<T>
		LIBCXX_INLINE;

	template<typename... Args_t>
	static inline auto create(Args_t &&...args) LIBCXX_INLINE;
};



//! A non-null pointer reference.

//! This template defines a pointer to a reference-counted object, and is
//! similar to \ref ptr "INSERT_LIBX_NAMESPACE::ptr", except that
//! a \ref ref "INSERT_LIBX_NAMESPACE::ref" must always refer to an instance
//! of the referenced object class.
//!
//! A \ref ptr "INSERT_LIBX_NAMESPACE::ptr" can be unbound, an equivalent of
//! a native NULL pointer. Dereferencing a NULL
//! \ref ptr "INSERT_LIBX_NAMESPACE::ptr", using its \c * or \c -> operator,
//! throws an %exception. This
//! \ref ref "INSERT_LIBX_NAMESPACE::ref" also has a \c * and a \c -> operators,
//! which work exactly like their
//! \ref ptr "INSERT_LIBX_NAMESPACE::ptr" counterparts, except that they
//! do not check if the underlying native point is NULL and
//! throw an %exception because
//! a \ref ptr "INSERT_LIBX_NAMESPACE::ptr" can never be a NULL pointer,
//!
//! This is similar to a relationship between a native C++ pointer and a
//! reference.
//!
//! There's also a \ref const_ref "INSERT_LIBX_NAMESPACE::const_ref", which
//! is the analogue for  \ref const_ptr "INSERT_LIBX_NAMESPACE::const_ptr",
//! and carries the similar semantisc.
//!
//! A \ref ref "INSERT_LIBX_NAMESPACE::ref" is generally interchangeable with
//! a \ref ptr "INSERT_LIBX_NAMESPACE::ptr". Each one of them can be
//! converted to the other. Converting or assigning a
//! \ref ptr "INSERT_LIBX_NAMESPACE::ptr" to a
//! \ref ref "INSERT_LIBX_NAMESPACE::ref" immediately throws an
//! %exception if the \ref ptr "INSERT_LIBX_NAMESPACE::ptr" is NULL. The
//! %exception gets thrown immediately, when the assignment, or a conversion,
//! takes place, and not when
//! \ref ref "INSERT_LIBX_NAMESPACE::ref"'s \c * or \c -> operator is used
//! at some pointer later.
//!
//! \ref const_ref "INSERT_LIBX_NAMESPACE::const_ref" is the equivalent of
//! \ref const_ptr "INSERT_LIBX_NAMESPACE::const_ptr".

template<typename objClass, typename baseClass>
class ref : public const_ref<objClass, baseClass> {
public:
	template<typename objClass2>
	friend class ptrImpl;

	inline explicit ref(objClass *pArg) noexcept
		LIBCXX_INLINE;


	inline ref(const ref<objClass, baseClass> &o) noexcept
		LIBCXX_INLINE;

	inline ~ref() LIBCXX_INLINE;

	inline objClass &operator*() const noexcept
		LIBCXX_INLINE;

	inline objClass *operator->() const noexcept
		LIBCXX_INLINE;

	inline ref<objClass, baseClass>
	&operator=(const ptr<objClass, baseClass> &o)
		noexcept LIBCXX_INLINE;

	inline ref<objClass, baseClass>
	&operator=(ptr<objClass, baseClass> &&o)
		noexcept LIBCXX_INLINE;

	inline ref<objClass, baseClass>
	&operator=(const ref<objClass, baseClass> &o)
		noexcept LIBCXX_INLINE;

	inline ref<objClass, baseClass>
	&operator=(ref<objClass, baseClass> &&o)
		noexcept LIBCXX_INLINE;

	using const_ref<objClass, baseClass>::operator<=>;
	using const_ref<objClass, baseClass>::operator==;
	using const_ref<objClass, baseClass>::operator!=;
	using const_ref<objClass, baseClass>::operator<;
	using const_ref<objClass, baseClass>::operator<=;
	using const_ref<objClass, baseClass>::operator>;
	using const_ref<objClass, baseClass>::operator>=;

	template<typename objClass2, typename baseClass2> friend class ref;

	template<typename objClass2, typename baseClass2> inline
	ref(const ptr<objClass2, baseClass2> &o)
		LIBCXX_INLINE;

	template<typename objClass2, typename baseClass2> inline
	ref(ptr<objClass2, baseClass2> &&o)
		LIBCXX_INLINE;

	template<typename objClass2, typename baseClass2> inline
	ref(const ref<objClass2, baseClass2> &o)
		LIBCXX_INLINE;

	template<typename objClass2, typename baseClass2>
	inline ref<objClass, baseClass>
	&operator=(const ptr<objClass2, baseClass2> &o)
		LIBCXX_INLINE;

	template<typename objClass2, typename baseClass2>
	inline ref<objClass, baseClass>
	&operator=(ptr<objClass2, baseClass2> &&o)
		LIBCXX_INLINE;

	template<typename objClass2, typename baseClass2>
	inline ref<objClass, baseClass>
	&operator=(const ref<objClass2, baseClass2> &o)
		LIBCXX_INLINE;

	template<typename objClass2, typename baseClass2>
	inline ref<objClass, baseClass>
	&operator=(ref<objClass2, baseClass2> &&o)
		LIBCXX_INLINE;

	template<typename objClass2> inline ref<objClass, baseClass>
	&operator=(objClass2 *o)
		LIBCXX_INLINE;

	inline operator const ptr<objClass, baseClass> &() const noexcept;

	inline operator const const_ptr<objClass, baseClass> &() const noexcept;

	template<typename objClass2, typename baseClass2>
	inline operator const ptr<objClass2, baseClass2>() const;

	template<typename objClass2, typename baseClass2>
	inline operator const const_ptr<objClass2, baseClass2>() const;

	template<typename... Args_t>
	static inline auto create(Args_t &&...args) LIBCXX_INLINE;

	inline auto weaken() const LIBCXX_INLINE;
};

class obj;

//! Create a reference or a nullable pointer reference to a new object.

//! This is the default second parameter to
//! \ref ref "INSERT_LIBX_NAMESPACE::ref" and
//! \ref ptr "INSERT_LIBX_NAMESPACE::ptr", which becomes
//! \ref ref "INSERT_LIBX_NAMESPACE::ref<class>::base" and
//! \ref ptr "INSERT_LIBX_NAMESPACE::ptr<class>::base".
//!
//!

class ptrref_base {

public:

	//! Instantiate a new class intance, put it into a reference or a pointer.

	//! \ref ref "INSERT_LIBX_NAMESPACE::ref<class>" and
	//! \ref ptr "INSERT_LIBX_NAMESPACE::ptr<class>" define a %create()
	//! that calls \c %base::factory&lt;TYPE&gt;::%create(), where \c TYPE
	//! is the \c ref or the \c ptr type, with the arguments to
	//! %create() getting forwarded.

	template<typename ptrrefType> class objfactory
	{
	public:

		template<typename... Args_t>
		static inline ptrrefType
		create(Args_t &&...args) LIBCXX_INLINE;
	};

	//! Used by create() to determine if an object implement constructor().

	//! The first parameter is an object class. The second parameter is a
	//! tuple of types of the parameters to create().

	template<typename objClass, typename tuple_type, typename C=void>
	struct has_init {

		//! The object does not have a suitable init().

		static constexpr bool value=false;
	};

	template<typename ...Args> struct create_args;

	//! Specialization that determines if an object implement constructor().

	template<typename objClass, typename ...Args>
	struct has_init<objClass, create_args<Args...>,
			decltype( std::declval<objClass &>().constructor
				  ( std::declval<Args &&>()... ))> {

		//! The object has a suitable init().

		static constexpr bool value=true;
	};
	template<typename ptrrefType, typename args,
		 bool has_init=has_init<typename ptrrefType::obj_type,
					args>::value>
	struct create_and_init;
};

//! Define the default base class for a \ref ptr "ptr" or a \ref ref "ref".

//! The default base class is ptrref_base

template<typename>
struct base_type {
public:

	typedef ptrref_base type;
};

template<typename objClass, typename type=typename base_type<objClass>
	 ::type> class ptr;

template<typename objClass, typename type=typename base_type<objClass>
	 ::type> class const_ptr;

template<typename objClass, typename type=typename base_type<objClass>
	 ::type> class ref;

template<typename objClass, typename type=typename base_type<objClass>
	 ::type> class const_ref;

//! Deduction guide for a \ref ptr "ptr".

template<typename objClass> ptr(objClass *) -> ptr<objClass>;

//! Deduction guide for a \ref const_ptr "const_ptr".

template<typename objClass> const_ptr(objClass *) -> const_ptr<objClass>;

//! Deduction guide for a \ref const_ptr "const_ptr".

template<typename objClass> const_ptr(const objClass *) -> const_ptr<objClass>;

//! Deduction guide for a \ref ref "ref".

template<typename objClass> ref(objClass *) -> ref<objClass>;

//! Deduction guide for a \ref const_ref "const_ref".

template<typename objClass> const_ref(objClass *) -> const_ref<objClass>;

//! Deduction guide for a \ref const_ref "const_ref".

template<typename objClass> const_ref(const objClass *) -> const_ref<objClass>;

//! create() and object that does not have a constructor().
template<typename ptrrefType, typename args >
struct ptrref_base::create_and_init<ptrrefType, args, false> {

	template<typename ...Args>
	static inline ptrrefType create(Args && ...argsValue)
		requires is_derived_from_obj<typename ptrrefType::obj_type>
		LIBCXX_INLINE;
};

//! create() and object with a constructor().
template<typename ptrrefType, typename args >
struct ptrref_base::create_and_init<ptrrefType, args, true> {

	template<typename ...Args>
	static inline ptrrefType create(Args && ...argsValue)
		requires is_derived_from_obj<typename ptrrefType::obj_type>
		LIBCXX_INLINE;
};

#define x_ptrfwd_H_included

#include <x/ptrfwd_obj.H>

#if 0
{
#endif
}
#endif
