/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_option_definitionobj_H
#define x_option_definitionobj_H

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

//! Define an ordinary %option

//! Instantiate this class to define an ordinary %option that may or may not
//! take a parameter.

class definitionObj : public definitionbaseObj {

protected:
	//! Where this option's value is stored

	mutable ptr<valuebaseObj> value;

public:

	//! Define an %option

	definitionObj(//! Store option's value here
		      ptr<valuebaseObj> valueArg,

		      //! Option's short name

		      //! This may be 0, if there is no shortname
		      char32_t shortnameArg,

		      //! Option's long name
		      const std::string &longnameArg,

		      //! Flags

		      //! * \c option::definition::hasvalue - %option takes a value
		      int flagsArg,

		      //! Option's description

		      //! This should be a brief sentence.
		      //!
		      const std::string &descriptionArg,

		      //! This should be a single word
		      const std::string &argDescriptionArg) noexcept;

	~definitionObj();

	//! Implement the set() method.

	int set(//! The parser object that's setting this option
		 parserObj &parserArg) override LIBCXX_HIDDEN;

	//! Implement the set() method.

	int set(//! The parser object that's setting this option
		 parserObj &parserArg,

		 //! The value for this option
		 const std::string &valueArg)
		override LIBCXX_HIDDEN;

	void reset() override LIBCXX_HIDDEN;

	//! Check the is_set() method.

	bool is_set() const override LIBCXX_HIDDEN;

private:
	//! Return an indication if the option can be specified more than once.

	//! \internal
	//!
	bool multiple() const override LIBCXX_HIDDEN;

};

#if 0
{
#endif
}
#endif
