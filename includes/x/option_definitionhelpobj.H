/*
** Copyright 2012-2021 Double Precision, Inc.
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

namespace LIBCXX_NAMESPACE::option {
#if 0
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
		 parserObj &parserArg) noexcept override;

	//! Implement the set() method.

	//! Invokes parser's \c help() function
	//!

	int set(//! The parser object that's setting this option
		 parserObj &parserArg,

		 //! The value for this option
		 const std::string &valueArg) noexcept override;

	//! Dummy is_set() function

	bool is_set() const noexcept override;

	//! Dummy reset() function

	void reset() noexcept override;
};

#if 0
{
#endif
}
#endif
