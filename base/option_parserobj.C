/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/option_parser.H"
#include "x/tostring.H"
#include "gettext_in.h"
#include "x/messages.H"
#include <cwchar>
#include <cctype>
#include <sstream>
#include <fstream>

namespace LIBCXX_NAMESPACE {
	namespace option {
#if 0
	}
}
#endif

parserObj::parserObj(const const_locale &lArg) noexcept
	: current_options(list::create()),
	  initial_options(current_options), l(lArg)
{
	reset();
}

parserObj::~parserObj() noexcept
{
}

void parserObj::setOptions(const list &current_optionsArg)
{
	initial_options=current_optionsArg;
	reset();
}

void parserObj::reset()
{
	current_options=initial_options;
	current_options->reset();

	err_status=0;
	err_option="";
	args.clear();
	next_func= &parserObj::collect_optname;
	cleanup_func= &parserObj::cleanup_none;
}

int parserObj::parseArgv(int argc,
			 const char **argv,
			 bool parse_argv0) noexcept
{
	if (argc && !parse_argv0)
	{
		++argv;
		--argc;
	}

	while (argc)
	{
		--argc;
		(this->*next_func)(unicode::iconvert
				   ::convert(*argv++,
					     unicode_default_chset(),
					     unicode::utf_8));
	}

	(this->*cleanup_func)();
	return err_status;
}

int parserObj::parseArgv(int argc, const char * const *argv,
			 bool parse_argv0) noexcept
{
	if (argc && !parse_argv0)
	{
		++argv;
		--argc;
	}
	while (argc)
	{
		--argc;
		(this->*next_func)(unicode::iconvert
				   ::convert(*argv++,
					     unicode_default_chset(),
					     unicode::utf_8));
	}
	(this->*cleanup_func)();
	return err_status;
}

int parserObj::parseArgv(const std::vector<std::string> &argv,
			 bool parse_argv0) noexcept
{
	std::vector<std::string>::const_iterator
		b=argv.begin(), e=argv.end();

	if (b != e && !parse_argv0)
		++b;

	while (b != e)
	{
		(this->*next_func)(unicode::iconvert
				   ::convert(*b++,
					     unicode_default_chset(),
					     unicode::utf_8));
	}
	(this->*cleanup_func)();
	return err_status;
}

int parserObj::parseString(const std::string &argstring) noexcept
{
	std::vector<std::string> words_vec;

	{
		std::list<std::string> words;

		auto b=argstring.begin(), e=argstring.end();

		while (b != e)
		{
			if (isspace(*b))
			{
				++b;
				continue;
			}

			std::ostringstream w;

			int inquote=0;

			auto p=b;

			while (b != e && (inquote || !isspace(*b)))
			{
				if (!inquote && (*b == '\'' || *b == '"'))
				{
					w << std::string(p, b);

					inquote=*b;
					p= ++b;
					continue;
				}

				if (inquote && *b == inquote)
				{
					w << std::string(p, b);
					inquote=0;
					p= ++b;
					continue;
				}

				if (*b == '\\')
				{
					w << std::string(p, b);
					p=++b;
					if (b == e)
						break;
				}

				++b;
			}
			w << std::string(p, b);

			words.push_back(w.str());
		}
		words_vec.reserve(words.size());
		words_vec.insert(words_vec.end(), words.begin(), words.end());
	}

	return parseArgv(words_vec, true);
}

void parserObj::error(int errcode,
		      const std::string &erroption) noexcept
{
	err_status=errcode;
	err_option=erroption;
	next_func=&parserObj::collect_errignore;
	cleanup_func=&parserObj::cleanup_none;
}

void parserObj::collect_errignore(const std::string &argstring) noexcept
{
}

void parserObj::cleanup_none() noexcept
{
}

