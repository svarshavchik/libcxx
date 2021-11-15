/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mpweakptrfwd_H
#define x_mpweakptrfwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<typename ptrType> class LIBCXX_PUBLIC mpweakptrObj;

/*! Mutex-protected weak pointers

\code
class widgetObj : virtual public INSERT_LIBX_NAMESPACE::obj {

  // ...
};

typedef INSERT_LIBX_NAMESPACE::ref<widgetObj> widget;
typedef INSERT_LIBX_NAMESPACE::ptr<widgetObj> widgetptr;

#include <INSERT_LIBX_NAMESPACE/mpweakptr.H>

auto w=widget::create();

auto ww=INSERT_LIBX_NAMESPACE::mpweakptr<widgetptr>::create(w);

ww->setptr(w);

widgetptr wp=ww->getptr();
\endcode

\c mpweakptr is a reference-counted object that stores a
\ref mpobj "mutex-protected" \ref weakptr "weak pointer". The reference-counted
object implements thread-safe setptr() and getptr() methods that update the
retrieve the weakly-reference object.

The template parameter must be an instance of
a \ref ptr "INSERT_LIBX_NAMESPACE::ptr".
 */

template<typename T>
using mpweakptr=ref<mpweakptrObj<T>>;

//! A nullable pointer reference to a \ref mpweakptr "mutex-protected weak pointer.
template<typename T>
using mpweakptrptr=ptr<mpweakptrObj<T>>;

//! A reference to a const \ref mpweakptr "mutex-protected weak pointer.
template<typename T>
using const_mpweakptr=const_ref<mpweakptrObj<T>>;

//! A nullable pointer reference to a const \ref mpweakptr "mutex-protected weak pointer.

template<typename T>
using const_mpweakptrptr=const_ptr<mpweakptrObj<T>>;

#if 0
{
#endif
}
#endif
