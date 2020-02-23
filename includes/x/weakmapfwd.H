/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakmapfwd_H
#define x_weakmapfwd_H

#include <x/ref.H>
#include <x/ptr.H>
#include <x/weakptrfwd.H>
#include <map>
namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename K, typename T, typename M>
class weakmapObj;

template<typename K, typename T, typename C, typename Allocator>
class weakmapBase;

//! A map of weak references.

//! This template defines a reference-counted object that implements a subset
//! of functionality offered by std::map. Like std::map, any class may be used
//! as the map key, the map value must be a reference-counted object, derived
//! from \ref obj "INSERT_LIBX_NAMESPACE::obj".
//!
//! The weak map itself is a reference-counted object, so it must be created
//! first:
//!
//! \code
//! class myObject : virtual public obj {
//!
//! // ...
//! };
//!
//! typedef weakmap<std::string, myObject> myWeakmap;
//!
//! myWeakmap weakObjMap(myWeakMap::create());
//! \endcode
//!
//! Since this is a reference-counted object, assigning references does not
//! actually copy the map, but rather shuffles the reference pointers around;
//! also, the method are invoked by dereferencing the reference.
//!
//! Like a regular std::map, there's an insert() method that adds to the map:
//!
//! \code
//! ptr<myObject> obj(ptr<myObject>::create());
//!
//! bool flag=weakObjMap->insert(std::make_pair("foo", obj));
//! \endcode
//!
//! The key difference betweak the weak map container and std::map is that
//! the weak map container does not actually keep the
//! reference to the underlying object, but a weak reference.
//! See weakptr for more information on weak references. When the last
//! strong reference to the object goes out of scope, the weak reference gets
//! automatically removed from the weak map.
//!
//! Furthermore, the insert() method,
//! returns a single \c bool value. A value of \c true gets returned
//! if the weak reference was added to the map, \c false if the map already
//! contains a reference with the same key, or if the reference passed to
//! insert() is a null pointers.
//!
//! \par Weak map iterators and methods
//!
//! Like a regular std::map, the weak map class has an empty() and a size()
//! method. The following methods carry similar semantics than their
//! std::map counterparts: begin(), end(), find(), lower_bound(), upper_bound(),
//! and equal_range(), except that the iterators returned by these methods
//! refer to weak references:
//!
//! \code
//! myWeakMap::base::iterator b, e;
//!
//! b=weakObjMap->begin();
//! e=weakObjMap->end();
//!
//! while (b != e)
//! {
//!    ptr<myObject> c= b->second.getptr();
//!    if (!c.null())
//!    {
//!           // ...
//!    }
//!    ++b;
//! }
//! \endcode
//!
//! The value portion of the iterator key/value pair is a weak reference.
//! The weak map iterators are input iterators, they
//! cannot be used to modify the value portion, unlike with
//! regular std::map iterators, and implement only the \c ++ operator, since
//! they are input iterators. These input iterators can be used to iterate
//! over the map, but not to modify the contents of the map.
//! The only way to modify the contents of the
//! map is by either the insert() method, or
//! automatically when the last reference to an object goes out of scope and
//! the object's weak reference (together with its key) gets removed from the
//! map.
//!
//! \code
//! weakObjMap->find_or_create(keyValue, []
//!     {
//!         return ref<myObject>::create();
//!     });
//! \endcode
//!
//! find_or_create() is mostly usefull with weak multimaps. It's atomically
//! performs the following actions: checks every map value returned by
//! equal_range(), and returns the first non-null value in the map, otherwise
//! calls the lambda, inserts its return value into the map, with the given
//! key, then returns the newly created value.
//!
//! \par Locking
//!
//! The reference-counted map's methods are thread-safe. All existing iterators
//! acquire an internal lock, which also gets acquired, temporarily, by
//! methods like size() and empty().
//!
//! The outstanding locks do not prevent insert() from adding new entries into
//! the container, however while any lock is outstanding, weak references that
//! become null, after their referenced object gets destroyed, cannot be
//! removed from the container, because that may invalidate some existing
//! iterator.
//!
//! Instead, weak referenced to objects that go out of scope and get destroyed
//! get removed from the container after the last iterator itself goes out
//! of scope and releases lock.
//!
//! For that reason, iterators' own scopes should be limited, and they should
//! not remain in existence for too long.
//!
//! The weak map iterators refer to weak references, so, as in the example
//! above, it is necessary to invoke getptr() to
//! recover a strong reference to the underlying
//! object. It is also necessary to check if the recovered strong reference
//! is not null. As described, weak references get automatically removed from
//! the weak map only when no iterators are present. getptr() may come back
//! with a null pointer if an object went out of scope, and got destroyed,
//! while this iterator, or some other iterator, existed.
//! object is already destroyed by the other thread.
//!
//! Also note that, for example, if count() indicates that a map contains
//! a given key,
//! some other thread might've dropped the last reference to the object, and
//! it's removed from the weak map before find(), or one of the other methods,
//! has a chance to retrieve the corresponding iterator.

template<typename K, typename T,
	 typename C=std::less<K>,
	 typename Allocator=std::allocator<std::pair<const K, weakptr<ptr<T> > > > >
using weakmap=ref<weakmapObj<K, ptr<T>,
			     std::map<K, weakptr<ptr<T> >, C, Allocator>>,
		  weakmapBase<K, T, C, Allocator> >;

//! A nullable pointer reference to a reference-counted map weak container.

//! \see weakmap
template<typename K, typename T,
	 typename C=std::less<K>,
	 typename Allocator=std::allocator<std::pair<const K, weakptr<ptr<T> > > > >
using weakmapptr=ptr<weakmapObj<K, ptr<T>,
				std::map<K, weakptr<ptr<T> >, C, Allocator>>,
		     weakmapBase<K, T, C, Allocator> >;


#if 0
{
#endif
}
#endif
