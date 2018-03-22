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

template<typename signature> class functionrefptr_impl;

//! A reference-counted \ref function "function object".

//! Implement type-erasure of callable object, like \c std::function
//! and \ref function "INSERT_LIBX_NAMESPACE::function",
//! but based on a reference-counted object derived from
//! \ref functionObj "INSERT_LIBX_NAMESPACE::functionObj<signature>".
//!
//! Typical usage of a callable object:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::functionref<size_t(const char *)> r{[]
//!                                        (const char *p)
//!                                        {
//!                                              return strlen(p);
//!                                        }};
//!
//! auto r2=r;
//!
//! INSERT_LIBX_NAMESPACE::function< size_t(const char *)> &p= *r;
//!
//! int l=p("foo");
//!
//! int l=r("bar");
//! \endcode
//!
//! This is a
//1 \ref ref "reference-counted object" that inherits from
//! \ref function "INSERT_LIBX_NAMESPACE::function". It serves the
//! same type-erasure purpose as
//! \ref function "INSERT_LIBX_NAMESPACE::function", but as a reference-counted
//! object.
//!
//! This means that, once constructed, copying
//! \c INSERT_LIBX_NAMESPACE::functionref
//! does not create a new function object. In the above example \c r2 and
//! \c r reference the same function object. A callable object may have
//! non-trivial amount of captured members, but copying it is cheap, and
//! involves only updating the internal reference count.
//!
//! This is a reference to a \ref functionObj "functionObj<signature>".
//!
//! \see functionObj
//! \see functionptr

template<typename signature>
using functionref=functionrefptr_impl<ref<functionObj<signature>,
					  functionBase<signature>>>;

//! A nullable pointer to a \ref functionref "reference to a function object".

//! \see functionref
template<typename signature>
using functionptr=functionrefptr_impl<ptr<functionObj<signature>,
					  functionBase<signature>>>;

#if 0
{
#endif
}
#endif
