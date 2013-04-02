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
	//!
	//! \code
	//! INSERT_LIBX_NAMESPACE::xml::doc::base::createnode
	//!     creator=wlock->create_child();
	//!
	//! INSERT_LIBX_NAMESPACE::xml::doc::base::createnode
	//!     creator=wlock->create_next_sibling();
	//!
	//! INSERT_LIBX_NAMESPACE::xml::doc::base::createnode
	//!     creator=wlock->create_previous_sibling();
	//! \endcode
	//!
	//! A writer lock has three methods that return factories which insert
	//! new content into an XML document. The create_child() factory inserts
	//! new child nodes. The create_next_sibling() factory inserts a
	//! sibling node after the writer lock's current node. The
	//! create_previous_sibling() factory inserts a sibling node before
	//! the writer lock's current node. All three methods return a
	//! \ref createnode "INSERT_LIBX_NAMESPACE::doc::xml::createnode",
	//! which is a reference to a reference-counted object.
	//!
	//! \note
	//! The factory holds a reference on the writer lock, until it
	//! goes out of scope and gets destroyed.
	//!
	//! A newly created writer lock, from create_writelock() is not
	//! positioned on any node. It must be positioned before invoking
	//! one of the factory's methods that insert new content into the
	//! XML document, with the exception of create_child(). Invoking
	//! create_child() on a new writer lock that's not positioned on any
	//! node in the document returns a factory whose
	//! element() method sets the document's root node.
	//!
	//! Like reader locks, writer locks may be used only by one thread, at
	//! a time.
	//!
	//! \code
	//! creator->element({"p"})->element({"span"});
	//! creator->text("rosebud");
	//! creator->cdata("<text>");
	//! \endcode
	//!
	//! A creator's element() method creates a new XML element, and
	//! adds to the XML document. A creator obtained from
	//! create_child() adds the new XML element as a child node of the
	//! writer lock's current node. Similarly, a creator obtained from
	//! create_next_sibling() or create_previous_sibling() inserts the
	//! new XML element as the writer lock's current node's next or
	//! previous sibling().
	//!
	//! Creator's method return the same creator reference, for convenience,
	//! allowing for creation of multiple elements in one statement.
	//! After adding a new element to the XML document, the writer lock
	//! gets positioned on the newly-added element.
	//!
	//! In the example above, if the creator was obtained from
	//! create_child(), the example creates a \c \<p>
	//! child element, positions the writer lock to it, then the second
	//! call to element() creates a \c \<span>
	//! child element of the \c \<p>.
	//!
	//! The parameter to element() is an internal helper class,
	//! \ref docObj::newElement "INSERT_LIBX_NAMESPACE::xml::doc::base::newelement",
	//! which has several different constructors, and gets typically given
	//! literally using a braced initializer list, for convenience.
	//! The above example's initializer is a single string, which
	//! initializes a new element, in an unnamed namespace.
	//!
	//! If the creator was obtained from create_next_sibling() instead,
	//! this example would create two sibling elements,
	//! \c \<p> and \c \<span>,
	//! of the writer lock's current node.
	//!
	//! text() and cdata() create new text and \c \<CDATA>
	//! elements in the XML document, and insert them accordingly.
	//!
	//! \code
	//! creator->create_namespace("prefix", "http://www.example.com");
	//! wlock->create_namespace("prefix", "http://www.example.com");
	//! \endcode
	//!
	//! create_namespace() opens a new namespace in the XML element node
	//! the writer locked is positioned on. The writer lock must be
	//! positioned to an existing element node.
	//! create_namespace() can be called either as a method on a writer
	//! lock or as a creator's method, in which case it, like other
	//! creator methods, returns the same creator reference, for convenience:
	//!
	//! \code
	//! creator->create_namespace("ns1", "http://www.example.com");
	//!      ->create_namespace("ns2", "http://www.example.com/ns2");
	//! \endcode
	//!
	//! The first parameter is
	//! a namespace prefix, the second parameter is the namespace \c URI
	//! which may be given as a string, or as an
	//! \ref uriimpl "INSERT_LIBX_NAMESPACE::uriimpl".
	//!
	//! The new namespace may be referenced by the node's children elements
	//! using its prefix or \c URI.
	//!
	//! \code
	//! creator->element({"item", "prefix", "http://www.example.com/items"});
	//! \endcode
	//!
	//! This version of element()'s initializer adds a new XML element
	//! in a new namespace.
	//! This example creates a '<prefix:item xmlns:prefix="http://www.example.com/items"/>'
	//! element, with "prefix".
	//!
	//! \code
	//! creator->element({"prefix:item"});
	//! \endcode
	//!
	//! This version of element()'s initializer adds a new XML element in a
	//! namespace
	//! with the given prefix. A namespace with this prefix
	//! must be open in one of the new element's parent elements.
	//!
	//! \code
	//! creator->element({"item", "http://www.example.com"});
	//! \endcode
	//!
	//! This version of element()'s initializer adds a new XML element in a namespace
	//! with the given \c URI that must be open in one of the new element's
	//! parent elements.
	//! The URI can be given as a literal string,
	//! or as an \ref uriimpl "INSERT_LIBX_NAMESPACE::uriimpl".
	//!


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

	//! Convenience typedef for a new element initialization parameters

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::xml::doc::base::newelement
	//! \see writelock
	typedef docObj::newElement newelement;

	//! Convenience typedef for a new element attribute

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::xml::doc::base::newattribute;
	typedef docObj::newAttribute newattribute;

	//! Modifying XML documents

	//! Refer to this class a \c INSERT_LIBX_NAMESPACE::doc::base::createnode.
	//! This is a \ref ref "reference" to a reference-counted object that
	//! gets returned by \ref writelock "INSERT_LIBX_NAMESPACE::doc::base::writelock"'s
	//! create_child(), create_next_sibling(), and
	//! create_previous_sibling(), and implements creation of new XML
	//! document elements that get inserted accordingly.

	typedef ref<docObj::createnodeObj> createnode;

	//! A nullable pointer reference to a \ref createnode "createnode".

	typedef ptr<docObj::createnodeObj> createnodeptr;
};

#if 0
{
	{
#endif
	}
}
#endif
