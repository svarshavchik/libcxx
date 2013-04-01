/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_doc_H
#define x_xml_doc_H

#include <x/xml/docfwd.H>
#include <x/xml/docobj.H>
#include <x/xml/parser.H>
#include <x/ref.H>
#include <x/refiterator.H>

namespace LIBCXX_NAMESPACE {
	namespace xml {
#if 0
	};
};
#endif

//! Base class for \ref doc "XML documents".

//! Refer to this class as %INSERT_LIBX_NAMESPACE::xml::doc::base.

class docBase {

public:

	//! Create an empty XML document
	static doc create();

	//! Load an XML document from a file

	static doc create(//! Filename
			  const std::string &filename);

	//! Load an XML document from a file, with non-default options
	static doc create(//! Filename
			  const std::string &filename,

			  //! Non-default options, see libxml/parser.h
			  const std::string &options);

	//! Load an XML document from an input sequence defined by iterators.
	template<typename iter_type>
	static doc create(//! Beginning iterator for an input sequence that defines an XML document
			  iter_type beg_iter,
			  //! Ending iterator for an input sequence that defines an XML document
			  iter_type end_iter,

			  //! What to call this document in any error messages.
			  const std::string &context,

			  //! Non-default options, see libxml/parser.h
			  const std::string &options="")
	{
		return std::copy(beg_iter, end_iter,
				 parser::create(context, options))
			.get()->done();
	}

	//! Object factory, used by \ref ref "INSERT_LIBX_NAMESPACE::ref::create()".

	template<typename ref_type> class objfactory {
	public:

		//! Forward create() call to the base class.

		template<typename ...Args>
		static ref_type create(Args && ...args)
		{
			return docBase::create(std::forward<Args>(args)...);
		}
	};

	//! A read lock on an XML document

	//! \code
	//! auto doc=INSERT_LIBX_NAMESPACE::xml::doc::create("filename.xml");
	//!
	//! auto INSERT_LIBX_NAMESPACE::xml::doc::base::readlock rlock=doc->create_readlock();
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
	//! nodes, or other nodes.
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
	//! "element_node".
	//!
	//! path() returns the XPath to the current node, starting with the
	//! document's root.
	//!
	//! \code
	//! INSERT_LIBX_NAMESPACE::xml::doc::base::readlock lock2=rlock->clone();
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
	//! std::set<INSERT_LIBX_NAMESPACE::doc::base::attribute> attributes;
	//!
	//! rlock->get_all_attributes(attributes);
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
	//! get_all_attributes() enumerates the current element's node
	//! attributes, which are placed into a set of
	//! \ref docObj::docAttribute "INSERT_LIBX_NAMESPACE::xml::doc::base::attribute".
	//! This class contains two string members:
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

	typedef ref<docObj::readlockObj> readlock;

	//! A nullable pointer reference to a \ref readlock "reader lock".

	typedef ptr<docObj::readlockObj> readlockptr;

	//! A reference to a const \ref readlock "reader lock".

	typedef const_ref<docObj::readlockObj> const_readlock;

	//! A nullable pointer reference to a constant \ref readlock "reader lock".

	typedef const_ptr<docObj::readlockObj> const_readlockptr;

	//! A writer lock on an XML document

	//! \code
	//! auto doc=INSERT_LIBX_NAMESPACE::xml::doc::create("filename.xml");
	//!
	//! INSERT_LIBX_NAMESPACE::xml::doc::base::writelock wlock=doc->create_writelock();
	//! \endcode
	//!
	//! This is a reference-counted writer lock on the XML document.
	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::xml::doc::base::writelock
	//! Changing the contents of a document requires acquiring a writer
	//! lock, which blocks all other reader and writer locks on the
	//! document.
	//!
	//! A writer lock subclasses the \ref readlock "reader lock", and
	//! implements all of its methods, with the exception of clone(), which
	//! throws an exception. A writer lock cannot be cloned.

	typedef ref<docObj::writelockObj> writelock;

	//! A nullable pointer reference to a \ref writelock "writer lock".
	typedef ptr<docObj::writelockObj> writelockptr;

	//! A reference to a constant \ref writelock "writer lock".
	typedef const_ref<docObj::writelockObj> const_writelock;

	//! A nullable pointer reference to a constant \ref writelock "writer lock".

	typedef const_ptr<docObj::writelockObj> const_writelockptr;

	//! Convenience typedef for an element attribute

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::xml::doc::base::attribute;

	typedef docObj::docAttribute attribute;
};

#if 0
{
	{
#endif
	}
}
#endif
