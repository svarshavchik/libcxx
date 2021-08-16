/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_option_parserfwd_H
#define x_option_parserfwd_H

#include <x/namespace.h>
#include <x/locale.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE::option {
#if 0
}
#endif

class parserObj;
class parserBase;

//! Reference to a parser object

//! This is a reference to a reference-counted object. The parser object
//! parses %option lists.
//! \see option

typedef ref<parserObj, parserBase> parser;

//! A nullable pointer reference to a parser object

//! \see parser

typedef ptr<parserObj, parserBase> parserptr;

//! Refer to this class as \c parser::base or \c parserptr::base

class parserBase : public ptrref_base {

public:

	static constexpr int err_ok=0,	//!< Status: no errors occured
		err_invalidoption=1,	//!< Status: invalid %option
		err_missingoption=2,	//!< Status: missing %option
		err_mutexoption=3,	//!< Status: mutually-exclusive %option was specified
		err_filenotfound=4,	//!< Status: %option file not found
		err_builtin=5;		//!< Status: --help, --usage, or --properties
};

#if 0
{
#endif
}
#endif
