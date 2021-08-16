/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/forkexec.H"
#include "x/exception.H"
#include "x/sysexception.H"

#include <iostream>

void testforkexec(int argc, char **argv)
{
	if (LIBCXX_NAMESPACE::forkexec("sh", "-c", "echo yes")
	    .system())
	{
		throw EXCEPTION("Can't echo \"yes\"?");
	}

	bool caught=false;
	try {
		std::vector<std::string> v;

		v.push_back("sh");
		v.push_back("-c");
		v.push_back("echo yes");

		LIBCXX_NAMESPACE::forkexec(v).program("./doesnotexist")
			.system();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Caught expected exception: "
			  << e << std::endl;
		caught=true;
	}

	if (!caught)
		throw EXCEPTION("Something's wrong with error checking");


	{
		LIBCXX_NAMESPACE::forkexec fe("sh", "-c", "echo yes");

		auto fd=fe.pipe_from(1);

		pid_t p=fe.spawn();

		std::string l;

		std::getline(*fd->getistream(), l);
		if (LIBCXX_NAMESPACE::forkexec::wait4(p) ||
		    l != "yes")
		{
			throw EXCEPTION("Did not capture stdout");
		}
	}

	{
		LIBCXX_NAMESPACE::forkexec fe("sh", "-c", "wc -l");

		auto stdin=fe.pipe_to(0);
		auto stdout=fe.socket_fd(1);

		fe.spawn_detached();

		(*stdin->getostream()) << "one" << std::endl
				       << "two" << std::endl << std::flush;
		stdin->close();

		int n=0;

		(*stdout->getistream()) >> n;

		if (n != 2)
			throw EXCEPTION("Something wrong with spawn_detached()");
	}

	caught=false;

	try
	{
		LIBCXX_NAMESPACE::forkexec("./doesnotexist").spawn_detached();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Caught expected exception: " << e <<std::endl;
		caught=true;
	}
	if (!caught)
		throw EXCEPTION("Something's wrong with error checking");

	caught=false;
	try
	{
		LIBCXX_NAMESPACE::forkexec("./doesnotexist").exec();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Caught expected exception: " << e <<std::endl;
		caught=true;
	}

	if (!caught)
		throw EXCEPTION("How did I get here?");

	LIBCXX_NAMESPACE::forkexec("true").exec();
}

int main(int argc, char **argv)
{
	try {
		testforkexec(argc, argv);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "testforkexec: "
			  << e << std::endl;
		exit(1);
	}
	return (0);
}