void parserObj::collect_optname(const std::string &argstring_arg) noexcept
{
	auto argstring=argstring_arg;

	if (argstring.substr(0, 1) == "@")
	{
		std::ifstream ifs(argstring.substr(1).c_str());

		if (!ifs.is_open())
		{
			error(parser::base::err_filenotfound, argstring);
			return;
		}


		std::string line;

		while (!std::getline(ifs, line).eof())
		{
			line=unicode::iconvert::convert(line,
							unicode_default_chset(),
							unicode::utf_8);
			parseString(line);
			line="";
			if (err_status)
				break;
		}
		parseString(line);
		return;
	}

	if (argstring == "--")
	{
		(this->*cleanup_func)();
		cleanup_func= &parserObj::cleanup_none;
		next_func= &parserObj::collect_params;
		return;
	}

	cleanup_func= &parserObj::cleanup_none;

	if (argstring.substr(0, 1) != "-")
	{
		args.push_back(argstring);
		return;
	}

	std::list<ref<definitionbaseObj> > &options=
		current_options->options;

	std::list<ref<definitionbaseObj> >::const_iterator b, e;

	std::string argvalue;

	size_t equal_pos=argstring.find('=');

	if (equal_pos != argstring.npos)
	{
		argvalue=argstring.substr(equal_pos+1);
		argstring=argstring.substr(0, equal_pos);
	}

	if (argstring.substr(0, 2) == "--")
	{
		argstring=argstring.substr(2);

		b=options.begin();
		e=options.end();

		while (b != e)
		{
			if ((*b)->longname == argstring)
				break;
			++b;
		}

		if (b == e)
		{
			error(parser::base::err_invalidoption,
			      argstring_arg);
			return;
		}

		// Value given?

		if (argvalue.size() > 0)
		{
			int rc=(*b)->set(*this, argvalue);

			if (rc)
			{
				error(rc, argstring_arg);
				return;
			}
			return;
		}

		// Option takes no value, can process it now.

		if (!((*b)->flags & list::base::hasvalue))
		{
			int rc= (*b)->set(*this);

			if (rc)
			{
				error(rc, argstring_arg);
				return;
			}
			return;
		}
	}
	else
	{
		// Single "-" options that take no values can be specified
		// together.

		auto opts=unicode::iconvert::tou::convert(argstring.substr(1),
							  unicode::utf_8).first;

		auto sb=opts.begin(), se=opts.end();

		while (sb != se)
		{
			b=options.begin();
			e=options.end();

			while (b != e)
			{
				if ((*b)->shortname == *sb)
					break;
				++b;
			}

			if (b == e)
			{
				error(parser::base::err_invalidoption, argstring);
				return;
			}

			if (!((*b)->flags & list::base::hasvalue))
			{
				int rc= (*b)->set(*this);

				if (rc)
				{
					error(rc, argstring);
					return;
				}
				++sb;
				continue;
			}
			break;
		}

		if (sb == se)
		{
			// All short options took no values

			if (argvalue.size() > 0)
			{
				// Value for what?

				error(parser::base::err_invalidoption, argstring);
				return;
			}

			return; // All no-value options parsed
		}

		// Short option that takes a value

		if (sb+1 == se)
		{
			// This was the last option

			if (argvalue.size() > 0)
			{
				// And we have a value

				int rc=(*b)->set(*this, argvalue);

				if (rc)
				{
					error(rc, argstring);
					return;
				}
				return;
			}
		}
		else
		{
			// Have the value right there

			int rc=parser::base::err_invalidoption;

			++sb;
			if (argvalue.size() > 0 || // What's this?
			    (rc=(*b)->set(*this,
					  unicode::iconvert::fromu
					  ::convert
					  (std::vector<unicode_char>(sb, se),
					   unicode::utf_8).first))
			    != 0)
			{
				error(rc, argstring);
				return;
			}
			return;
		}
	}

	// Wait for the value to come in

	optname= *b;
	optname_s=argstring_arg;

	next_func=&parserObj::collect_optvalue;
	cleanup_func=&parserObj::cleanup_optname;
}

void parserObj::collect_optvalue(const std::string &argstring) noexcept
{
	ptr<definitionbaseObj> opt=optname;

	optname=ptr<definitionbaseObj>(); // Clear the reference

	next_func=&parserObj::collect_optname;
	cleanup_func=&parserObj::cleanup_none;

	int rc=opt->set(*this, argstring);

	if (rc)
	{
		error(rc, optname_s);
		return;
	}

}

void parserObj::cleanup_optname() noexcept
{
	ptr<definitionbaseObj> opt=optname;

	optname=ptr<definitionbaseObj>(); // Clear the reference

	int rc=opt->set(*this);

	if (rc)
	{
		error(rc, optname_s);
		return;
	}

	next_func=&parserObj::collect_optname;
	cleanup_func=&parserObj::cleanup_none;
}

void parserObj::collect_params(const std::string &argstring) noexcept
{
	args.push_back(argstring);
}

int parserObj::validate() noexcept
{
	if (!err_status)
	{
		std::list<ref<definitionbaseObj> > &options=
			current_options->options;

		std::list<ref<definitionbaseObj> >::const_iterator b, e;

		for (b=options.begin(), e=options.end(); b != e; b++)
		{
			if ((*b)->flags & list::base::required &&
			    !(*b)->isSet())
			{
				std::ostringstream o;

				if ( (*b)->shortname)
				{
					std::vector<unicode_char> buf;

					buf.push_back('-');
					buf.push_back( (*b)->shortname );

					o << unicode::iconvert
						::fromu
						::convert(buf, unicode::utf_8)
						.first;
				}
				else
				{
					o << "--" << (*b)->longname;
				}

				error(parser::base::err_missingoption, o.str());
				break;
			}
		}
	}
	return err_status;
}

std::string parserObj::errmessage() const noexcept
{
	std::ostringstream o;

	switch (err_status) {
	case 0:
		break;
	case option::parser::base::err_invalidoption:
		o << gettextmsg(_("Invalid option: %1%"), err_option)
		  << std::endl;
		break;
	case option::parser::base::err_missingoption:
		o << gettextmsg(_("Missing option: %1%"), err_option)
		  << std::endl;
		break;
	case option::parser::base::err_mutexoption:
		o << gettextmsg(_("Mutually exclusive option: %1%"),
				err_option)
		  << std::endl;
		break;
	case option::parser::base::err_filenotfound:
		o << gettextmsg(_("File not found: %1%"), err_option)
		  << std::endl;
		break;
	case option::parser::base::err_builtin:
		break;
	default:
		o << gettextmsg(_("Unknown error: %1%"), err_option)
		  << std::endl;
		break;
	}
	return o.str();
}

void parserObj::help(std::ostream &o, size_t w) noexcept
{
	current_options->help(o, w);
	error(option::parser::base::err_builtin, "--help");
}

void parserObj::usage(std::ostream &o, size_t w) noexcept
{
	current_options->usage(o, w);
}

#if 0
{
	{
#endif
	}
}
