/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_option_definitionusageobj_H
#define x_option_definitionusageobj_H

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

//! Define a \c --usage %option

//! Instantiate this class to define an %option that implements the
//! canned \c --usage message.

class definitionUsageObj : public definitionbaseObj {

	//! Output stream for the help and usage %messages

	std::ostream &helpStream;

public:
	//! Default constructor
	definitionUsageObj(std::ostream &helpStreamArg) noexcept;

	//! Default destructor
	~definitionUsageObj();

	//! Implement the set() method.

	//! Invokes parser's \c usage() function
	//!
	int set(//! The parser object that's setting this option
		 parserObj &parserArg) const noexcept LIBCXX_HIDDEN;

	//! Implement the set() method.

	//! Invokes parser's \c usage() function
	//!

	int set(//! The parser object that's setting this option
		 parserObj &parserArg,

		 //! The value for this option
		 const std::string &valueArg) const noexcept
		override LIBCXX_HIDDEN;

	//! Dummy isSet() function

	bool isSet() const noexcept LIBCXX_HIDDEN;

	//! Dummy usage() function

	bool usage(std::ostream &o,
		   size_t indentlevel,
		   size_t width) LIBCXX_HIDDEN;

	//! Dummy reset() function

	void reset() noexcept LIBCXX_HIDDEN;

	//! Dummy usage() function

	void help(std::ostream &o,
		  size_t indentlevel,
		  size_t width) LIBCXX_HIDDEN;
};

#if 0
{
	{
#endif
	}
}
#endif
