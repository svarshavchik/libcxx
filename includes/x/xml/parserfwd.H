/*
** Copyright 2013-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_parserfwd_H
#define x_xml_parserfwd_H

#include <x/ptrfwd.H>
#include <x/refiterator.H>

namespace LIBCXX_NAMESPACE {
	namespace xml {
#if 0
	};
};
#endif

class parserObj;
class parserBase;

//! A push XML parser

//! \code
//! auto document =
//!     std::copy(container.begin(), container.end(),
//!               INSERT_LIBX_NAMESPACE::xml::parser::create("document.xml"))
//!     .get()->done();
//! \endcode
//!
//! %INSERT_LIBX_NAMESPACE::xml::parser::create() constructs a parser for
//! XML documents.
//!
//! create() takes an optional second parameter, a string containing a
//! comma or a whitespace delimited list of
//! parsing options. This is the list of \c XML_PARSE_ options listed in
//! \c libxml/parser.h. \c XML_PARSE_XINCLUDE gets specified as "xinclude",
//! \c XML_PARSE_NONET as "nonet", and so on.
//!
//! The parser is an output iterator
//! that iterates over %char values. After iterating over the entire contents
//! of the document, get() returns the underlying
//! \ref outputrefiterator "outputrefiterator" subclass, whose
//! \ref parserObj "done"() method returns the parsed document.

typedef refiterator<parserObj, parserBase> parser;

//! A nullable pointer reference to an \ref parser "output iterator XML parser".
typedef ptriterator<parserObj, parserBase> parserptr;

#if 0
{
	{
#endif
	}
}
#endif
