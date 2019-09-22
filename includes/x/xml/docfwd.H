/*
** Copyright 2013-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_docfwd_H
#define x_xml_docfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {
	namespace xml {
#if 0
	};
};
#endif

class docObj;
class docBase;

//! A reference-counted XML document handle

//! This is a \ref ref "reference" to an XML document that's parsed into a DOM tree.
//!
//! \code
//! auto empty_document=INSERT_LIBX_NAMESPACE::xml::doc::create();
//! \endcode
//!
//! Creates an empty document.
//!
//! \code
//! auto loaded_document=INSERT_LIBX_NAMESPACE::xml::doc::create("filename.xml");
//! \endcode
//!
//! Loads a document from a file.
//
//! An %exception gets thrown if an error occurs when parsing the document.
//!
//! \code
//! auto loaded_document=INSERT_LIBX_NAMESPACE::xml::doc::create("filename.xml", "nonet");
//! \endcode
//!
//! The second optional argument is a comma or a whitespace delimited list of
//! parsing options. This is the list of \c XML_PARSE_ options listed in
//! \c libxml/parser.h. \c XML_PARSE_XINCLUDE gets specified as "xinclude",
//! \c XML_PARSE_NONET as "nonet", and so on.
//!
//! \code
//! auto parsed_document=INSERT_LIBX_NAMESPACE::xml::doc::create(container.begin(), container.end(), "filename.xml");
//! \endcode
//!
//! This parses an XML document from an input sequence defined by a beginning
//! and an ending iterator. The third parameter specifies some label that
//! refers to the document, which gets included in any error messages if there
//! are parsing errors. A fourth optional parameter specifies parsing options.
//!
//! Once a document is parsed or created, use a
//! \ref docBase::readlock "INSERT_LIBX_NAMESPACE::doc::base::readlock" or a
//! \ref docBase::writelock "INSERT_LIBX_NAMESPACE::doc::base::writelock" to
//! examine or modify its contents.
//!
//! \code
//! auto new_document=lock->clone_document();
//! \endcode
//!
//! This clones the entire document and returns a cloned copy of the original.
//! This is a lock method, a lock must be acquired on the document, to prevent
//! it from getting modified while it's being copied.

typedef ref<docObj, docBase> doc;

//! A nullable reference pointer to an \ref doc "XML document".
typedef ptr<docObj, docBase> docptr;

//! A reference to a constant \ref doc "XML document".
typedef const_ref<docObj, docBase> const_doc;

//! A nullable reference pointer to a constant \ref doc "XML document".
typedef const_ptr<docObj, docBase> const_docptr;

#if 0
{
	{
#endif
	}
}
#endif
