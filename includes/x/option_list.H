/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_option_list_H
#define x_option_list_H

#include <x/namespace.h>
#include <x/option_listfwd.H>
#include <x/option_listobj.H>
#include <x/option_valueobj.H>
#include <x/option_valueopobj.H>
#include <x/option_listvalueobj.H>
#include <list>

//! Parse command line options

//! This namespace defines objects for parsing command line options received
//! by the application. This is not a particularly speedy or efficient set of
//! classes. Command line parsing needs to be done just once. These objects
//! trade speed and efficiency for power. This is a very flexible command
//! line parsing library that includes automatic conversion of arguments
//! to native datatypes, as well as a built-in implementation of several
//! standard options, such as \c --help
//! and --usage. The help message text gets assembled
//! from the %option metadata.
//!
//! The process of parsing command line options involves several steps.
//!
//! \par Instantiate a value object
//!
//! Value objects hold the actual values of individual %options:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::option::bool_value verbose_value(INSERT_LIBX_NAMESPACE::option::bool_value::create());
//!
//! // ...
//!
//! if (verbose_value->value)
//! {
//!    // ... option was specified
//! }
//! \endcode
//!
//! Use an \c INSERT_LIBX_NAMESPACE::option::bool_value for options that don't take any values.
//! The %option gets specified without a value parameter.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::option::int_value retries_value(INSERT_LIBX_NAMESPACE::option::int_value::create());
//! \endcode
//!
//! Use an \c INSERT_LIBX_NAMESPACE::option::int_value for options that take an \c int value.
//! The %option's parameter gets automatically converted to an \c int and
//! placed into the object.
//! It's possible to place an initial, default value:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::option::int_value retries_value(INSERT_LIBX_NAMESPACE::option::int_value::create(4));
//! \endcode
//!
//! After the command line options are parsed, access the \c value member
//! to retrieve the option's given value.
//!
//! \code
//! if (retries_value->is_set())
//! {
//!     int retries=retries_value->value;
//!
//!     // ...
//! }
//! \endcode
//!
//! This will be the initial, default value (the default value is zero, unless
//! explicitly specified) or the value specified by the %option parameter.
//! Access the \c specified member to check if the %option was specified on the
//! command line (otherwise \c value remains its initial default contents).
//!
//! Other value objects are available for different types:
//! \c INSERT_LIBX_NAMESPACE::option::uint_value,
//! \c INSERT_LIBX_NAMESPACE::option::long_value,
//! \c INSERT_LIBX_NAMESPACE::option::ulong_value,
//! \c INSERT_LIBX_NAMESPACE::option::longlong_value,
//! \c INSERT_LIBX_NAMESPACE::option::ulonglong_value,
//! \c INSERT_LIBX_NAMESPACE::option::int16_value,
//! \c INSERT_LIBX_NAMESPACE::option::int32_value,
//! \c INSERT_LIBX_NAMESPACE::option::int64_value,
//! \c INSERT_LIBX_NAMESPACE::option::uint16_value,
//! \c INSERT_LIBX_NAMESPACE::option::uint32_value, and
//! \c INSERT_LIBX_NAMESPACE::option::uint64_value.
//!
//! An \c INSERT_LIBX_NAMESPACE::option::string_value stores the option's value as an unformatted
//! \c std::string.
//!
//! An \c INSERT_LIBX_NAMESPACE::option::hms_value stores the option's value into an
//! \ref hms "hms" object.
//!
//! An \c INSERT_LIBX_NAMESPACE::option::ymd_value stores the option's value into a
//! \ref ymd "ymd" object.
//!
//! An \c INSERT_LIBX_NAMESPACE::option::ymd_interval_value stores the option's value into an
//! ymd::interval object.
//!
//! An \c INSERT_LIBX_NAMESPACE::option::uri_value stores the option's value into a
//! \ref uriimpl "uriimpl" object.
//!
//! An \c INSERT_LIBX_NAMESPACE::option::memsize stores the option's value into a
//! \ref memsize "memsize" object. This is a means of specifying
//! memory or file size as a command line option string.
//!
//! \par Options that initialize a custom class
//!
//! In addition to the predefined option values, with some work any custom
//! class can be initialized with an option. The custom option class must
//! meet be convertable to and from character string using the
//! \link value_string value_string template\endlink,
//! then used as follows:
//!
//! \code
//!
//! typedef ref<INSERT_LIBX_NAMESPACE::option::valueObj<customObj> > custom_value;
//!
//! \endcode
//!
//! The option value type is defined by instantiating a reference to an
//! INSERT_LIBX_NAMESPACE::option::valueObj template. Subsequently, this option value may be used
//! as any other option value (see below for more information):
//!
//! \code
//! int main(int argc, char **argv)
//! {
//!     custom_value custom(custom_value::create());
//!
//!     INSERT_LIBX_NAMESPACE::option::list l(INSERT_LIBX_NAMESPACE::option::list::create());
//!
//!     l->add(custom, 'c', "custom",
//!            INSERT_LIBX_NAMESPACE::option::list::hasvalue,
//!            "Custom option",
//!            "value");
//!     l->addDefaultOptions();
//!
//!     INSERT_LIBX_NAMESPACE::option::parser
//!         optionParser(INSERT_LIBX_NAMESPACE::option::parser::create(locale::base::environment(""))));
//!
//!
//!     optionParser->setOptions(l);
//!
//!     int err=optionParser->parseArgv(argc, argv);
//!
//!     if (err == 0)
//!         err=optionParser->validate();
//!
//!     switch (err) {
//!     case 0:
//!         break;
//!     case INSERT_LIBX_NAMESPACE::option::parser::err_builtin:
//!         exit(1);
//!         break;
//!     default:
//!         std::cerr << optionParser-&gt;errmessage();
//!         exit(1);
//!     }
//!
//!     std::cout << "Option value is " << custom->value.foo << std::endl;
//!     return (0);
//! }
//! \endcode
//!
//! \par Combining options together
//!
//! It's possible to define multiple values that are combined together using
//! an \c OR, \c AND, or an \c XOR operation. Define a single value first,
//! then instantiate \c INSERT_LIBX_NAMESPACE::option::or_op::value,
//! \c INSERT_LIBX_NAMESPACE::option::and_op::value, or \c INSERT_LIBX_NAMESPACE::option::xor_op::value:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::option::int_value debug_value(INSERT_LIBX_NAMESPACE::option::int_value::create(0));
//!
//! INSERT_LIBX_NAMESPACE::option::or_op<int>::value
//!     debug_alloc_value(INSERT_LIBX_NAMESPACE::option::or_op<int>::value::create(debug_value, 1)),
//!     debug_auth_value(INSERT_LIBX_NAMESPACE::option::or_op<int>::value::create(debug_value, 2));
//!
//! // ...
//!
//! if (debug_value->value & 1)
//! {
//!     // ...
//! }
//!
//! if (debug_value->value & 2)
//! {
//!     // ...
//! }
//! \endcode
//!
//! Do not create an %option definition object for \c debug_value (see below),
//! create definition objects for \c debug_alloc_value and
//! \c debug_auth_value. \c debug_value gets instantiated with an initial
//! value of 0 (this is the default value for integer options, and the explicit
//! 0 value can be omited from the create() method).
//! If the \c debug_alloc_value %option gets specified, the value of
//! \c debug_value is \c OR -ed with the value 1.
//! If the \c debug_auth_value %option gets specified, the value of
//! \c debug_value is \c OR -ed with the value 2.
//!
//! These operation class templates may be used in conjunction with any
//! integer value %option, by using the matching template.
//!
//! \par Option lists
//!
//! Value objects hold a single value. If the %option is specified more than
//! once
//! either the last value is kept, or an error gets reported (based on
//! the option's definition). A separate set of objects can store more than
//! one value:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::option::string_list filename_values(INSERT_LIBX_NAMESPACE::option::string_list::create());
//!
//! // ...
//!
//! std::list<std::string>::iterator
//!     b=filename_values->value.begin(),
//!     e=filename_values->value.end();
//! // ...
//! \endcode
//!
//! The option's \c value member is a \c std::list, rather than a single
//! value. Each time this %option is specified, its specified value gets added
//! to the list. A \c INSERT_LIBX_NAMESPACE::option::string_list stores %option values verbatim,
//! as \c std::string s. Other available objects, that convert %option values
//! to an appropriate type, are:
//! \c INSERT_LIBX_NAMESPACE::option::int_list,
//! \c INSERT_LIBX_NAMESPACE::option::uint_list,
//! \c INSERT_LIBX_NAMESPACE::option::long_list,
//! \c INSERT_LIBX_NAMESPACE::option::ulong_list,
//! \c INSERT_LIBX_NAMESPACE::option::longlong_list,
//! \c INSERT_LIBX_NAMESPACE::option::ulonglong_list,
//! \c INSERT_LIBX_NAMESPACE::option::string_list,
//! \c INSERT_LIBX_NAMESPACE::option::int16_list,
//! \c INSERT_LIBX_NAMESPACE::option::int32_list,
//! \c INSERT_LIBX_NAMESPACE::option::int64_list,
//! \c INSERT_LIBX_NAMESPACE::option::uint16_list,
//! \c INSERT_LIBX_NAMESPACE::option::uint32_list, and
//! \c INSERT_LIBX_NAMESPACE::option::uint64_list.
//!
//! Use an \c INSERT_LIBX_NAMESPACE::option::bool_list for options that don't take values, yet
//! may be specified more than once, like the common "-v" %option. Each
//! occurence of this %option adds a \c true -valued \c bool to the
//! \c value list. Use \c size() to determine how many times \c -v was
//! specified.
//!
//! \c INSERT_LIBX_NAMESPACE::option::hms_list,
//! \c INSERT_LIBX_NAMESPACE::option::ymd_list,
//! \c INSERT_LIBX_NAMESPACE::option::ymd_interval_list, and
//! \c INSERT_LIBX_NAMESPACE::option::uri_list define lists of options that use the corresponding
//! classes.
//!
//! \par Mutually exclusive options
//!
//! \code
//! INSERT_LIBX_NAMESPACE::option::string_value filename_opt(INSERT_LIBX_NAMESPACE::option::string_value::create());
//! INSERT_LIBX_NAMESPACE::option::string_value command_opt(INSERT_LIBX_NAMESPACE::option::string_value::create());
//!
//! INSERT_LIBX_NAMESPACE::option::mutex filenamecmd_mutex(INSERT_LIBX_NAMESPACE::option::mutex::create());
//!
//! filenamecmd_mutex->add(filename_opt).add(command_opt);
//! \endcode
//!
//! This example defines two %option values: \c filename_opt and \c command_opt.
//! Only one of them may be specified, and an error will be returned if both
//! are provided. An INSERT_LIBX_NAMESPACE::option::mutex defines a group of mutually-exclusive
//! options, which are placed into the group by invoking the \c add()
//! method for each mutually-exclusive option. More than one mutually-exclusive
//! group objects can be created, but an %option cannot belong to more than one
//! mutually-exclusive %option group.
//!
//! \par Define the option list
//!
//! <a name="optionlist"></a>Once all values are defined, they must be collected into a single %option
//! list.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::option::list options(INSERT_LIBX_NAMESPACE::option::list::create());
//!
//! options->add(retries_value, 'r', "retries",
//!              INSERT_LIBX_NAMESPACE::option::list::hasvalue,
//!              "How many times to retry",
//!              "count")
//!         .add(filename_values, 'f', "",
//!              INSERT_LIBX_NAMESPACE::option::list::hasvalue,
//!              "Filename to process",
//!              "filename");
//! \endcode
//!
//! An INSERT_LIBX_NAMESPACE::option::list object holds all options and their descriptions.
//! Each %option
//! gets added by invoking the add() method, which returns a reference
//! to the same object. This allows chaining consecutive add() methods, as
//! in this example. The parameters to the add() method are:
//!
//! - the option's value
//! - the option's short, one character name (specify 0, if none)
//! - the option's long name (specify an empty string, if none)
//! - a flag value
//! - the option's description, this should be a brief sentence
//! - the option's argument's description, this should be one or two words
//!
//! Note that the option's long name, description, and argument description
//! must be encoded in UTF-8.
//!
//! Specify 0 for the flag, if none of the following flags are needed.
//! Combine multiple flags by \c OR -ing them together:
//!
//! - \c INSERT_LIBX_NAMESPACE::option::list::hasvalue - this %option takes a value.
//! - \c INSERT_LIBX_NAMESPACE::option::list::required - this %option is required, it must be
//! specified.
//!
//! The semantics of INSERT_LIBX_NAMESPACE::option::list::hasvalue are
//! slightly different for options specified by their long name or by their
//! short, one character names.
//!
//! "--option=value" always specifies the value for the option, whether or not
//! it has \c INSERT_LIBX_NAMESPACE::option::list::hasvalue set.
//! For long options, \c INSERT_LIBX_NAMESPACE::option::list::hasvalue decides
//! what happens with a "--option" that does not affixed with "=value".
//!
//! * Without a \c INSERT_LIBX_NAMESPACE::option::list::hasvalue, the next
//! word is expected to be another "--option" or "-o", otherwise this is the
//! end of the option list.
//!
//! * With a \c INSERT_LIBX_NAMESPACE::option::list::hasvalue,  the next word
//! sets the option's value (even if it starts with a "-").
//!
//! Short option names that do not set
//! \c INSERT_LIBX_NAMESPACE::option::list::hasvalue may be specified together.
//! "-abc" is the same as "-a -b -c", if neither of them set
//! \c INSERT_LIBX_NAMESPACE::option::list::hasvalue.
//!
//! A short name of an option with a
//! \c INSERT_LIBX_NAMESPACE::option::list::hasvalue
//! must appear by itself, with its
//! value specified as either "-ovalue", "-o=value", or "-o value".
//!
//! \code
//! if (option_value->is_set())
//! {
//!     int retries=option_value->value;
//!
//!     // ...
//! }
//! \endcode
//!
//! A specified option's is_set() returns true. If its value was not provided,
//! the option's \c value is set to the default value for that option.
//! Generally, the default value for numeric option is 1, for a
//! \c INSERT_LIBX_NAMESPACE::option::bool_value is \c true, and string options
//! have a default value of an empty string.
//!
//! \par Define non-option arguments
//!
//! \code
//! options->addArgument("mode", INSERT_LIBX_NAMESPACE::option::list::required)
//!         .addArgument("filename", INSERT_LIBX_NAMESPACE::option::list::repeated);
//! \endcode
//!
//! The addArgument() method defines non-option arguments
//! (see "Parse the options"). This is for informational purposes only.
//! The specified metadata gets added to the output of the \c --usage
//! option, no other processing takes place. All non-option arguments
//! are collected and returned in the args member as a single vector
//! (see below).
//!
//! The first argument to addArgument() is a wide-character string that
//! names the non-option argument. The second argument is a bitmask that
//! specifies the following flags:
//!
//! - \c INSERT_LIBX_NAMESPACE::option::list::required - this is a required argument
//!
//! - \c INSERT_LIBX_NAMESPACE::option::list::repeated - this argument may be repeated
//!
//! \par Function options
//!
//! Rather than storing %option values in objects, it is possible to pass
//! an %option value to a function. This is defined by an overloaded
//! \c add() method:
//!
//! \code
//! bool set_verbose()
//! {
//!    // ...
//!    return 0;
//! }
//!
//! int set_debug_level(int debug_level)
//! {
//!    // ...
//!    return 0;
//! }
//!
//! ...
//!
//! options->add(&set_verbose, 'v', "enable-verbose", 0,
//!                           "Produce verbose output")
//!         .add(&set_debug_level, 'd', "debug-level", INSERT_LIBX_NAMESPACE::option::list::hasvalue,
//!                           "Set debugging level");
//!
//! \endcode
//!
//! When the first parameter to add() is a function, specifying the %option
//! invokes the given function. The %option function may either take no
//! parameters, or take one parameter, and the %option's value gets
//! automatically converted to the parameter's type. The remaining parameters
//! to add() are the same.
//!
//! When the function's prototype takes a parameter value, the fourth parameter
//! to add(), flags, will always have \c INSERT_LIBX_NAMESPACE::option::list::hasvalue set,
//! automatically.
//!
//! The function should return 0 if the %option or its value is acceptable, or
//! one of the error codes, defined below.
//!
//! \par Define default options
//!
//! Default \c --help and \c --usage option implementations are available that
//! assemble and print a help/usage message from the option metadata.
//! These options are disabled by default, and must be explicitly enabled:
//!
//! \code
//! options->addDefaultOptions();
//! \endcode
//!
//! \par Instantiate a parser object
//!
//! \code
//! INSERT_LIBX_NAMESPACE::option::parser optionParser(INSERT_LIBX_NAMESPACE::option::parser::create(locale));
//!
//! optionParser->setOptions(options);
//! \endcode
//!
//! The INSERT_LIBX_NAMESPACE::option::parser object implements the actual command line parsing.
//! Its constructor takes an optional \ref locale "locale" reference specifying
//! the locale for parsing the options. Also,
//! before the options get parsed, the \c setOptions() method must receive
//! the INSERT_LIBX_NAMESPACE::option::list object that specifies the list of options to parse.
//!
//! \par Parse the options
//!
//! \code
//! int err=optionParser->parseArgv(argc, argv)
//!
//! if (err == 0 && (err=optionParser->validate()) == 0)
//! {
//!     std::list<std::string> args=optionParser->args;
//!
//!     // ...
//! }
//! \endcode
//!
//! The \c parseArgv() function parses the options specified in the given
//! word array. Pass it the explicit \c argc and \c argv arguments that
//! \c main() receives. Normally, \c argv[0] gets ignored, that's the
//! application's filename. Supply \c true as an optional third argument to
//! \c parseArgv() in order to process \c argv[0] as well.
//!
//! \code
//! std::vector<std::string> argv;
//!
//! int err=optionParser->parseArgv(argv);
//! \endcode
//!
//! An alternative \c parserArgv() method takes a string %vector instead.
//! The alternative function also ignores \c argv[0], unless \c true is
//! given as the optional second parameter.
//!
//! \code
//! int err=optionParser->parseString("-o filename.out -v");
//! \endcode
//!
//! \c parseString() is an alternative function that parses options out of
//! a single string. The given string is split into individual words, and
//! they are parsed for options (all words are always processed, including
//! the first one). The string gets split across whitespace boundaries.
//! Either \c &quot; or \c ' may be used to delimit individual words that contain
//! whitespace, and backslashes escape their following character from any
//! special meaning.
//!
//! These functions return 0 if no errors were detected, or a non-zero
//! error code. A 0 return code does not mean that all options were completely
//! processed, just that no errors were yet detected. Any combination of
//! \c parseArgv() and \c parseArgs() may be invoked more than once, with the
//! end result as if all their words were combined together and processed as a
//! single %option list.
//!
//! The \c validate() method is required to finalize %option parsing. All
//! %option parsing is completed, and all %option values are placed into their
//! corresponding objects. After \c validate(), the non-option arguments are
//! collected into the \c args list, which is a \c std::list&lt;std::string&gt;
//! member in the %option parser object.
//! The value objects created in the initial step
//! may be checked only after validate() returns a 0 return code, with a
//! non zero return code indicating an error. The possible error codes are:
//!
//! - \c INSERT_LIBX_NAMESPACE::option::parser::err_invalidoption - The %option does not exist,
//! or its specified value is not valid.
//! An example would be a non-numerical value specified for a numerical %option.
//!
//! - \c INSERT_LIBX_NAMESPACE::option::parser::err_missingoption - A required %option was not
//! given.
//!
//! - \c INSERT_LIBX_NAMESPACE::option::parser::err_mutexoption - Mutually-exclusive %options
//! were given.
//!
//! - \c INSERT_LIBX_NAMESPACE::option::parser::err_filenotfound - Unable to open an %option file
//! (see below).
//!
//! - \c INSERT_LIBX_NAMESPACE::option::parser::err_builtin - Either \c --help, \c --usage, or the
//! \c --properties %option was
//! specified. This is a mechanism to let the application know it should
//! terminate, rather than start, after processing this %option. This %option
//! library generates and prints the output of this %option, then returns this
//! error code.
//!
//! After a 0 return code from \c validate(), the \c args member of the
//! parser object will contain a list of non-option words.
//! If \c parseArgv() or \c parseArgs() returns a non-0 error code, subsequent
//! invocations are generally ignored, and the same error code gets returned,
//! and the subsequent \c validate() also returns the same error code.
//!
//! The parser object has an \c err_option member that gives the name of the
//! %option that reported the error:
//! \code
//! int err=optionParser->validate();
//!
//! switch (err) {
//! case 0:
//!        break;
//! case INSERT_LIBX_NAMESPACE::option::parser::err_invalidoption:
//!        std::cout << "Invalid option: "
//!                  << optionParser->err_option << std::endl;
//!        break;
//! case INSERT_LIBX_NAMESPACE::option::parser::err_missingoption:
//!        std::cout << "Missing option: "
//!                  << optionParser->err_option << std::endl;
//!        break;
//! case INSERT_LIBX_NAMESPACE::option::parser::err_mutexoption:
//!	   std::cout << "Mutually exclusive option: " + err_option;
//!        break;
//! case INSERT_LIBX_NAMESPACE::option::parser::err_filenotfound:
//!        std::cout << "File not found: "
//!                  << optionParser->err_option << std::endl;
//!        break;
//! default:
//!        std::cout << "Unknown error parsing options" << std::endl;
//!        break;
//! };
//!
//! // Or, alternatively, same as above:
//!
//! std::cout << optionParser->errmessage();
//!
//! \endcode
//!
//! \note
//! The main program must not use \c std::cout until after the options are
//! parsed. The \c --help and \c --usage options may not work if the main
//! program
//! uses \c std::cout (this is because these options generate wide character
//! output which cannot be mixed with \c std::cout ).
//!
//! \par Specifying options
//!
//! This library understands several alternative conventions for specifying
//! options. Use \c -<b>%X</b>, with a single dash, to specify an %option by its
//! short,
//! one character name. Use \c --<b>longname</b>, with a double dash, to use the
//! option's long name.
//!
//! Options that take a value can be specified as \c -<b>%X</b>&nbsp;<i>value</i>,
//! \c -<b>%X</b>=<i>value</i>, \c --<b>longname</b>&nbsp;<i>value</i>, or
//! \c --<b>longname</b>=<i>value</i>. Short names can also use
//! \c -<b>%X</b><i>value</i>. Finally, short %option names that do not take
//! values can be combined together: \c -<b>abc</b> is equivalent to
//! \c -<b>a</b>&nbsp;-<b>b</b>&nbsp;-<b>c</b>.
//!
//! Options and non-option arguments can be freely mixed. The %option library
//! scans the entire list of %option words, and removes those that look like
//! options, placing the remaining words in the non-option list.
//! A lone \c <b>--</b> terminates the %option scan. All remaining words are
//! placed in the non-option list, even if they look like options.
//!
//! Finally, \c <b>\@</b><i>filename</i> (if it appears before the
//! \c <b>--</b> ) opens the file and reads additional
//! options from the file, as if \c <b>\@</b><i>filename</i> is replaced by
//! the file's contents. The contents of the file are processed as if they
//! were processed by \c parseString() (and they are).

namespace LIBCXX_NAMESPACE::option {
#if 0
};
#endif

#if 0
{
#endif
}
#endif
