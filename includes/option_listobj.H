/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_option_listobj_H
#define x_option_listobj_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/obj.H>
#include <x/locale.H>
#include <x/option_listfwd.H>
#include <x/option_definitionobj.H>
#include <x/option_definitionfunctionobj.H>
#include <list>
#include <iostream>

namespace LIBCXX_NAMESPACE {

	//! Parse command line options

	namespace option {
#if 0
};
};
#endif

class groupdefinitionObj;

//! A list of options

class listObj : virtual public obj {

	//! Define application name

	std::wstring applicationName;
public:
	//! All defined options

	std::list<ref<definitionbaseObj> > options;

	//! First group %option

	//! This iterator points to the first entry in the options list which
	//! is a group option.

	std::list<ref<definitionbaseObj> >::iterator first_group_option;

	//! Non-option arguments

	std::list<std::wstring> arguments;

	//! Default constructor
	listObj();

	//! Default destructor
	~listObj() noexcept;

	//! Reset all options to defaults

	void reset() LIBCXX_HIDDEN;

	//! Set application's name

	void setAppName(const std::wstring &applicationNameArg);

	//! Set application's name

	void setAppName(const std::string &applicationNameArg,
			const const_locale &l=locale::base::global());

	//! Define a new %option

	listObj &add(//! Store option's value here
		     const ref<valuebaseObj> &valueArg,

		     //! Option's short name

		     //! This may be 0, if there is no shortname
		     //!
		     wchar_t shortnameArg,

		     //! Option's long name
		     const std::wstring &longnameArg,

		     //! Flags

		     //! * \c option::list::base::hasvalue - %option takes a value
		     //! * \c option::list::base::required - this %option must be specified
		     int flagsArg,

		     //! Option's description

		     //! This should be a brief sentence.
		     //!
		     const std::wstring &descriptionArg=L"",

		     //! This should be a single word
		     const std::wstring &argDescriptionArg=L"");


	//! Define a new %option group

	//! When this %option is specified, all subsequent %options get
	//! parsed against the replacement option group.

	listObj &add(//! Store option's value here
		     const ref<valuebaseObj> &valueArg,

		     //! Option's short name

		     //! This may be 0, if there is no shortname
		     //!
		     wchar_t shortnameArg,

		     //! Option's long name
		     const std::wstring &longnameArg,

		     //! The replacement %option group.

		     //! No flags are implied for the %option group entry
		     const list &groupOptionsArg,


		     //! Option's description

		     //! This should be a brief sentence.
		     //!
		     const std::wstring &descriptionArg=L"",

		     //! This should be a single word
		     const std::wstring &argDescriptionArg=L"");

	//! Define a new %option that's handled by a function

	//! When the option is specified, an external function gets invoked.
	//!

	listObj &add(//! This function gets invoked.

		     //! The function should return \c true if the option
		     //! is valid, or \c false to report an error.
		     int (*optfunc)(),

		     //! Option's short name

		     //! This may be 0, if there is no shortname
		     //!
		     wchar_t shortnameArg,

		     //! Option's long name
		     const std::wstring &longnameArg,

		     //! Flags

		     //! * \c option::list::base::hasvalue - %option takes a value
		     //! * \c topion::list::required - this %option must be specified
		     int flagsArg=0,

		     //! Option's description

		     //! This should be a brief sentence.
		     //!
		     const std::wstring &descriptionArg=L"",

		     //! This should be a single word
		     const std::wstring &argDescriptionArg=L"");

	//! Define a new %option that's handled by a function that takes a value

	//! \overload
	//! The function takes a parameter

	template<typename option_value>
	listObj &add(//! This function gets invoked.

		     //! The function should return \c true if the option
		     //! is valid, or \c false to report an error.
		     int (*optfunc)(option_value val),

		     //! Option's short name

		     //! This may be 0, if there is no shortname
		     //!
		     wchar_t shortnameArg,

		     //! Option's long name
		     const std::wstring &longnameArg,

		     //! Flags

		     //! * \c option::list::base::hasvalue - %option takes a value
		     //! * \c topion::list::base::required - this %option must be specified
		     int flagsArg=0,

		     //! Option's description

		     //! This should be a brief sentence.
		     //!
		     const std::wstring &descriptionArg=L"",

		     //! This should be a single word
		     const std::wstring &argDescriptionArg=L"")
	{
		flagsArg |= option::list::base::hasvalue; /* Always */

		options.insert(first_group_option,
			       ref<definitionfuncvalueObj<option_value> >
			       ::create(shortnameArg, longnameArg, flagsArg,
					descriptionArg, argDescriptionArg,
					optfunc));
		return *this;
	}

	//! Define a new %option that's handled by a function that takes a value

	//! \overload
	//! The function takes a parameter that's a reference to a const value

	template<typename option_value>
	listObj &add(//! This function gets invoked.

		     //! The function should return \c true if the option
		     //! is valid, or \c false to report an error.
		     int (*optfunc)(const option_value &val),

		     //! Option's short name

		     //! This may be 0, if there is no shortname
		     //!
		     wchar_t shortnameArg,

		     //! Option's long name
		     const std::wstring &longnameArg,

		     //! Flags

		     //! * \c option::list::base::hasvalue - %option takes a value
		     //! * \c topion::list::base::required - this %option must be specified
		     int flagsArg=0,

		     //! Option's description

		     //! This should be a brief sentence.
		     //!
		     const std::wstring &descriptionArg=L"",

		     //! This should be a single word
		     const std::wstring &argDescriptionArg=L"")
	{
		flagsArg |= option::list::base::hasvalue; /* Always */

		options.insert(first_group_option,
			       ref<definitionfunccrvalueObj<option_value> >
			       ::create(shortnameArg, longnameArg, flagsArg,
					descriptionArg, argDescriptionArg,
					optfunc));
		return *this;
	}

	//! Add definitions of standard options

	//! This method adds the definition of the \c --help, \c --usage,
	//! \c --properties and \c --set-property options.

	listObj &addDefaultOptions(//! Output stream for text produced by these options
				   std::wostream &o=std::wcout);

	//! Add a non-option argument

	listObj &addArgument(//! Non-option argument's name
			     const std::wstring &name,

			     //! Flags

			     //! * option::list::base::required
			     //! * option::list::base::repeated

			     int flags);

	//! Generate a usage message

	void usage(//! Output stream
		   std::wostream &o=std::wcout,

		   //! Output width

		   //! If \c 0, use tty size
		   size_t width=0);

	//! Generate a help message

	void help(//! Output stream
		  std::wostream &o=std::wcout,

		  //! Output width

		  //! If \c 0, use tty size
		  size_t width=0);

private:
	//! Return output terminal width

	//! \internal
	//!
	size_t getTtyWidth() LIBCXX_INTERNAL;

	//! Internal usage formatter
	void usage_internal(std::wostream &o,
			    const std::wstring &ipfix,
			    size_t width,
			    size_t indent) LIBCXX_HIDDEN;

	//! Internal help formatter
	void help_internal(std::wostream &o,
			   size_t width,
			   size_t indent) LIBCXX_HIDDEN;

public:
	friend class groupdefinitionObj;
};

#if 0
{
	{
#endif
	}
}
#endif
