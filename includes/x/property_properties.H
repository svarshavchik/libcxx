/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_property_properties_H
#define x_property_properties_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/ptr.H>
#include <x/property_valueeventhandlerobj.H>
#include <x/locale.H>
#include <iostream>
#include <map>
#include <list>

namespace LIBCXX_NAMESPACE {
	namespace property {
#if 0
	};
};
#endif

//! Error class for reporting %property load errors

//! Errors while parsing a %property file are reported using this class.
//! This class may choose to log the error, somewhere, or throw an exception.

class errhandler {

public:
	class errthrow;
	class errstream;
	class errlog;

	//! Default constructor

	errhandler() noexcept;

	//~ Default destructor

	virtual ~errhandler();

	//! Report an error that occured when parsing a %property definition.

	virtual void operator()(//! File and line number
				const std::string &location,

				//! Error message
				const std::string &errtxt)
		const=0;


	//! Report an error that occured when parsing a %property definition.

	//! \overload
	void operator()(//! File
			const std::string &location,

			//! Line number
			size_t linenum,

			//! Error message
			const std::string &errtxt) const;

};

//! Report an error by throwing an exception

class errhandler::errthrow : public errhandler {

public:

	//! Default constructor

	errthrow() noexcept;

	//~ Default destructor

	virtual ~errthrow();

	//! Report an error by throwing an exception

	void operator()(//! File and line number
			const std::string &location,

			//! Error message
			const std::string &errtxt)
		const override;
};

//! Report an error by writing out an error message to a file.

class errhandler::errstream : public errhandler {

	//! The output stream
	std::ostream &o;
public:

	//! Default constructor

	errstream(//! Output stream
		  std::ostream &o=std::cerr) noexcept;

	//~ Default destructor

	virtual ~errstream();

	//! Report an error by writing it to the stream

	void operator()(//! File and line number
			const std::string &location,

			//! Error message
			const std::string &errtxt) const override;
};

//! Report an error by writing out an error message to a file.

class errhandler::errlog : public errhandler {

public:

	//! Default constructor

	errlog() noexcept;

	//~ Default destructor

	virtual ~errlog();

	//! Report an error by writing it to the stream

	void operator()(//! File and line number
			const std::string &location,

			//! Error message
			const std::string &errtxt) const override;
};

//! Parse a %property name, defined by an input iterator sequence

template<typename iter_type>
iter_type parsepropname(iter_type b, //!< Beginning iterator
			iter_type e, //!< Ending iterator
			std::list<std::string> &prophier, //!< Parsed %property name
			const const_locale &locale=
			const_locale::base::environment() //!< Locale for parsing
			)
{
	prophier.clear();

	std::string cur_name;

	while (1)
	{
		bool eof=b == e;

		if (eof || *b == ' ' || *b == '\t' || *b == '\r' || *b == '\n'
		    || *b == '<' || *b == '>' || *b == ','
		    || *b == '.' || *b == ':')
		{
			if (cur_name.size() > 0)
			{
				prophier.push_back(cur_name);
				cur_name.clear();
			}

			if (eof)
				break;
		}
		else
		{
			cur_name.push_back(*b);
		}
		++b;
	}

	return b;
}

//! Combine a parsed %property name into a string, using "::" as delimiters

std::string combinepropname(const std::list<std::string> &prophier);

//! Return true if this property is owned by this library

//! \return true if the property is in the \c INSERT_LIBX_NAMESPACE hierarchy
//! or if it contains an "@log" component somewhere in it.

bool islibraryprop(const std::string &value);

//! Enumerate existing properties

void enumerate_properties(std::map<std::string, std::string> &properties);

//! Load global properties from a file

void load_file(//! Filename, for error reporting purposes
	       const std::string &filename,

	       //! Whether existing properties should be overriden
	       bool update,

	       //! Whether nonexisting properties should be created
	       bool create,

	       //! Error handler
	       const errhandler &errh=errhandler::errlog(),

	       //! Locale for interpreting %properties
	       const const_locale &localeRef=locale::create());

//! Load global properties

//! Different subsystems may define default settings. These settings
//! are overriden by anything that's defined in the %property file.
//! This function should only be used when the application starts up.
//! Properties that are already defined do not get updated by
//! load_properties().
//!
//! \code
//!   std::ifstream i("app.config");
//!
//!   if (i.is_open())
//!     property::load_properties(i, "app.config");
//! \endcode
//!
//! The %properties get loaded from a stream.
//! The %property settings are specified as if they
//! were specified in the application's default %property file.
//! The values read from the file take effect only if they were not
//! already specified in the %property file.

void load_properties(//! Properties are read from this input stream
		     std::istream &propertyfile,

		     //! Filename, for error reporting purposes
		     const std::string &filename,

		     //! Whether existing properties should be overriden
		     bool update,

		     //! Whether nonexisting properties should be created
		     bool create,

		     //! Error handler
		     const errhandler &errh=errhandler::errlog(),

		     //! Locale for interpreting %properties
		     const const_locale &localeRef=locale::create())
;

//! Load initial properties

//! \overload
//!
//! \code
//!    property::load_properties( "window::width=300\n"
//!                               "window::height=100\n");
//! \endcode
//! The %property settings are specified as if they
//! were specified in a %property file. The values passed to
//! %property::load_properties() take effect only if they were not already
//! defined.

void load_properties(//! Literal %properties
		     const std::string &default_properties,
		     //! Whether existing properties should be overriden
		     bool update,

		     //! Whether nonexisting properties should be created
		     bool create,

		     //! Error handler
		     const errhandler &errh=errhandler::errthrow(),

		     //! Locale for interpreting %properties
		     const const_locale &localeRef=locale::create())
;

//! Update a single %property value

//! \code
//! property::load_property("main.window.width", "400", true, true);
//! \endcode
//!
//! Set a value of a single %property, the value specified by a string value.

void load_property(//! %Property name
		   const std::string &propnamestr,

		   //! %Property value
		   const std::string &valuestr,

		   //! Whether existing properties should be overriden
		   bool update,

		   //! Whether nonexisting properties should be created
		   bool create,

		   //! Error handler
		   const errhandler &errh=errhandler::errlog(),

		   //! Locale for interpreting the %property value
		   const const_locale &localeRef=const_locale::base::global());

template<typename iter_type>
iter_type save_properties(//! Properties
			  const std::map<std::string, std::string> &properties,

			  //! Output iterator
			  iter_type iter,
			  //! Locale for parsing
			  const const_locale &locale=
			  const_locale::base::environment());


#ifndef DOXYGEN
#define LIBCXX_TEMPLATE_DECL extern
#include <x/property_properties_t.H>
#undef LIBCXX_TEMPLATE_DECL
#endif

#if 0
{
	{
#endif
	}
}
#endif
