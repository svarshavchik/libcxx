/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_parser_H
#define x_yaml_parser_H

#include <x/namespace.h>
#include <x/yaml/parserobj.H>
#include <x/ref.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {
	namespace yaml {
#if 0
	}
}
#endif

//! A YAML parser.

//! The parser's constructor takes a beginning and an ending input sequence
//! iterator, and parses the sequence as one or more YAML documents.
//!
//! \code
//! std::istream i;
//!
//! auto p=INSERT_LIBX_NAMESPACE::yaml::parser::create(std::istreambuf_iterator<char>(i),
//!                       std::istreambuf_iterator<char>());
//!
//! for (auto document:p->documents)
//! {
//!     INSERT_LIBX_NAMESPACE::yaml::node root=document->root();
//!
//!     load_document(root);
//! }
//! \endcode
//!
//! \ref parser "INSERT_LIBX_NAMESPACE::yaml::parser" is a
//! \ref ref "reference to a reference-counted" object that represents zero
//! or more YAML documents that get parsed from a character input sequence.
//! The constructor takes a beginning and an ending iterator for an input
//! sequence of chars from which one or more YAML documents get loaded.
//! This example parses YAML documents from a file.
//!
//! The \c documents member of the parser object is a \c std::list of
//! \ref document "document"s. An input character sequence can contain
//! more than one document.

typedef ref<parserObj> parser;

//! A nullable pointer reference to a \ref parser "YAML parser".

typedef ptr<parserObj> parserptr;

#if 0
{
	{
#endif
	}
}
#endif
