/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_hierfwd_H
#define x_hierfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename hier_type, typename ref_type> class hierBase;
template<typename hier_type, typename node_type> class hierObj;

//! A hierarchical container

//! This is an associative container, with limited functionality, that
//! stores values with a hierarchical key.
//!
//! \code
//! typedef INSERT_LIBX_NAMESPACE::hier<std::string, INSERT_LIBX_NAMESPACE::ref<valueObj> > tree;
//!
//! auto t=tree::create();
//!
//! std::list<std::string> key;
//!
//! key.insert("foo");
//! key.insert("bar");
//!
//! bool flag=t->insert(key, []
//!     {
//!         return INSERT_LIBX_NAMESPACE::ref<valueObj>::create();
//!     }, false);
//!
//! \endcode
//!
//! The first parameter to \ref hier "INSERT_LIBX_NAMESPACE::hier" is a key
//! type. The second parameter must be a
//! a \ref ref "INSERT_LIBX_NAMESPACE::ref" to a reference-counted object.
//! This defines the value stored in the container.
//!
//! The values in the container are keyed by a \c std::list<key>, and this
//! example keys the values in the container by a \c std::list<string>.
//!
//! The keys form a hierarchical relationship. In this example, the top level
//! key is called "foo", with the child node "bar", whose value gets set.
//! This example defines the value of "bar". Setting a value automatically
//! creates the parent "foo" node, if one does not exist already because it
//! has some other, existing child node, or if itself has a value.
//!
//! Individual nodes in the hierarchy may or may not have a value. Inserting
//! a node into a hierarchy automatically creates any intermediate nodes in the
//! hierarchy that do not exist already, the intermediate nodes will have no
//! values, unless they're subsequently set, themselves.
//!
//! Remove the last child node, with a value, from a parent without its own
//! value (or other child node with a value), automatically remoevs the
//! parent node (this is done recursively).
//!
//! A hierarchy container always contains the root node, the parent node
//! of all the top level nodes.
//!
//! create_readlock() and create_writelock() instantiate a reader or a writer
//! lock on the hierarchy container. Each instance of a reader or a writer
//! lock gets associated with some existing node in the hierarchy (which may
//! or may not have a set value). Initially the lock points to the root node.
//! Its methods repoint the lock to other parts of the hierarchy, and the
//! writer lock has additional methods that insert or erase the value of the
//! current node, or one of its children (creating and removing any intermediate
//! parts of the hierarchy, as needed.
//!
//! Additionally, the container object provides insert(), erase(), and
//! search() methods, that instantiate the appropriate lock object, locate
//! the specified node in the hierarchy, and perform the requested operation.
//!
//! search() locates the specified node in the hierarchy. If it does not exist
//! or it's an intermediate node with no value, its closest parent with a value
//! gets returned instead.
//!
//! insert(), erase(), and search() are just convenience functions that
//! invoke create_readlock() or create_writelock(), then invoke the returned
//! lock object's methods, before discarding it.
//!
//! \code
//! tree::base::iterator b=t->begin(), e=t->end();
//! 
//! for (auto iter: *t)
//! {
//!     std::list<std::string> key=iter;
//! }
//! \endcode
//!
//! begin() and end() return a beginning and an ending iterator for an input
//! sequence of keys in the container.
//!
//! \par Locking
//!
//! Operations on the container object are thread safe. Individual lock objects
//! are not thread safe themselves; but different lock objects may be used
//! by different threads, independently.

template<typename hier_type, typename ref_type>
using hier=ref<hierObj<hier_type, ref_type>, hierBase<hier_type, ref_type> >;

//! A nullable pointer reference to a hierarchical container.

//! \see hier
template<typename hier_type, typename ref_type>
using hierptr=ptr<hierObj<hier_type, ref_type>, hierBase<hier_type, ref_type> >;

//! A reference to a constant hierarchical container

//! \see hier
template<typename hier_type, typename ref_type>
using const_hier=const_ref<hierObj<hier_type, ref_type>,
			   hierBase<hier_type, ref_type> >;

//! A nullable pointer reference to a constant hierarchical container.

//! \see hier
template<typename hier_type, typename ref_type>
using const_hierptr=const_ptr<hierObj<hier_type, ref_type>,
			      hierBase<hier_type, ref_type> >;

#if 0
{
#endif
}
#endif
