/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sequencenodefwd_H
#define x_sequencenodefwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

	namespace yaml {
#if 0
	}
}
#endif

class sequencenodeBase;
class sequencenodeObj;

//! A reference to a parsed YAML sequence node object

//! \code
//! void dump(const INSERT_LIBX_NAMESPACE::yaml::sequencenode &seq)
//! {
//!     size_t n=seq->size();
//!     std::cout << n << " elements" << std::endl;
//!     for (size_t i=0; i<n; ++i)
//!         dump(INSERT_LIBX_NAMESPACE::yaml::scalarnode(seq->get(i)));
//! }
//! \endcode
//!
//! This is a reference to a subclass of the
//! \ref node "YAML document node superclass"
//! that represents a sequence in a YAML document. Its
//! \ref sequencenodeObj::size "size"() method gives the number of values
//! in the sequence.
//! \ref sequencenodeObj::get "get"() returns a
//! \ref node "node" reference to the given value, which can be converted
//! to a
//! \ref scalarnode "INSERT_LIBX_NAMESPACE::yaml::scalarnode",
//! \ref sequencenode "INSERT_LIBX_NAMESPACE::yaml::sequencenode", or a
//! \ref mappingnode "INSERT_LIBX_NAMESPACE::yaml::mappingnode".
//!
//! \code
//! void dump(const INSERT_LIBX_NAMESPACE::yaml::sequencenode &seq)
//! {
//!     for (INSERT_LIBX_NAMESPACE::yaml::scalarnode scalar: *seq)
//!     {
//!         dump(scalar);
//!     }
//! }
//! \endcode
//!
//! The \ref sequencenodeObj::begin "begin"() and
//! \ref sequencenodeObj::end "end"() methods return
//! \ref sequencenodeObj::iterator "iterator"s over \ref node "node" values
//! in the sequence. This example iterates over a sequence that contains
//! only
//! \ref scalarnode "scalarnode"s, so the iteration value gets coerced to a
//! \ref scalarnode "scalarnode", for convenience.
//!
//! \code
//! void dump(const INSERT_LIBX_NAMESPACE::yaml::sequencenode &seq)
//! {
//!     seq->for_each([]
//!         (LIBCXX_NAMESPACE::yaml::scalarnode &&n)
//!         {
//!             dump(n);
//!         });
//! }
//! \endcode
//!
//! The \ref sequencenodeObj::for_each "for_each"() method produces slightly
//! more efficient code that iterates over the sequence, and invokes the
//! functor or the lambda argument, passing each node in the sequence by rvalue.
//!
//! \note
//! Sequences do not have to be mono-typed. The same sequence may contain
//! a mixture of
//! \ref scalarnode "scalarnode"s and \ref sequencenode "sequencenode"s, for
//! example. In this case, the range iteration or the 
//! The \ref sequencenodeObj::for_each "for_each" functor should take a
//! \ref node "INSERT_LIBX_NAMESPACE::yaml::node" rvalue, then proceed based on
//! its \c nodetype.

typedef ref<sequencenodeObj, sequencenodeBase> sequencenode;

//! A nullable pointer reference to a parser YAML sequence node object

typedef ptr<sequencenodeObj, sequencenodeBase> sequencenodeptr;

//! A reference to a parsed constant YAML sequence node object

typedef const_ref<sequencenodeObj, sequencenodeBase> const_sequencenode;

//! A nullable pointer reference to a parsed constant YAML sequence node object

typedef const_ptr<sequencenodeObj, sequencenodeBase> const_sequencenodeptr;

#if 0
{
	{
#endif
	}
}
#endif
