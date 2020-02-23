/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weaklistfwd_H
#define x_weaklistfwd_H

#include <x/ref.H>
#include <x/ptr.H>
#include <x/weakptr.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename T, typename Allocator> class weaklistObj;
template<typename T, typename Allocator> class weaklistBase;

//! A list of weak references.

//! This template defines a reference-counted object that implements a version
//! of \c std::list that stores weak references.
//!
//! This template is similar to weakmap, except that it uses a std::list
//! container. This template's semantics are same as weakmap's, with the
//! following %exception:
//!
//! \code
//! class widgetObj : virtual public obj {
//!
//! // ...
//! };
//!
//! typedef INSERT_LIBX_NAMESPACE::ref<widgetObj> widget;
//!
//! typedef INSERT_LIBX_NAMESPACE::ptr<widgetObj> widgetptr;
//!
//! typedef INSERT_LIBX_NAMESPACE::weaklist<widgetObj> weakwidgetlist;
//!
//! typedef INSERT_LIBX_NAMESPACE::weaklistptr<widgetObj> weakwidgerlistptr;
//!
//! weakwidgetlist weakObjList=weakwidgetlist::create();
//!
//! widget obj(widget::create());
//!
//! weakObjList->push_back(obj);
//!
//! // ...
//!
//! weakwidgetlist::base::iterator b, e;
//!
//! b=weakObjList->begin();
//! e=weakObjList->end();
//!
//! while (b != e)
//! {
//!    widgetptr c= b->getptr();
//!    ++b;
//!
//!    if (!c.null())
//!    {
//!       // ...
//!    }
//! }
//! \endcode
//!
//! \c INSERT_LIBX_NAMESPACE::weaklist<class> defines a
//! \ref ref "ref", and
//! \c INSERT_LIBX_NAMESPACE::weaklistptr<class> defines a
//! \ref ref "ptr" to a reference-counted weak list container.
//!
//! The only methods that directly modify the weak list is push_front() and
//! push_back(), which carry similar semantics as std::list's.
//!
//! The container stores \c weakptr<class>. When the last reference to a
//! referenced
//! object goes out of scope, and the weak pointer becomes undefined, the
//! weak pointer gets automatically removed from the list.
//!
//! \c weaklist<Class>::base::iterator or
//! \c weaklistptr<Class>::base::iterator is an iterator over
//! the current contents of the weak list container. They iterate over
//! \ref weakptr "weakptr<class>", so getptr() must be used to recover a
//! strong reference.
//!
//! See \ref weakmap "INSERT_LIBX_NAMESPACE::weakmap" for a discussion of
//! locking and other aspects of weak container iterators. The same locking
//! properties apply to the weak list container, also.

template<typename T,
	 typename Allocator=std::allocator<weakptr<ptr<T> > > >
using weaklist=ref< weaklistObj< ptr<T>, Allocator>,
		    weaklistBase<T, Allocator> >;

//! A nullable pointer to a weak list

//! \see weaklist

template<typename T,
	 typename Allocator=std::allocator<weakptr<ptr<T> > > >
using weaklistptr=ptr< weaklistObj< ptr<T>, Allocator>,
		       weaklistBase<T, Allocator> >;

#if 0
{
#endif
}
#endif
