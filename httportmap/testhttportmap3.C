/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/httportmap.H"
#include "x/options.H"
#include "x/globlock.H"
#include "x/sysexception.H"

#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
	alarm(30);

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
			    ::create(""));

		pm1->reg("test1.libx", "0", 0);

		typedef	std::vector<LIBCXX_NAMESPACE::httportmap::base::service>
			svclist_t;

		typedef std::back_insert_iterator<svclist_t> ins_iter_t;

		svclist_t l;

		pm1->list(ins_iter_t(l), "test1.libx");

		if (l.size() != 1 ||
		    l[0].getPort() != "0")
		{
			throw EXCEPTION("test1 failed");
		}

		l.clear();

		pm1->list(ins_iter_t(l),
			  LIBCXX_NAMESPACE::httportmap::base::portmap_service,
			  (uid_t)0);

		if (l.size() != 1)
			throw EXCEPTION(std::string("Cannot find ") +
					LIBCXX_NAMESPACE::httportmap::base
					::portmap_service);

		pid_t orig_pid=l[0].getPid();

		pid_t p=LIBCXX_NAMESPACE::fork();

		if (p < 0)
			throw SYSEXCEPTION("fork");

		if (p == 0)
		{
			std::vector<std::vector<char>> charbuf;

			std::vector<char *> argvec;

			for (const auto &word:optionParser->args)
			{
				charbuf.push_back(std::vector<char>
						  (word.begin(), word.end()));

				auto &last=charbuf.back();

				last.push_back(0);
				argvec.push_back(&last[0]);
			}
			argvec.push_back(nullptr);
			execv(optionParser->args.front().c_str(), &argvec[0]);
			perror(optionParser->args.front().c_str());
			exit(0);
		}

		int status;

		if (waitpid(p, &status, 0) < 0)
			throw EXCEPTION("wait");

		if (status != 0)
		{
			std::cerr << "Error: child process exited with a non-zero exit code"
				  << std::endl;
		}

		l.clear();

		pm1->list(ins_iter_t(l), "test1.libx");

		if (l.size() != 1 ||
		    l[0].getPort() != "0")
		{
			throw EXCEPTION("test2 failed");
		}

		l.clear();

		pm1->list(ins_iter_t(l),
			  LIBCXX_NAMESPACE::httportmap::base::portmap_service,
			  (uid_t)0);

		if (l.size() != 1)
			throw EXCEPTION(std::string("Cannot find ") +
					LIBCXX_NAMESPACE::httportmap::base
					::portmap_service);

		pid_t new_pid=l[0].getPid();

		if (new_pid == orig_pid)
			throw EXCEPTION("test3 failed");

		std::cout << "yes" << std::endl;

	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
	}
	return 0;
}
