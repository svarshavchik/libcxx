/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/property_value.H"
#include "x/exception.H"
#include "x/options.H"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cerrno>

LIBCXX_NAMESPACE::property::value<int> int_value(L"intvalue", 0);

int main(int argc, char **argv)
{
	LIBCXX_NAMESPACE::option::string_list load_file_list(LIBCXX_NAMESPACE::option::string_list::create());
	LIBCXX_NAMESPACE::option::string_list load_val_list(LIBCXX_NAMESPACE::option::string_list::create());
	LIBCXX_NAMESPACE::option::string_list upd_file_list(LIBCXX_NAMESPACE::option::string_list::create());
	LIBCXX_NAMESPACE::option::string_list upd_val_list(LIBCXX_NAMESPACE::option::string_list::create());

	LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

	options->add(load_file_list, 0, L"load-files",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"Property files to pass to load_properties()",
		     L"filename")
		.add(load_val_list, 0, L"load-string",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"Property literal settings to pass to load_properties()",
		     L"name=value")
		.add(upd_file_list, 0, L"update-files",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"Property files to pass to update_properties()",
		     L"filename")
		.add(upd_val_list, 0, L"update-string",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"Property literal settings to pass to update_properties()",
		     L"name=value")
		.addDefaultOptions();

	LIBCXX_NAMESPACE::option::parser
		p(LIBCXX_NAMESPACE::option::parser::create());

	p->setOptions(options);

	int err=p->parseArgv(argc, argv);

	if (err == 0)
		err=p->validate();

	switch (err) {
	case 0:
		break;
	case LIBCXX_NAMESPACE::option::parser::base::err_builtin:
		exit(1);
	default:
		std::cerr << p->errmessage() << std::endl;
		exit(1);
	}

	try {
		LIBCXX_NAMESPACE::property::errhandler::errthrow errh;
		LIBCXX_NAMESPACE::locale cloc(LIBCXX_NAMESPACE::locale::create("C"));

		for (std::list<std::string>::iterator
			     b(load_file_list->values.begin()),
			     e(load_file_list->values.end()); b != e; ++b)
		{
			std::basic_ifstream<LIBCXX_NAMESPACE::property::propvalue
					    ::value_type> i(b->c_str());

			if (!i.is_open())
			{
				std::cerr << *b << ": " << strerror(errno);
				exit(1);
			}

			LIBCXX_NAMESPACE::property::load_properties(i, *b,
								  false, true,
								  errh, cloc);
		}

		for (std::list<std::string>::iterator
			     b(load_val_list->values.begin()),
			     e(load_val_list->values.end()); b != e; ++b)
		{
			LIBCXX_NAMESPACE::property
				::load_properties(LIBCXX_NAMESPACE::stringize
						  <LIBCXX_NAMESPACE::property
						   ::propvalue, std::string>
						  ::tostr(*b), false, true,
						  errh, cloc);
		}

		for (std::list<std::string>::iterator
			     b(upd_file_list->values.begin()),
			     e(upd_file_list->values.end()); b != e; ++b)
		{
			std::basic_ifstream<LIBCXX_NAMESPACE::property::propvalue
					    ::value_type> i(b->c_str());

			if (!i.is_open())
			{
				std::cerr << *b << ": " << strerror(errno);
				exit(1);
			}

			LIBCXX_NAMESPACE::property::load_properties(i, *b,
								  true, true,
								  errh, cloc);
		}

		for (std::list<std::string>::iterator
			     b(upd_val_list->values.begin()),
			     e(upd_val_list->values.end()); b != e; ++b)
		{
			LIBCXX_NAMESPACE::property
				::load_properties(LIBCXX_NAMESPACE::stringize
						  <LIBCXX_NAMESPACE::property
						   ::propvalue, std::string>
						  ::tostr(*b),
						  true, true,
						  errh, cloc);
		}

		for (std::list<std::string>::iterator
			     b(p->args.begin()),
			     e(p->args.end()); b != e; ++b)
		{
			std::cout <<
				LIBCXX_NAMESPACE::property::value<std::string>
				(LIBCXX_NAMESPACE::towstring(*b,
							   LIBCXX_NAMESPACE::
							   locale::base::environment()),
				 "").getValue() << std::endl;
		}
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}
