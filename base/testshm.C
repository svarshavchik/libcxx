/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fd.H"
#include "x/mmap.H"
#include "x/mmapfile.H"
#include <iostream>
#include <cstring>

void testshm()
{
	auto fd=LIBCXX_NAMESPACE::fd::base::shm_open("/testshm",
						     O_RDWR|O_CREAT, 0600);

	fd->truncate(128);

	{
		auto mmap=LIBCXX_NAMESPACE::mmap<char>
			::create(fd, PROT_READ|PROT_WRITE);
		strcpy(mmap->object(), "Hello world!");
		mmap->msync();
	}
	fd->close();

	fd=LIBCXX_NAMESPACE::fd::base::shm_open("/testshm", O_RDONLY);

	if (strcmp(LIBCXX_NAMESPACE::const_mmap<char>
		   ::create(fd, PROT_READ)->object(), "Hello world!"))
		throw EXCEPTION("Couldn't read back what I wrote");

	bool caught=false;
	try {
		LIBCXX_NAMESPACE::mmap<char>::create(fd, PROT_READ|PROT_WRITE,
						     MAP_SHARED, 0, 0);
	} catch (...)
	{
		caught=true;
	}

	if (!caught)
		throw EXCEPTION("Did not catch expected exception");

	LIBCXX_NAMESPACE::fd::base::shm_unlink("/testshm");
}

void testmmap()
{
	{
		auto fd=LIBCXX_NAMESPACE::fd::create("testshm.tmp");

		fd->write("Hello world", 12);

		auto mmap=LIBCXX_NAMESPACE::mmapfile
			::create(fd, PROT_READ|PROT_WRITE);

		if (mmap->size() != 12 ||
		    strcmp(mmap->buffer(), "Hello world"))
			throw EXCEPTION("mmapfile failed");
	}
	unlink("testshm.tmp");
}

int main()
{
	alarm(60);
	try {
		testshm();
		testmmap();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return 0;
}
