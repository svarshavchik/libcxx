/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_threads_weaklocalfwd_H
#define x_threads_weaklocalfwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename objClass, typename baseType=ptrrefBase>
class weakthreadlocalObj;

//! Weak local thread objects

//! \see makeweakthreadlocal
//!
//! \note
//! This is a template alias for a \ref ref "ref" to a weakthreadlocalObj.

template<typename objClass, typename baseType=ptrrefBase>
#ifdef DOXYGEN
class weakthreadlocal {}; // TODO
#else
using weakthreadlocal=ref<weakthreadlocalObj<objClass, baseType> >;
#endif

//! A nullable pointer reference to a weak local thread object.

template<typename objClass, typename baseType=ptrrefBase>
#ifdef DOXYGEN
class weakthreadlocalptr {}; // TODO
#else
using weakthreadlocalptr=ptr<weakthreadlocalObj<objClass, baseType> >;
#endif

//! A constant reference to a weak local thread object.

template<typename objClass, typename baseType=ptrrefBase>
#ifdef DOXYGEN
class const_weakthreadlocal {}; // TODO
#else
using const_weakthreadlocal=const_ref<weakthreadlocalObj<objClass, baseType> >;
#endif

//! A nullable pointer reference to a constant weak local thread object.

template<typename objClass, typename baseType=ptrrefBase>
#ifdef DOXYGEN
class const_weakthreadlocalptr {}; // TODO
#else
using const_weakthreadlocalptr=const_ptr<weakthreadlocalObj<objClass, baseType> >;
#endif

//! A weak local thread object reference

//! \code
//! class cacheObj : virtual public INSERT_LIBX_NAMESPACE::obj {
//!
//! // ...
//!
//! typedef INSERT_LIBX_NAMESPACE::ref<cacheObj> cache;
//!
//! typedef INSERT_LIBX_NAMESPACE::ptr<cacheObj> cacheptr;
//!
//! // ...
//!
//! cache item=cache::create();
//!
//! INSERT_LIBX_NAMESPACE::weakthreadlocalObj<cacheObj>::ref_t weakitem=INSERT_LIBX_NAMESPACE::makeweakthreadlocal(item);
//!
//! // ...
//!
//! cacheptr itemptr=weakitem->getptr();
//! \endcode
//!
//! A weak local thread object reference is similar to a \ref weakptr "weak pointer"
//! with one additional characteristic. If the thread that created the weak
//! local thread object terminates, the weak local thread object's getptr()
//! also returns a null pointer, even if strong references to the underlying
//! object still remain in scope elsewhere.
//!
//! Like weak pointer, the weak local thread object reference also becomes
//! unbound if all strong references to the underlying object go out of scope
//! and the object gets destroyed.
//!
//! \note
//! Weak local object references must be created only in threads that are
//! started by \ref run "INSERT_LIBX_NAMESPACE::run()" (or in the main thread).
//! makeweakthreadlocal() should not be invoked in other objects' constructors
//! or destructors, if it's possible that these objects are statically scoped
//! and they may be invoked when the execution thread is starting or
//! terminating (since the order of initialization of staticly-scoped objects
//! will not guarantee that makewakthreadlocal() gets invoked when the
//! underlying supporting structures for weak local thread objects have been
//! properly initialized).

template<typename objClass, typename baseType>
inline weakthreadlocal<objClass, baseType>
makeweakthreadlocal(const ref<objClass, baseType> &refobj);

//! A weak local thread object reference

//! \overload

template<typename objClass, typename baseType>
inline weakthreadlocal<objClass, baseType>
makeweakthreadlocal(const ptr<objClass, baseType> &refobj);

#if 0
{
#endif
}

#endif
