/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_option_definitionbaseobj_H
#define x_option_definitionbaseobj_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/obj.H>
#include <x/locale.H>

#include <string>
#include <cctype>
#include <iostream>
#include <courier-unicode.h>

namespace LIBCXX_NAMESPACE {
	namespace option {
#if 0
};
};
#endif

class parserObj;

//! A base class for %option definition

//! The base class defines the option's short and long names, the option's
//! description, and %option flags.

class definitionbaseObj : virtual public obj {

public:
	//! Option's short name
	char32_t shortname;

	//! Option's long name
	std::string longname;

	//! %Option flags
	int flags;

	//! Option's description

	std::string description;

	//! Option's argument description

	std::string argDescription;

	//! Default constructor

	definitionbaseObj(char32_t shortnameArg,
			  const std::string &longnameArg,
			  int flagsArg,
			  const std::string &descriptionArg,
			  const std::string &argDescriptionArg) noexcept;

	//! Default destructor

	~definitionbaseObj();

	//! Set the %option - no value provided

	//! \return 0 if the option was set succesfully, or an error code
	//! otherwise.
	//! \see option

	virtual int set(//! The parser object that's setting this option
			parserObj &parserArg) const=0;

	//! Set the %option to a given value

	//! \return 0 if the option was set succesfully, or an error code
	//! otherwise.
	//! \see option

	virtual int set(//! The parser object that's setting this option
			parserObj &parserArg,

			//! The value for this option
			const std::string &valueArg) const=0;

	//! Reset the option to the initial default

	virtual void reset()=0;

	//! Check if the option is set

	//! \c true if the option was previously set, \c false oterhwise

	virtual bool isSet() const=0;

	//! Format the option's usage summary

	//! The option is generally formatted as \c -s|--longname=value
	//! \return \c true if this is a complex multiline format,
	//! \c false otherwise.

	virtual bool usage(std::ostream &o,
			   size_t indentlevel,
			   size_t width) const;

	//! Return an indication if this option can be specified more than once

	//! This information is used for formatting the \c usage method

	virtual bool multiple() const;

	//! Format the option's help summary

	//! The option is generally formatted as \c -s, --longname=value
	//! followed by an indented description.

	virtual void help(std::ostream &o,
			  size_t indentlevel,
			  size_t width) const;
};

#if 0
{
	{
#endif
	}
}
#endif
