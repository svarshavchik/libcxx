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
	//! \code
	//! rlock->to_root();
	//!
	//! INSERT_LIBX_NAMESPACE::xml::doc::base::readlock lock2=rlock->clone();
	//!
	//! std::string type=rlock->type();
	//! \endcode
	//!
	//! A reader lock is associated with a single node in the XML document.
	//! to_root() positions the reader lock to the XML document's root
	//! node.
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
	//! The lock objects' methods are thread-safe with respect to other
	//! locks on the same document, but a single reader or a writer lock
	//! can be used by only one thread at a time.

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
};

#if 0
{
	{
#endif
	}
}
#endif
