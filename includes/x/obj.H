/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_obj_H
#define x_obj_H

#include <x/namespace.h>
#include <x/concepts.H>
#include <x/weakinfofwd.H>
#include <x/weakinfobasefwd.H>
#include <x/functionalrefptrfwd.H>

#include <list>
#include <string>
#include <mutex>
#include <type_traits>
#include <atomic>
#include <utility>

//! Library namespace

//! This namespace contains all templates and classes defined by
//! this library.

namespace LIBCXX_NAMESPACE {

#if 0
}
#endif

//! Enable object debugging.

//! A weakly-defined flag.
//!
//! In your code
//!
//! \code
//! namespace INSERT_LIBX_NAMESPACE {
//!
//!     int obj_debug=1;
//! \endcode
//!
//! Makes dump_all() available, to dump all objects and where they were
//! created.

extern int obj_debug;

class LIBCXX_PUBLIC ondestroyObj;
class exception;

//! Callable ondestroy() callbacks.

typedef functionref<void ()> ondestroy_cb_t;

//! Nullable pointer reference to an ondestroy() callback.

typedef functionptr<void ()> ondestroy_cb_tptr;

template<class refType> class weakptr;
template<class> class objFunctorCallbackObj;

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

class obj {

	//! The weak reference information associated with this object.

	//! obj_weakinfo gets initialized upon demand, when the first weak
	//! reference to this object gets created.

	weakinfoptr obj_weakinfo;

protected:
	//! A miscellaneous object mutex

	//! This miscellanous mutex is used to protect access to obj_weakinfo,
	//! and can be used by subclasses for very short term locking needs.

	mutable std::mutex objmutex;
private:

	//! Reference count of this object.

	mutable std::atomic_int32_t refcnt;
public:

	template<typename, typename> friend class ref;
	template<typename, typename> friend class const_ref;
	template<typename, typename> friend class ptr;
	template<typename, typename> friend class const_ptr;
	template<typename> friend class ptrImpl;
	friend class ptrref_base;

	//! Dump all objects in existence.

	//! obj_debug must be set to true.

	static void dump_all();

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

private:
	//! Alias

	//! See https://bugzilla.redhat.com/show_bug.cgi?id=1632063

	template<typename F>
	using functorCallbackObj=objFunctorCallbackObj<F>;

	//! An exception occured in the destructor callback functor

	static void report_exception(const exception &);

public:
	template<typename baseType> friend class ptrImpl;
	template<typename baseClass> friend class ptrBase;

	friend class exception;
	friend class weakinfoObj;
	friend class ondestroyObj;
	template<typename refType> friend class weakptr;

	//! Deleted copy constructor
	obj(const obj &)=delete;

	//! Deleted assignment operator
	obj &operator=(const obj &)=delete;

	//! The default constructor

	obj() noexcept;

	//! The destructor

	//! \note The destructor may not necessarily be invoked by the same
	//! thread that constructed the object. If more than one thread carries
	//! references to the same object, whichever thread has the last
	//! reference go the object go out of scope will end up invoking
	//! the destructor

	virtual ~obj();

	//! Helper method for constructing ondestroy() callbacks.

	template<typename functor_type> static
	ondestroy_cb_t create_ondestroy(functor_type &&functor);

	//! Install a destructor callback

	//! Add a callback to be invoked when this object gets destroyed.
	//! The lambda functor parameter gets invoked when the last reference
	//! to this object goes out of scope, and it gets automatically
	//! destroyed.
	//!
	//! \return internal iterator of the inserted callback reference, in
	//! obj_weakinfo's \c callback_list.

	template<typename functor_type>
	std::list< ondestroy_cb_t >::iterator
	ondestroy(functor_type &&functor)
	{
		return do_add_ondestroy(create_ondestroy(std::forward
							 <functor_type>
							 (functor)));
	}
private:

	//! Install a destructor callback

	std::list< ondestroy_cb_t >::iterator
	do_add_ondestroy(//! Callback
			 const ondestroy_cb_t &callback) LIBCXX_PUBLIC;

	//! Construct a weak reference.

	//! \internal This function gets invoked by the weakptr template
	//! class.
	//! \return a weak reference to this object.

	weakinfo get_weak() LIBCXX_HIDDEN;

	//! Construct a weak reference.

	//! Interface used by weakptr, calls weak() and returns the same
	//! thing. weakptr does not need to pull in everything that's in
	//! weakinfo.

	weakinfobase get_weakbase();
};

#if 0
{
#endif
}


namespace LIBCXX_NAMESPACE {

#if 0
}
#endif

//! ref(this) or ptr(this) cannot be used in a constructor or destructor

void ref_in_constructor(obj *p) __attribute__((noreturn));

#define x_obj_H_included

#include <x/ptrimpl_obj.H>
#include <x/ptrfwd_obj.H>
#include <x/ref_obj.H>
#include <x/ref_obj_ondestroy.H>

#if 0
{
#endif
}

#endif
