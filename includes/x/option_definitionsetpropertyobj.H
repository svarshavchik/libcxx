/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_option_definitionsetpropertyobj_H
#define x_option_definitionsetpropertyobj_H

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

//! Define a \c --set-property %option

//! Instantiate this class to define an %option that implements the
//! \c --set-property %option.

class definitionSetPropertyObj : public definitionbaseObj {

public:
	//! Default constructor
	definitionSetPropertyObj() noexcept;

	//! Default destructor
	~definitionSetPropertyObj();

	//! Implement the set() method.

	int set(//! The parser object that's setting this option
		 parserObj &parserArg) noexcept override LIBCXX_HIDDEN;

	//! Implement the set() method.

	int set(//! The parser object that's setting this option
		 parserObj &parserArg,

		 //! The value for this option
		 const std::string &valueArg) noexcept
		override LIBCXX_HIDDEN;

	//! Dummy is_set() function

	bool is_set() const noexcept override LIBCXX_HIDDEN;

	//! Dummy reset() function

	void reset() noexcept override LIBCXX_HIDDEN;
};

#if 0
{
#endif
}
#endif
