/*
** Copyright 2012-2019 Double Precision, Inc.
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

	std::ostream &helpStream;
public:
	//! Default constructor
	definitionHelpObj(std::ostream &helpStreamArg) noexcept;

	//! Default destructor
	~definitionHelpObj();

	//! Implement the set() method.

	//! Invokes parser's \c help() function
	//!
	int set(//! The parser object that's setting this option
		 parserObj &parserArg) const noexcept override;

	//! Implement the set() method.

	//! Invokes parser's \c help() function
	//!

	int set(//! The parser object that's setting this option
		 parserObj &parserArg,

		 //! The value for this option
		 const std::string &valueArg) const noexcept override;

	//! Dummy isSet() function

	bool isSet() const noexcept override;

	//! Dummy usage() function

	bool usage(std::ostream &o,
		   size_t indentlevel,
		   size_t width);

	//! Dummy reset() function

	void reset() noexcept override;

	//! Dummy help() function

	void help(std::ostream &o,
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
