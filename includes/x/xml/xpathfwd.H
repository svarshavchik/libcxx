/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_xpathfwd_H
#define x_xml_xpathfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

class xpathObj;

//! XPath API

//! This is a \ref ref "reference" to a reference-counted object that
//! represents the results of a reader or a writer lock's get_xpath()
//! method.
//!
//! \see readlock
//!
//! An \c xpath is returned by a reader or a writer lock's
//! get_xpath() method. The xpath-reference object holds an
//! internal reference on the lock that created it, which gets released
//! when the last reference on the xpath goes out of scope and it
//! gets destroyed.
//!
//! \code
//! size_t count=xpath->count();
//!
//! xpath->to_node(1);
//! \endcode
//!
//! The count() method returns the number of nodes in the result set,
//! which will be zero if the xpath expression evaluated to an empty
//! nodeset, these nodes did not exist.
//!
//! to_node() positions the reader or the writer lock, that created
//! the xpath object, to the given node in the nodeset.
//!
//! \note
//! The nodes are numbered starting with 1, not 0, to be consistent
//! with how nodes are numbered in XPath expressions. If the nodeset
//! has three nodes, the valid node numbers are 1, 2, and 3. Any other
//! node number results in to_node() throwing an exception.
//!
//! \code
//! xpath->to_node();
//! \endcode
//!
//! The overloaded to_node(), without a parameter, expects the nodeset
//! to contain exactly one node, which the reader or the writer
//! lock gets positioned to. An exception gets thrown if the nodeset
//! is empty or has more than one node.
//!
//! \code
//! rlock->get_xpath("body/h1")->to_node();
//! \endcode
//!
//! This is an example of the most
//! common situation where an XPath expression is expected
//! to evaluate to a single node. In this example the reader lock gets
//! repositioned to the desired element in the XML documen.
//!
//! \code
//! bool flag=xpath->as_bool();
//!
//! double number=xpath->as_number();
//!
//! std::string string=xpath->as_string();
//! \endcode
//!
//! as_bool(), as_number(), and as_string() return the value of the
//! XPath nodeset expressed as the corresponding data type.
//!
//! \note
//! When a nodeset gets created by a writer lock, calling the writer
//! lock's remove() automatically turns all xpath objects that were
//! created by the writer lock, and which still exist, into empty
//! nodesets (their count() methods will all return zero).

typedef ref<xpathObj> xpath;

//! A nullable pointer reference to an \ref xpath "get_xpath() result set".

typedef ptr<xpathObj> xpathptr;

//! A reference to a constant \ref xpath "get_xpath() result set".
typedef const_ref<xpathObj> const_xpath;

//! A nullable pointer reference to a constant \ref xpath "get_xpath() result set".
typedef const_ptr<xpathObj> const_xpathptr;

#if 0
{
#endif
}
#endif
