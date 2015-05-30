/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_option_definitionhelpobj_H
#define x_option_definitionhelpobj_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/obj.H>
#include <x/option_definitionbaseobj.H>
#include <x/option_valuebaseobj.H>

#include <string>

namespace LIBCXX_NAMESPACE {
	namespace option {
#if 0
};
};
#endif

//! Define a \c --help %option

//! Instantiate this class to define an %option that implements the
//! canned \c --help message.

class definitionHelpObj : public definitionbaseObj {

	//! Output stream for the help and usage %messages

	std::wostream &helpStream;
public:
	//! Default constructor
	definitionHelpObj(std::wostream &helpStreamArg) noexcept;

	//! Default destructor
	~definitionHelpObj() noexcept;

	//! Implement the set() method.

	//! Invokes parser's \c help() function
	//!
	int set(//! The parser object that's setting this option
		 parserObj &parserArg) const noexcept;

	//! Implement the set() method.

	//! Invokes parser's \c help() function
	//!

	int set(//! The parser object that's setting this option
		 parserObj &parserArg,

		 //! The value for this option
		 const std::string &valueArg,

		 //! Value's locale
		 const const_locale &localeArg) const noexcept;

	//! Dummy isSet() function

	bool isSet() const noexcept;

	//! Dummy usage() function

	bool usage(std::wostream &o,
		   size_t indentlevel,
		   size_t width);

	//! Dummy reset() function

	void reset() noexcept;

	//! Dummy help() function

	void help(std::wostream &o,
		  size_t indentlevel,
		  size_t width);
};

#if 0
{
	{
#endif
	}
}
#endif