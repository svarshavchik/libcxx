/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/pidinfo.H"
#include "x/exception.H"
#include "x/sysexception.H"
#include "x/fd.H"
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#if 0
void testpidinfo(int argc, char **argv)
{
	LIBCXX_NAMESPACE::pidinfo p1;
	sleep(1);
	LIBCXX_NAMESPACE::pidinfo p2;

	if (p1.exe != p2.exe ||
	    p1.start_time != p2.start_time ||
	    p1.exe.size() == 0 ||
	    p1.start_time.size() == 0)
	{
		throw EXCEPTION("test 1 failed");
	}

	pid_t p=fork();

	if (p == 0)
	{
		LIBCXX_NAMESPACE::pidinfo p3;

		if (p3.exe != p2.exe || p3.start_time.size() == 0 ||
		    p3.start_time == p2.start_time)
		{
			exit(1);
		}

		exit(0);
	}

	if (p < 0)
		throw SYSEXCEPTION("fork");

	int status;

	if (wait4(p, &status, 0, 0) != p || !WIFEXITED(status))
		throw EXCEPTION("process failed");

	if (WEXITSTATUS(status) != 0)
		throw EXCEPTION("test 2 failed");

	unlink(argv[0]);

	if (LIBCXX_NAMESPACE::pidinfo().exe != p1.exe)
		throw EXCEPTION("test 3 failed");
}

int main(int argc, char **argv)
{
	try {
		testpidinfo(argc, argv);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "testpidinfo: "
			  << e << std::endl;
		exit(1);
	}
	return (0);
}

#endif

int main(int argc, char **argv)
{
	std::cout << LIBCXX_NAMESPACE::exename() << std::endl;
	std::cout << LIBCXX_NAMESPACE::fd::base::realpath(".") << std::endl;
}
