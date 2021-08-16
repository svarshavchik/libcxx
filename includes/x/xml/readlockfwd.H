/*
** Copyright 2020-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_readlockfwd_H
#define x_xml_readlockfwd_H

#include <x/xml/attribute.H>
#include <x/ptrfwd.H>
#include <unordered_set>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

class readlockObj;

//! Container populated by get_all_attributes

//! \see readlock

typedef std::unordered_set<attribute> get_all_attributes_t;

//! A read lock on an XML document

//! \code
//! auto doc=INSERT_LIBX_NAMESPACE::xml::doc::create("filename.xml");
//!
//! auto INSERT_LIBX_NAMESPACE::xml::readlock rlock=doc->create_readlock();
//! \endcode
//!
//! This is a reference-counted reader lock on the XML document.
//! Examining the contents of a document requires acquiring a reader
//! lock, which blocks the document from being changed, as long as
//! a reader lock remains in scope.
//!
//! The lock objects' methods are thread-safe with respect to other
//! locks on the same document, but a single reader or a writer lock
//! can be used by only one thread at a time.
//!
//! \code
//!
//! bool flag;
//!
//! flag=rlock->get_root();
//!
//! flag=rlock->get_parent();
//!
//! flag=rlock->get_first_child();
//!
//! flag=rlock->get_last_child();
//!
//! flag=rlock->get_next_sibling();
//!
//! flag=rlock->get_previous_sibling();
//!
//! flag=rlock->get_first_element_child();
//!
//! flag=rlock->get_last_element_child();
//!
//! flag=rlock->get_next_element_sibling();
//!
//! flag=rlock->get_previous_element_sibling();
//!
//! size_t n=rlock->get_child_element_count();
//! \endcode
//!
//! A reader lock is associated with a single node in the XML document.
//!
//! get_root() positions the reader lock to the XML document's root
//! node.
//! get_first_child() and get_last_child() position the reader
//! lock to its current node's first or last child node in the XML
//! document. get_next_sibling() and get_previous_sibling() position
//! the node to its next or the previous sibling node.
//!
//! get_first_element_child(), get_last_element_child(),
//! get_next_element_sibling() and get_previous_element_sibling()
//! are similar, except that they reposition the reader lock to
//! the corresponding XML element node only, skipping over any text
//! nodes, or other nodes. get_child_element_count() return the count
//! of child elements.
//!
//! get_parent() positions the reader lock to the parent node.
//!
//! All these functions return \c true if the reader lock gets
//! repositioned accordingly, or \c false if no such node exists
//! (no first child or last child, because the element has no child
//! nodes; or its the first or the last sibling node, etc...).
//!
//! \code
//! std::string type=rlock->type();
//!
//! std::string name=rlock->name();
//!
//! std::string prefix=rlock->prefix();
//!
//! std::string uri=rlock->uri();
//!
//! std::string path=rlock->path();
//! \endcode
//!
//! type() returns the type of the current node, as a string.
//! "element_node" gets returned when the current node is an XML
//! element node, "text_node" gets returned when the current node is
//! a text node. The string corresponds directly to the enumerated
//! \c xmlElementType values of the underlyling \c libxml library:
//! \c XML_ELEMENT_NODE, \c XML_TEXT_NODE, and so on.
//!
//! An empty string gets returned if the reader lock is not positioned
//! on a node.
//!
//! name() returns the name of the current node when it is an
//! "element_node". prefix() and uri() return the element node's
//! namespace prefix and URI, or an empty string if the element node
//! is not in any namespace.
//!
//! path() returns the XPath to the current node, starting with the
//! document's root.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::xml::readlock lock2=rlock->clone();
//! \endcode
//!
//! clone() clones the reader lock, creating another read lock, that's
//! separately positionable. \ref readlock "readlock" is a reference-
//! counted object, and clone() creates another instance.
//! Different reader locks may reference
//! different positions in the same XML document. A thread must use
//! clone() instead of explicitly calling readlock() again, because
//! the second reader clock can get deadlocked by a waiting
//! \ref writelock "writer lock" that's waiting for the first
//! reader lock to go out of scope and release its lock on the
//! document.
//!
//! \code
//! std::string value;
//!
//! value=rlock->get_attribute("style");
//!
//! INSERT_LIBX_NAMESPACE::uriimpl ns;
//!
//! value=rlock->get_attribute("style", ns);
//!
//! value=rlock->get_any_attribute("style");
//!
//! &ns;::xml::get_all_attributes_t attributes=rlock->get_all_attributes(attributes);
//! \endcode
//!
//! get_attribute() returns the value of the given attribute in the
//! null XML namespace, if the read lock is positioned on an element
//! node, and the given un-namespaced attribute exists. An empty
//! string gets returned in all other situations.
//!
//! A second parameter to get_attribute() specifies an XML namespace
//! for the attribute's name, and returns the value of the attribute
//! in the referenced namespace. The namespace can be specified either
//! as \ref uriimpl "INSERT_LIBX_NAMESPACE::uriimpl" or a literal
//! string.
//!
//! get_any_attribute() retrieves the value of the specified attribute
//! in any namespace.
//!
//! get_all_attributes() enumerates the current element's
//! attributes, and returns a container of
//! \ref xml::attribute "INSERT_LIBX_NAMESPACE::xml::attribute"s.
//! The  attribute class contains two string members:
//!
//! - attrname: the attribute's name.
//!
//! - attrnamespace: the attribute's namespace. An empty string
//! specifies an attribute that's not in any namespace.
//!
//! \code
//! std::string text=rlock->get_text();
//!
//! std::string lang=rlock->get_lang();
//!
//! bool flag=rlock->is_text();
//!
//! bool flag=rlock->is_blank();
//!
//! int flag=rlock->get_space_preserve();
//!
//! std::string uri=rlock->get_base();
//! \endcode
//!
//! get_text() returns the text representation of the current node.
//! get_lang() returns the current's node declared language (the
//! \c xml:lang attribute, if it's defined, an empty string otherwise).
//! is_text() returns true if the current node is a text node.
//! is_blank() returns true if the current node is a text node, and
//! it is empty.
//!
//! get_space_preserve() returns 1 if \c xml:space specifies
//! 'preserve', 0 if it's 'default', and negative if \c xml:space
//! is not declared for the current node.
//!
//! get_base() returns the node's base URI, as defined in RFC 2396
//! section 5.1.1.
//!
//! \code
//! rlock->save_file("filename.xml");
//!
//! std::string s;
//!
//! rlock->save_to(std::back_insert_iterator<std::string>(s));
//! \endcode
//!
//! save_file() saves the XML document into a file.
//! save_to() writes the XML document into an output iterator.
//! save_to() returns the new value of the output iterator.
//!
//! Both save_file() and save_to() take an optional second \c bool
//! parameter. Setting it to \c true formats the XML document with
//! whitespace indentation.
//!
//! \note
//! save_file() writes the XML document into "filename.tmp", and
//! renames it to the given "filename".
//!
//! \code
//! INSERT_LIBX_NAMESPACE::xml::dtd dtd=rlock->get_internal_dtd();
//! \endcode
//!
//! get_internal_dtd() returns the XML document's \c DOCTYPE declaration
//! object. An object gets returned even if the XML document does not
//! have a \c DOCTYPE declaration, in which case its exists() method
//! returns \c false.
//! There's also a
//! get_external_dtd(), which retuns the XML document's "external"
//! declaration. This is not normally used; its primary purpose is to
//! validate an XML document's DTD.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::xml::xpath xpath=rlock->get_xpath("body/p[1]");
//! \endcode
//!
//! \ref xml::readlockObj::get_xpath "get_xpath"() evaluates
//! an XPath expression with respect to the document node that
//! the reader lock is currently positioned to. Returns a
//! reference-counted that \ref xml::xpath "represents the results" of the
//! XPath-derived nodeset.

typedef ref<readlockObj> readlock;

//! A nullable pointer reference to a \ref readlock "reader lock".

typedef ptr<readlockObj> readlockptr;

//! A reference to a const \ref readlock "reader lock".

typedef const_ref<readlockObj> const_readlock;

//! A nullable pointer reference to a constant \ref readlock "reader lock".

typedef const_ptr<readlockObj> const_readlockptr;


#if 0
{
#endif
}
#endif
