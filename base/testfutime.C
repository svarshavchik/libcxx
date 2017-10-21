/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/obj.H"
#include "x/ptr.H"
#include "x/fd.H"
#include "x/timespec.H"

int main()
{
	try {
		LIBCXX_NAMESPACE::fd fd(LIBCXX_NAMESPACE::fd::base::tmpfile());

		time_t t=fd->stat().st_atime;

		LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj> mcguffin=
			fd->futimens_interval(std::chrono::milliseconds(250));

		sleep(2);

		if (fd->stat().st_atime == t)
			throw EXCEPTION("File access time did not update");

		mcguffin=LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>();

		t=fd->stat().st_atime;

		sleep(2);

		if (fd->stat().st_atime != t)
			throw EXCEPTION("File access time keeps getting updated");
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}

	return 0;
}
