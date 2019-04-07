/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_option_groupdefinitionobj_H
#define x_option_groupdefinitionobj_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/obj.H>
#include <x/option_list.H>
#include <x/option_definitionobj.H>

#include <string>

namespace LIBCXX_NAMESPACE {
	namespace option {
#if 0
};
};
#endif

//! Define an %option group

//! An %option group is a group of %options that are activated by specifying
//! the group option's name first. Then, the remaining options are parsed
//! as the expected group options.

class groupdefinitionObj : public definitionObj {

	//! The individual options in the group

	mutable list groupOptions;

public:

	//! Define the group %option

	groupdefinitionObj(//! Store option's value here
			   ptr<valuebaseObj> valueArg,

			   //! Option's short name

			   //! This may be 0, if there is no shortname
			   char32_t shortnameArg,

			   //! Option's long name
			   std::string longnameArg,

			   //! The group's %options

			   list groupOptionsArg,

			   //! Option's description

			   //! This should be a brief sentence.
			   std::string descriptionArg,

			   //! This should be a single word
			   std::string argDescriptionArg) noexcept;

	~groupdefinitionObj();

	//! Override the set() method.

	int set(//! The parser object that's setting this option
		 parserObj &parserArg) const override LIBCXX_HIDDEN;

	//! Override the set() method.

	int set(//! The parser object that's setting this option
		 parserObj &parserArg,

		 //! The value for this option
		 std::string valueArg,

		 //! Value's locale
		 const const_locale &localeArg) const
		LIBCXX_HIDDEN;

	void reset() override LIBCXX_HIDDEN;

private:
	//! Override the multiple() method.

	//! \internal
	//!
	bool multiple() const noexcept override LIBCXX_HIDDEN;

	//! Override the help() method.

	//! After using the default format for this %option, recursively invoke
	//! help() for the group's options, with a slightly larger indent
	//! level.

	void help(std::ostream &o,
		  size_t indentlevel,
		  size_t width) const override LIBCXX_HIDDEN;

	//! Override the usage() method.

	//! After using the default format for this %option, recursively invoke
	//! usage() for the group's options, with a slightly larger indent
	//! level.

	bool usage(std::ostream &o,
		   size_t indentlevel,
		   size_t width) const override LIBCXX_HIDDEN;

};

#if 0
{
	{
#endif
	}
}
#endif
