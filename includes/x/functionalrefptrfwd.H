/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_functionalrefptrfwd_H
#define x_functionalrefptrfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename signature> class functionObj;
template<typename signature> class functionBase;

//! A reference-counted \ref function "function object".

//! create(lambda) returns an implemenation subclass of a
//! \ref functionObj "INSERT_LIBX_NAMESPACE::functionObj<signature>".
//!
//! Typical usage of a callback object:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::functionref<size_t(const char *)> ref=INSERT_LIBX_NAMESPACE::functionref<size_t(const char *)>::create(
//!                                           (const char *p)
//!                                           {
//!                                                 return strlen(p);
//!                                           });
//! INSERT_LIBX_NAMESPACE::function< size_t(const char *)> &p= *ref;
//!
//! int l=p("foo");
//!
//! int l=ref->invoke("bar");
//! \endcode
//!
//! create() constructs a
//1 \ref ref "reference-counted object" that inherits from
//! \ref function "INSERT_LIBX_NAMESPACE::function". It serves the
//! same type-erasure function as
//! \ref function "INSERT_LIBX_NAMESPACE::function", but as a reference-counted
//! object.
//!
//! This is a reference to a \ref functionObj "functionObj<signature>".
//! In addition to having the \c operator() operator available,
//! the \c invoke() method offers a more convenient syntax of invoking the
//! type-erased lambda, using the reference pointer.
//!
//! \see functionObj
//! \see functionptr

template<typename signature>
using functionref=ref<functionObj<signature>, functionBase<signature>>;

//! A nullable pointer to a \ref functionref "reference to a function object".

//! \see functionref
template<typename signature>
using functionptr=ptr<functionObj<signature>, functionBase<signature>>;

#if 0
{
#endif
}
#endif
