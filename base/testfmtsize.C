/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/grp.H"
#include "x/fmtsize.H"
#include "x/fd.H"
#include "x/df.H"
#include <iostream>
#include <iomanip>

#define TESTFMTSIZE(n)						\
	{							\
		std::string s=LIBCXX_NAMESPACE::fmtsize(n);	\
		std::cout << s << std::endl			\
			  << LIBCXX_NAMESPACE::			\
			parsesize(s,				\
				  LIBCXX_NAMESPACE		\
				  ::locale::base::global())	\
			  << std::endl; }

int main(int argc, char **argv)
{
	LIBCXX_NAMESPACE::fd t(LIBCXX_NAMESPACE::fd::base::tmpfile());

	(*t->getostream()) << std::setw(1023) << "" << std::flush;

	TESTFMTSIZE(t->stat().st_size) ;
	TESTFMTSIZE(1024) ;
	TESTFMTSIZE(5000) ;
	TESTFMTSIZE(1024 * 10 - 1) ;
	TESTFMTSIZE(1024 * 10) ;
	TESTFMTSIZE(1024 * 1024 - 1) ;
	TESTFMTSIZE(1024 * 1024);

	std::cout << LIBCXX_NAMESPACE::parsesize("5000",
					       LIBCXX_NAMESPACE::locale::base::global())
		  << std::endl;

	LIBCXX_NAMESPACE::df space(LIBCXX_NAMESPACE::df::create("."));

	std::cerr << LIBCXX_NAMESPACE::fmtsize(space->allocSize()
					     * space->allocFree())
		  << " (" << LIBCXX_NAMESPACE::fmtsize(space->allocSize())
		  << " allocation unit), "
		  << space->inodeFree() << " inodes left" << std::endl;

	{
		LIBCXX_NAMESPACE::df::base::reservation
			reserve(space->reserve(space->allocFree()/2,
					       space->inodeFree()/2));

		std::cerr << LIBCXX_NAMESPACE::fmtsize(space->allocSize()
						     * space->allocFree())
			  << " (" << LIBCXX_NAMESPACE::fmtsize(space->allocSize())
			  << " allocation unit), "
			  << space->inodeFree() << " inodes left" << std::endl;
	}
	{
		LIBCXX_NAMESPACE::df::base::reservation
			reserve(space->reserve(space->allocFree()+1,
					       space->inodeFree()+1));

		std::cerr << LIBCXX_NAMESPACE::fmtsize(space->allocSize()
						     * space->allocFree())
			  << " (" << LIBCXX_NAMESPACE::fmtsize(space->allocSize())
			  << " allocation unit), "
			  << space->inodeFree() << " inodes left" << std::endl;
	}

	std::cerr << LIBCXX_NAMESPACE::fmtsize(space->allocSize()
					     * space->allocFree())
		  << " (" << LIBCXX_NAMESPACE::fmtsize(space->allocSize())
		  << " allocation unit), "
		  << space->inodeFree() << " inodes left" << std::endl;


	space=LIBCXX_NAMESPACE::df::create(LIBCXX_NAMESPACE::fd::base::tmpfile());
	std::cerr << LIBCXX_NAMESPACE::fmtsize(space->allocSize()
					     * space->allocFree())
		  << " (" << LIBCXX_NAMESPACE::fmtsize(space->allocSize())
		  << " allocation unit), "
		  << space->inodeFree() << " inodes left" << std::endl;
	return 0;
}
