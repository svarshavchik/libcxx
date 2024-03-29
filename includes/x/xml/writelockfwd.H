/*
** Copyright 2020-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_writelockfwd_H
#define x_xml_writelockfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

class writelockObj;

//! A writer lock on an XML document

//! \code
//! auto doc=INSERT_LIBX_NAMESPACE::xml::doc::create("filename.xml");
//!
//! INSERT_LIBX_NAMESPACE::xml::writelock wlock=doc->create_writelock();
//! \endcode
//!
//! This is a reference-counted writer lock on the XML document.
//! Refer to this class as \c INSERT_LIBX_NAMESPACE::xml::writelock
//! Changing the contents of a document requires acquiring a writer
//! lock, which blocks all other reader and writer locks on the
//! document.
//!
//! A writer lock subclasses the \ref readlock "reader lock", and
//! implements all of its methods, with the exception of clone(), which
//! throws an exception. A writer lock cannot be cloned.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::xml::createnode
//!     creator=wlock->create_child();
//!
//! INSERT_LIBX_NAMESPACE::xml::createnode
//!     creator=wlock->create_next_sibling();
//!
//! INSERT_LIBX_NAMESPACE::xml::createnode
//!     creator=wlock->create_previous_sibling();
//! \endcode
//!
//! A writer lock has three methods that return factories which insert
//! new content into an XML document. The create_child() factory inserts
//! new child nodes. The create_next_sibling() factory inserts a
//! sibling node after the writer lock's current node. The
//! create_previous_sibling() factory inserts a sibling node before
//! the writer lock's current node. All three methods return a
//! \ref xml::createnode "INSERT_LIBX_NAMESPACE::doc::xml::createnode",
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
//! \ref xml::new_element "INSERT_LIBX_NAMESPACE::xml::new_element",
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
//! creator->text(40);
//!
//! INSERT_LIBX_NAMESPACE::number<int, tag_t> v;
//!
//! creator->text(v);
//! \endcode
//!
//! text() is overloaded for all integer numeric values (except `char`s)
//! and \ref number "type-safe numbers".
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
//! \ref xml::new_attribute "INSERT_LIBX_NAMESPACE::xml::new_attribute",
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
//! \ref xml::new_element "INSERT_LIBX_NAMESPACE::xml::new_element",
//! that can be given literally as a braced
//! initializer list.
//!
//! The second parameter is a \c std::vector that enumerates the
//! new element's attributes, as
//! \ref xml::new_attribute "INSERT_LIBX_NAMESPACE::xml::new_attribute"
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
//! creator->clone(rlock);
//! \endcode
//!
//! clone() inserts a part of another XML document. \c rlock is a
//! \ref readlock "read lock" on another XML document. The portion of
//! the XML document anchored at the read lock gets copied into this
//! XML document.
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
//! \ref xml::newdtd "INSERT_LIBX_NAMESPACE::xml::newdtd". Its referenced
//! object is a subclass of the object referenced by
//! \ref xml::dtd "INSERT_LIBX_NAMESPACE::xml::dtd", and implements all of
//! its methods; \ref xml::newdtd "INSERT_LIBX_NAMESPACE::xml::newdtd"'s
//! additional methods modify the XML document's DOCTYPE declaration.
//! The lesser used get_external_dtd() also returns a
//! \ref xml::newdtd "INSERT_LIBX_NAMESPACE::xml::newdtd" for the external
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

typedef ref<writelockObj> writelock;

//! A nullable pointer reference to a \ref writelock "writer lock".
typedef ptr<writelockObj> writelockptr;

//! A reference to a constant \ref writelock "writer lock".
typedef const_ref<writelockObj> const_writelock;

//! A nullable pointer reference to a constant \ref writelock "writer lock".

typedef const_ptr<writelockObj> const_writelockptr;

#if 0
{
#endif
}
#endif
