/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/httportmap.H"
#include "x/options.H"
#include "x/fileattr.H"
#include "x/pidinfo.H"

int main(int argc, char **argv)
{
	LIBCXX_NAMESPACE::option::string_value
		svcname_value(LIBCXX_NAMESPACE::option::string_value
			      ::create("test"));

	LIBCXX_NAMESPACE::option::bool_value
		svcexcl_value(LIBCXX_NAMESPACE::option::bool_value::create());

	LIBCXX_NAMESPACE::option::bool_value
		pid2exe_value(LIBCXX_NAMESPACE::option::bool_value::create());

	LIBCXX_NAMESPACE::option::string_list
		reg_values(LIBCXX_NAMESPACE::option::string_list::create());

	LIBCXX_NAMESPACE::option::string_list
		dereg_values(LIBCXX_NAMESPACE::option::string_list::create());

	LIBCXX_NAMESPACE::option::string_list
		reg_values2(LIBCXX_NAMESPACE::option::string_list::create());

	LIBCXX_NAMESPACE::option::list
		options(LIBCXX_NAMESPACE::option::list::create());

	options->add(reg_values,
		     0, "register",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "Register port for debugging purposes",
		     "port")
		.add(dereg_values,
		     0, "deregister",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "Then deregister this port for debugging purposes",
		     "port")
		.add(reg_values2,
		     0, "register2",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "Register a second port for debugging",
		     "port")
		.add(svcname_value,
		     0, "name",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "Service name for test registration")
		.add(svcexcl_value,
		     0, "exclusive",
		     0,
		     "Register exclusive services")
		.add(pid2exe_value,
		     0, "pid2exe",
		     0,
		     "Test pid2exe()")
		.addDefaultOptions();

	LIBCXX_NAMESPACE::option::parser
		optionParser(LIBCXX_NAMESPACE::option::parser::create());

	optionParser->setOptions(options);

	int err=optionParser->parseArgv(argc, argv);

	if (err == 0)
		err=optionParser->validate();

	switch (err) {
	case 0:
		break;
	case LIBCXX_NAMESPACE::option::parser::base::err_builtin:
		exit(1);
	default:
		std::cerr << optionParser->errmessage();
		exit(1);
	}

	try {
		LIBCXX_NAMESPACE::httportmap
			portmapper(LIBCXX_NAMESPACE::httportmap
				   ::create(""));

		if (!reg_values->values.empty())
		{
			std::list<LIBCXX_NAMESPACE::httportmap::base::reginfo> svcs;

			for (std::list<std::string>::const_iterator
				     b(reg_values->values.begin()),
				     e(reg_values->values.end()); b != e; ++b)
			{
				LIBCXX_NAMESPACE::httportmap::base::reginfo r;

				r.name=svcname_value->value;
				r.port= *b;
				r.flags=svcexcl_value->isSet() ?
					LIBCXX_NAMESPACE::httportmap::base
					::pm_exclusive:0;

				svcs.push_back(r);
			}

			if (!portmapper->reg(svcs))
				std::cerr << svcname_value->value
					  << ": failed to register"
					  << std::endl;
		}

		for (std::list<std::string>::const_iterator
			     b(dereg_values->values.begin()),
			     e(dereg_values->values.end()); b != e; ++b)
		{
			portmapper->dereg(svcname_value->value, *b);
		}

		if (!reg_values2->values.empty())
		{
			std::list<LIBCXX_NAMESPACE::httportmap::base::reginfo> svcs;

			for (std::list<std::string>::const_iterator
				     b(reg_values2->values.begin()),
				     e(reg_values2->values.end()); b != e; ++b)
			{
				LIBCXX_NAMESPACE::httportmap::base::reginfo r;

				r.name=svcname_value->value;
				r.port= *b;
				r.flags=svcexcl_value->isSet();
				svcs.push_back(r);
			}

			if (!portmapper->reg(svcs))
			{
				std::cerr << svcname_value->value
					  << ": failed to register"
					  << std::endl;
			}
		}

		if (pid2exe_value->value)
		{
			portmapper->regpid2exe();

			std::string s=portmapper->pid2exe(getpid());

			if (!s.size())
				throw EXCEPTION("pid2exe() failed");


			auto stat1=
				LIBCXX_NAMESPACE::fileattr::create(s)->stat();
			auto stat2=
				LIBCXX_NAMESPACE::fileattr
				::create(LIBCXX_NAMESPACE::exename())
				->stat();

			if (stat1.st_dev != stat2.st_dev ||
			    stat1.st_ino != stat2.st_ino)
				throw EXCEPTION("pid2exe() mismatch");

		}

		typedef std::list<LIBCXX_NAMESPACE::httportmap::base::service>
			services_t;

		services_t services;

		portmapper->list(std::back_insert_iterator<services_t>(services));

		for (services_t::const_iterator b(services.begin()),
			     e(services.end()); b != e; ++b)
			std::cout << b->getName() << '\t'
				  << b->getUser() << '\t'
				  << b->getPid() << '\t'
				  << b->getPort() << std::endl;
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
	}
	return 0;
}
