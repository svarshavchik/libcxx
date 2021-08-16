/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mappingnodefwd_H
#define x_mappingnodefwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

class mappingnodeBase;
class mappingnodeObj;

//! A reference to a parsed YAML mapping node object

//! \code
//! void dump(const INSERT_LIBX_NAMESPACE::yaml::mappingnode &map)
//! {
//!     size_t n=map->size();
//!     std::cout << n << " elements" << std::endl;
//!     for (size_t i=0; i<n; ++i)
//!     {
//!         std::pair<INSERT_LIBX_NAMESPACE::yaml::scalarnode, INSERT_LIBX_NAMESPACE::yaml::scalarnode> value=map->get(i);
//!         std::cout << value.first << "=" << value.second << std::endl;
//! }
//! \endcode
//!
//! This is a reference to a subclass of the
//! \ref node "YAML document node superclass"
//! that represents a mapping in a YAML document. Its
//! \ref mappingnodeObj::size "size"() method gives the number of values
//! in the mapping.
//! \ref mappingnodeObj::get "get"() returns a value in the mapping given as a
//! \c std::pair of a
//! \ref node "node"s. When the type of the node is known in advance, the
//! \ref scalarnode "INSERT_LIBX_NAMESPACE::yaml::scalarnode",
//! \ref sequencenode "INSERT_LIBX_NAMESPACE::yaml::sequencenode", or a
//! \ref mappingnode "INSERT_LIBX_NAMESPACE::yaml::mappingnode" can be used
//! directly for either the \c first value in the pair, the mapping's key, or
//! the \c second value in the pair, the mapping's value.
//!
//! \code
//! void dump(const INSERT_LIBX_NAMESPACE::yaml::mappingnode &map)
//! {
//!     for (std::pair<INSERT_LIBX_NAMESPACE::yaml::scalarnode, INSERT_LIBX_NAMESPACE::yaml::scalarnode> value: *map)
//!     {
//!         std::cout << value.first << "=" << value.second << std::endl;
//!     }
//! }
//! \endcode
//!
//! The \ref mappingnodeObj::begin "begin"() and
//! \ref mappingnodeObj::end "end"() methods return
//! \ref mappingnodeObj::iterator "iterator"s over the values in the mapping,
//! as a \c std::pair of
//! \ref node "node"s.
//! This example iterates over a mapping of a
//! \ref scalarnode "scalarnode" to another
//! \ref scalarnode "scalarnode", exclusively
//! so the iteration value gets coerced to a
//! \c std::pair of
//! \ref scalarnode "scalarnode"s.
//!
//! \code
//! void dump(const INSERT_LIBX_NAMESPACE::yaml::mappingnode &map)
//! {
//!     seq->for_each([]
//!         (INSERT_LIBX_NAMESPACE::yaml::scalarnode &&key, INSERT_LIBX_NAMESPACE::yaml::scalarnode &&value)
//!         {
//!             std::cout << value.first << "=" << value.second << std::endl;
//!         });
//! }
//! \endcode
//!
//! The \ref mappingnodeObj::for_each "for_each"() method produces slightly
//! more efficient code that iterates over the mapping, and invokes the
//! functor or the lambda argument, with the
//! mapping's key and value as separate rvalue arguments to its functor.
//!
//! \note
//! Mappings do not have to be mono-typed. The same mapping may have values,
//! or even keys, as
//! a mixture of
//! \ref scalarnode "scalarnode"s, or other
//! \ref sequencenode "sequencenode"s and
//! \ref mappingnode "mappingnode"s, for example.
//! In this case, the range iteration
//! or the 
//! \ref mappingnodeObj::for_each "for_each" functor should take
//! \ref node "INSERT_LIBX_NAMESPACE::yaml::node"s for the key and the value,
//! then proceed based on the key's and the value's
//! \c nodetype.

typedef ref<mappingnodeObj, mappingnodeBase> mappingnode;

//! A nullable pointer reference to a parsed YAML mapping node object

typedef ptr<mappingnodeObj, mappingnodeBase> mappingnodeptr;

//! A reference to a parsed constant YAML mapping node object

typedef const_ref<mappingnodeObj, mappingnodeBase> const_mappingnode;

//! A nullable pointer reference to a parsed constant YAML mapping node object

typedef const_ptr<mappingnodeObj, mappingnodeBase> const_mappingnodeptr;

#if 0
{
#endif
}
#endif
