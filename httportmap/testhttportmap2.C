/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/httportmap.H"
#include "x/options.H"

int main(int argc, char **argv)
{
	LIBCXX_NAMESPACE::option::list
		options(LIBCXX_NAMESPACE::option::list::create());

	options->addDefaultOptions();

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
		LIBCXX_NAMESPACE::httportmapptr
			pm1(LIBCXX_NAMESPACE::httportmap
			    ::create("")),
			pm2(LIBCXX_NAMESPACE::httportmap
			    ::create(""));

		pm1->reg("test1.libx", "0", 0);
		pm2->reg("test2.libx", "0", 0);
		typedef	std::vector<LIBCXX_NAMESPACE::httportmap::base::service>
			svclist_t;

		typedef std::back_insert_iterator<svclist_t> ins_iter_t;

		svclist_t l;

		pm2->list(ins_iter_t(l), "test1.libx");

		if (l.size() != 1 ||
		    l[0].getPort() != "0")
		{
			throw EXCEPTION("test1 failed");
		}

		l.clear();
		pm2->list(ins_iter_t(l), "test2.libx");

		if (l.size() != 1 ||
		    l[0].getPort() != "0")
		{
			throw EXCEPTION("test2 failed");
		}

		pm1=LIBCXX_NAMESPACE::httportmapptr();
		l.clear();

		pm2->list(ins_iter_t(l), "test1.libx");

		if (l.size() > 0)
		{
			throw EXCEPTION("test3 failed");
		}
		pm2->list(ins_iter_t(l), "test2.libx");

		if (l.size() != 1 ||
		    l[0].getPort() != "0")
		{
			throw EXCEPTION("test4 failed");
		}

	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
	}
	return 0;
}
