/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_option_parserobj_H
#define x_option_parserobj_H

#include <x/namespace.h>
#include <x/option_list.H>
#include <x/locale.H>
#include <vector>
#include <string>
#include <list>
#include <cwchar>

namespace LIBCXX_NAMESPACE::option {
#if 0
};
#endif

class groupdefinitionObj;

//! The parser object

//! \internal
//! \see parser

class parserObj : virtual public obj {

	//! The current %option group.

	list current_options;

	//! The initial %option group

	list initial_options;

	//! The locale used for parsing options

	const const_locale l;

public:
	friend class groupdefinitionObj;

	//! Default constructor
	parserObj(const const_locale &lArg=locale::base::environment())
	noexcept;

	//! Default destructor
	~parserObj();

	//! Current status
	int err_status;

	//! Option related to the parsing error
	std::string err_option;

	//! Define the list of options to parse
	void setOptions(//! List of options
			const list &current_optionsArg);

	//! Parse an argv-style array

	//! \return \c err_ok (0) if options were parsed succesfully,
	//! or a non-zero exit code otherwise.
	int parseArgv(//! argc argument to main()
		      int argc,
		      //! argv argument to main()
		      const char **argv,

		      //! If \c true, consider argv[0] to be part of options
		      bool parse_argv0=false) noexcept;

	//! Parse an argv-style array

	//! \overload
	//!
	int parseArgv(int argc, const char * const *argv,
		      bool parse_argv0=false) noexcept;

	//! Parse an argv-style array

	//! \overload
	//!
	int parseArgv(const std::vector<std::string> &argv,
		      bool parse_argv0=false) noexcept;

	//! Parse options given in a single string

	//! The given string is split into individual words, and gets parsed
	//! accordingly.
	int parseString(const std::string &utf8_string) noexcept;

	//! Validate (for missing options, and such)

	//! \return \c err_ok (0) if options were validated succesfully,
	//! or a non-zero exit code otherwise.

	int validate() noexcept;

	//! Reset option% parsing after validate()

	//! After validate(), use this method to reset all values to their
	//! original defauls, so that another set of options can be parsed.

	void reset();

	//! Return a generic error message

	std::string errmessage() const noexcept;

	//! Non-option words

	std::list<std::string> args;

	//! Generate the help message
	void help(std::ostream &o, size_t w=0) noexcept;

	//! Generate the usage message
	void usage(std::ostream &o, size_t w=0) noexcept;

private:

	//! This function handles the next word.
	void (parserObj::*next_func)(const std::string &) noexcept;

	//! Cleanup function, at end of the given arguments

	void (parserObj::*cleanup_func)() noexcept;

	//! Next word is an option name
	void collect_optname(const std::string &) noexcept LIBCXX_INTERNAL;

	//! Next word is an option value
	void collect_optvalue(const std::string &) noexcept LIBCXX_INTERNAL;

	//! Remaining words are non-parameters
	void collect_params(const std::string &) noexcept LIBCXX_INTERNAL;

	//! An error occured, ignore everything else
	void collect_errignore(const std::string &) noexcept LIBCXX_INTERNAL;

	//! Saved option name
	ptr<definitionbaseObj> optname;

	//! Saved option name
	std::string optname_s;

	//! No cleanup necessary
	void cleanup_none() noexcept LIBCXX_HIDDEN;

	//! Clean up option name orphan
	void cleanup_optname() noexcept LIBCXX_HIDDEN;

	//! An error occured

	void error(//! Error code
		   int errcode,
		   //! Responsible option
		   const std::string &erroption) noexcept LIBCXX_HIDDEN;

};

#if 0
{
#endif
}
#endif
