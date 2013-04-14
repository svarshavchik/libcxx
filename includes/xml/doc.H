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
	//! INSERT_LIBX_NAMESPACE::xml::doc::base::xpath xpath=rlock->get_xpath("body/p[1]");
	//! \endcode
	//!
	//! \ref docObj::readlockObj::get_xpath "get_xpath"() evaluates
	//! an XPath expression with respect to the document node that
	//! the reader lock is currently positioned to. Returns a
	//! reference-counted that \ref xpath "represents the results" of the
	//! XPath-derived nodeset.

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
	//! \code
	//! creator->attribute({"style", "white-space: nowrap"});
	//! wlock->attribute({"style", "white-space: nowrap"});
	//! \endcode
	//!
	//! The attribute() method may be called either directly on a writer
	//! lock, or one of the creators, in which case it, like other
	//! create methods, returns the same creator reference, for convenience:
	//!
	//! \code
	//! creator->attribute({"style", "white-space: nowrap"})
	//!        ->attribute({"class", "address"});
	//! \endcode
	//!
	//! The writer lock must be positioned
	//! on an element node in the document; attribute() adds a new
	//! attribute to the element.
	//!
	//! The parameter to attribute() is an internal helper class,
	//! \ref docObj::newAttribute "INSERT_LIBX_NAMESPACE::xml::doc::base::newattribute",
	//! which has several different constructors, and gets typically given
	//! literally using a braced initializer list, for convenience.
	//! The first example's initializer is a single string, which
	//! initializes a new attribute named "style", in an unnamed namespace
	//! with the value of "white-space: nowrap".
	//!
	//! \code
	//! wlock->attribute({"html:style", "white-space: nowrap"});
	//! \endcode
	//!
	//! This version of attribute()'s initializer adds a new attribute in a
	//! namespace
	//! with the given prefix that must be open in its element, or one
	//! of its parent elements.
	//!
	//! \code
	//! wlock->attribute({"style", "http://www.w3.org/1999/xhtml", "white-space: nowrap"});
	//! \endcode
	//!
	//! This version of attribute()'s initializer adds a new attribute in a
	//! namespace
	//! with the given URI, that must be open in its element, or one
	//! of its parent elements.
	//! The URI can be given as a literal string,
	//! or as an \ref uriimpl "INSERT_LIBX_NAMESPACE::uriimpl".
	//!
	//! \code
	//! creator->element({"html:div"},
	//!                  {
	//!                      {"style", "white-space: nowrap"},
	//!                      {"id", "box"}
	//!                  });
	//! \endcode
	//!
	//! The two parameter version of element() creates a new element,
	//! and all of its
	//! attributes, in one go. The first parameter is still the same
	//! internal helper class,
	//! \ref docObj::newElement "INSERT_LIBX_NAMESPACE::xml::doc::base::newelement",
	//! that can be given literally as a braced
	//! initializer list.
	//!
	//! The second parameter is a \c std::vector that enumerates the
	//! new element's attributes, as
	//! \ref docObj::newAttribute "INSERT_LIBX_NAMESPACE::xml::doc::base::newattribute"
	//! helper class instances. This also can be expressed as a braced
	//! initializer list. The above example creates an XML element node
	//! "div", in the "html" namespace, with two attributes, "style" and
	//! "id".
	//!
	//! The two parameter version of element() is equivalent to calling
	//! the one parameter version, passing it the first parameter, then
	//! calling attribute() repeatedly, passing it each element in the
	//! second parameter's vector.
	//!
	//! \code
	//! wlock->remove();
	//! \endcode
	//!
	//! remove() removes the current node that the writer lock is positioned
	//! on. The wrier lock gets repositioned to the removed node's parent
	//! node. If the removed node is the document root node, this becomes
	//! an empty document, and the writer lock is no longer positioned on
	//! any element.
	//!
	//! \code
	//! wlock->set_base("http://www.example.com");
	//! wlock->set_lang("EN");
	//! wlock->set_space_preserve(true);
	//! \endcode
	//!
	//! These function set the \c xml:base, \c xml:lang, and
	//! \c xml:space_preserve attributes of the current element node that
	//! the writer lock is positioned to. An exception gets thrown if the
	//! writer lock is not position on an element node.
	//!
	//! \code
	//! creator->comment("Optional element");
	//!
	//! creator->processing_instruction("man2html", "style=visible");
	//! \endcode
	//!
	//! comment() and processing_instruction() adds a new XML comment
	//! or a processing instruction.
	//!
	//! A reader or a writer lock that's positioned on a comment node
	//! results in type() returning "comment_node" and get_text() returning
	//! the contents of the comment.
	//!
	//! A reader or a writer lock that's positioned on a processing
	//! instruction node
	//! results in type() returning "pi_node", name() returning the
	//! processing instruction name, and get_text() returning
	//! the contents of the processing instruction.
	//!
	//! \code
	//! wlock->create_child()->element({"p"})
	//!      ->create_next_sibling()->element({"p"})
	//!      ->parent()->element({"p"});
	//! \endcode
	//!
	//! For convenience, each creator factory also implements its origin
	//! writer lock's create_child(), create_next_sibling() and
	//! create_previous_sibling() methods, that return the other types
	//! of creator factories. This allows a single statement to create
	//! new XML nodes in different "directions". The above example uses
	//! the create_child() factory to install a new \c <p> child element
	//! of the parent node, then switch to the
	//! create_next_sibling() creator factory and installs another
	//! \c <p> element as its sibling. The net effect is the creation of
	//! two \c <p> elements from the parent node.
	//!
	//! For convenience, each creator factory also implements parent()
	//! by repositioning the writer lock to its current node's parent node.
	//! After creating the two new \c <p> child elements of of the parent
	//! node, the writer lock is positioned on the second child element.
	//! parent() repositions the writer lock to the original parent node.
	//! The creator factory remains unchanged, it's still
	//! create_next_sibling(), and a third call to element() creates
	//! a sibling element of the original parent node.
	//!
	//! \code
	//! INSERT_LIBX_NAMESPACE::xml::newdtd dtd=
	//!    wlock->create_internal_dtd("-//W3C//DTD XHTML 1.0 Strict//EN",
	//!        "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd");
	//! \endcode
	//!
	//! create_internal_dtd() declaration an XML document's \c DOCTYPE
	//! definition. The XML document must be non empty, and the DOCTYPE's
	//! name gets automatically set to the XML document's root element.
	//!
	//! There's also a create_external_dtd(), which is rarely used; mostly
	//! when validating a documents DTD.
	//!
	//! \code
	//! INSERT_LIBX_NAMESPACE::xml::newdtd dtd=wlock->get_internal_dtd();
	//! \endcode
	//!
	//! The writer lock implements get_internal_dtd() like a reader clock,
	//! except that the writer lock returns a
	//! \ref newdtd "INSERT_LIBX_NAMESPACE::xml::newdtd". Its referenced
	//! object is a subclass of the object referenced by
	//! \ref dtd "INSERT_LIBX_NAMESPACE::xml::dtd", and implements all of
	//! its methods; \ref newdtd "INSERT_LIBX_NAMESPACE::xml::newdtd"'s
	//! additional methods modify the XML document's DOCTYPE declaration.
	//! The lesser used get_external_dtd() also returns a
	//! \ref newdtd "INSERT_LIBX_NAMESPACE::xml::newdtd" for the external
	//! subset.
	//!
	//! \code
	//! wlock->remove_internal_dtd();
	//! \endcode
	//!
	//! remove_internal_dtd() (and remove_external_dtd()) removes any
	//! existing \c DOCTYPE. create_internal_dtd() throws an exception
	//! if the XML document already has a declared \c DOCTYPE.
	//!
	//! The following methods add entity declarations to a document
	//! type definition. They are typically used with an internal 
	//! DOCTYPE, when creating, then saving, an XML document.
	//!
	//! \code
	//! newdtd->create_general_entity("appname", "<application>LibXML</application>");
	//! \endcode
	//!
	//! create_general_entity() adds a general entity declaration. This
	//! example adds "<!ENTITY appname '<application>LibXML</application>'>".
	//!
	//! \code
	//! newdtd->create_parsed_entity("ch1", "", "chapter1.xml");
	//! \endcode
	//!
	//! create_parsed_entity() adds a declaration for an external
	//! parsed entity. The second parameter is a public identifier,
	//! which is normally an empty string. This example adds
	//! "<!ENTITY ch1 SYSTEM 'chapter1.xml'>".
	//!
	//! \code
	//! newdtd->create_unparsed_entity("table1", "", "table1.jpg", "jpg");
	//! \endcode
	//!
	//! create_unparsed_entity() adds a declaration for an external
	//! unparsed entity. The second parameter is a public identifier,
	//! which is normally an empty string. This example adds
	//! "<!ENTITY table1 SYSTEM 'table1.jpg' NDATA jpg>".
	//!
	//! \code
	//! std::ostringstream o;
	//!
	//! for (int i=1; i<10; ++i)
	//! {
	//!     o << "<!ENTITY ch" << i << " SYSTEM \"ch" << i << ".xml\">";
	//! }
	//! newdtd->create_internal_parameter_entity("chapters", o.str());
	//! \endcode
	//!
	//! create_internal_parameter_entity() adds a declaration for an
	//! internal parameter entity. This example adds
	//! "<!ENTITY % chapters '[...]'>" (with a long, messy
	//! string instead of the ellipsis) to the document.
	//!
	//! \code
	//! newdtd->create_external_parameter_entity("chapters", "", "chapters.xml");
	//! \endcode
	//!
	//! create_external_parameter_entity() adds a declaration for an
	//! external parameter entity. The second parameter is a public
	//! identifier, which is normally an empty string. This example adds
	//! "<!ENTITY % chapters SYSTEM 'chapters.xml'>" to the document.
	//!
	//! \code
	//! newdtd->include_parameter_entity("chapters");
	//! \endcode
	//!
	//! This example adds "%chapters;" to the document.

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

	//! This is passed as a parameter to \ref writelock "attribute()", to
	//! add a new attribute to the current element node.
	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::xml::doc::base::newattribute.
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

	//! XPath API

	//! This is a \ref ref "reference" to a reference-counted object that
	//! represents the results of a reader or a writer lock's get_xpath()
	//! method. Refer to this reference as
	//! \c INSERT_LIBX_NAMESPACE::xml::doc::base::xpath.
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

	typedef ref<docObj::xpathObj> xpath;

	//! A nullable pointer reference to an \ref xpath "get_xpath() result set".

	typedef ptr<docObj::xpathObj> xpathptr;

	//! A reference to a constant \ref xpath "get_xpath() result set".
	typedef const_ref<docObj::xpathObj> const_xpath;

	//! A nullable pointer reference to a constant \ref xpath "get_xpath() result set".
	typedef const_ptr<docObj::xpathObj> const_xpathptr;
};

#if 0
{
	{
#endif
	}
}
#endif
